#define NS_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#include "renderer.h"
#include "app.h"
#include "canvas.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <simd/simd.h>
#include <sstream>

// Two triangles to make a quad
simd::float3 quadVertices[] = {{-1.0f, -1.0f, 0.0f}, {1.0f, -1.0f, 0.0f},
                               {-1.0f, 1.0f, 0.0f},  {1.0f, 1.0f, 0.0f},
                               {-1.0f, 1.0f, 0.0f},  {1.0f, -1.0f, 0.0f}};

Renderer *Renderer::_instance = nullptr;

std::vector<std::function<void(Renderer *)>> Renderer::drawCallbacks;
bool Renderer::ready = false;

Renderer &Renderer::instance() {
  assert(_instance != nullptr &&
         "Renderer::init() must be called before instance()");
  return *_instance;
}

void Renderer::create(MTL::Device *device, CA::MetalLayer *layer) {

  _instance = new Renderer(device, layer);
}

MTL::Texture *Renderer::createTexture(int width, int height,
                                      std::string label) {
  MTL::TextureDescriptor *desc = MTL::TextureDescriptor::alloc()->init();
  desc->setTextureType(MTL::TextureType2D);
  desc->setPixelFormat(MTL::PixelFormatRGBA32Float);
  desc->setWidth(width);
  desc->setHeight(height);
  desc->setStorageMode(MTL::StorageModeShared);
  desc->setUsage(MTL::TextureUsageRenderTarget | MTL::TextureUsageShaderRead |
                 MTL::TextureUsageShaderWrite);

  MTL::Texture *texture = instance()._device->newTexture(desc);
  texture->setLabel(NS::String::string(label.c_str(), NS::ASCIIStringEncoding));

  desc->release();
  return texture;
}

MTL::Texture *Renderer::loadTexture(const char *imagePath, std::string label) {
  int width, height, channels;
  stbi_uc *pixels = stbi_load(imagePath, &width, &height, &channels,
                              STBI_rgb_alpha); // force RGBA8
  if (!pixels)
    return nullptr;

  MTL::TextureDescriptor *desc = MTL::TextureDescriptor::alloc()->init();
  desc->setWidth(width);
  desc->setHeight(height);
  desc->setPixelFormat(MTL::PixelFormatRGBA8Unorm);
  desc->setTextureType(MTL::TextureType2D);
  desc->setStorageMode(MTL::StorageModeShared); // or Managed on macOS
  desc->setUsage(MTL::TextureUsageShaderRead);

  MTL::Texture *texture = instance()._device->newTexture(desc);
  desc->release();

  MTL::Region region = MTL::Region(0, 0, 0, width, height, 1);
  NS::UInteger bytesPerRow = width * 4;
  texture->replaceRegion(region, 0, pixels, bytesPerRow);

  stbi_image_free(pixels);
  return texture;
}

Renderer::Renderer(MTL::Device *device, CA::MetalLayer *layer)
    : _device(device), _layer(layer) {
  _commandQueue = _device->newCommandQueue();
  _quadBuffer = _device->newBuffer(&quadVertices, sizeof(quadVertices),
                                   MTL::ResourceStorageModeShared);
  _defaultLibrary = compileShaderLibrary();
  createRenderPipeline();

  std::cout << "BrushPY Renderer ready, created Quad Buffer & Shader Library"
            << std::endl;
}

MTL::Library *Renderer::compileShaderLibrary() {
  std::filesystem::path shaderDir =
      std::filesystem::path(__FILE__).parent_path().parent_path() / "shaders";

  std::string source;
  for (const auto &file : shaderFiles) {
    std::filesystem::path path = shaderDir / file;
    std::ifstream in(path);
    if (!in) {
      std::cerr << "Renderer: failed to open shader file " << path << std::endl;
      continue;
    }
    std::stringstream contents;
    contents << in.rdbuf();
    source += contents.str();
    source += "\n";
  }

  NS::Error *error = nullptr;
  MTL::Library *library = _device->newLibrary(
      NS::String::string(source.c_str(), NS::UTF8StringEncoding), nullptr,
      &error);

  if (!library) {
    std::cerr << "Renderer: failed to compile shader library: "
              << (error ? error->localizedDescription()->utf8String()
                        : "unknown error")
              << std::endl;
  }

  return library;
}

void Renderer::init() {
  _outputTexture = createTexture(WIDTH, HEIGHT);

  // _canvas = new Canvas(WIDTH, HEIGHT);
  // _canvas->init(this);
  ready = true;
}

void Renderer::createRenderPipeline() {
  // This automatically finds the vertex function in any metal files
  MTL::Function *vertexShader = _defaultLibrary->newFunction(
      NS::String::string("vertexShader", NS::ASCIIStringEncoding));
  MTL::Function *fragmentShader = _defaultLibrary->newFunction(
      NS::String::string("fragmentShader", NS::ASCIIStringEncoding));

  MTL::RenderPipelineDescriptor *renderPipelineDescriptor =
      MTL::RenderPipelineDescriptor::alloc()->init();
  renderPipelineDescriptor->setLabel(
      NS::String::string("Quad Pipeline", NS::ASCIIStringEncoding));
  renderPipelineDescriptor->setVertexFunction(vertexShader);
  renderPipelineDescriptor->setFragmentFunction(fragmentShader);

  assert(renderPipelineDescriptor != nullptr);

  MTL::PixelFormat pixelFormat = (MTL::PixelFormat)_layer->pixelFormat();
  renderPipelineDescriptor->colorAttachments()->object(0)->setPixelFormat(
      pixelFormat);

  NS::Error *error;
  _metalRenderPSO =
      _device->newRenderPipelineState(renderPipelineDescriptor, &error);

  vertexShader->release();
  fragmentShader->release();
  renderPipelineDescriptor->release();
}

void Renderer::encodeRenderCommands(
    MTL::RenderCommandEncoder *renderCommandEncoder) {
  renderCommandEncoder->setRenderPipelineState(_metalRenderPSO);
  renderCommandEncoder->setVertexBuffer(_quadBuffer, 0, 0);
  MTL::PrimitiveType type = MTL::PrimitiveTypeTriangle;
  NS::UInteger vertexStart = 0;
  NS::UInteger vertexCount = sizeof(quadVertices) / sizeof(simd::float3);

  renderCommandEncoder->setFragmentTexture(_outputTexture, 0);
  renderCommandEncoder->drawPrimitives(type, vertexStart, vertexCount);
}

void Renderer::draw() {
  CA::MetalDrawable *drawable = _layer->nextDrawable();
  if (!drawable)
    return;

  MTL::RenderPassDescriptor *descriptor =
      MTL::RenderPassDescriptor::renderPassDescriptor();

  auto *color = descriptor->colorAttachments()->object(0);
  color->setTexture(drawable->texture());
  color->setLoadAction(MTL::LoadActionClear);
  color->setClearColor(MTL::ClearColor(0.0, 0.0, 1.0, 1.0));
  color->setStoreAction(MTL::StoreActionStore);

  _activeCommandBuffer = _commandQueue->commandBuffer();

  for (auto &callback : drawCallbacks) {
    callback(this);
  }

  _canvas->draw(this);

  drawCallbacks.clear();

  MTL::RenderCommandEncoder *enc =
      _activeCommandBuffer->renderCommandEncoder(descriptor);
  encodeRenderCommands(enc);
  enc->endEncoding();

  _activeCommandBuffer->presentDrawable(drawable);
  _activeCommandBuffer->commit();
  _activeCommandBuffer->waitUntilCompleted();
  _activeCommandBuffer = nullptr;
}

Renderer::~Renderer() { _commandQueue->release(); }

void Renderer::destroy() {
  _instance->_canvas->dispose();
  delete _instance;
  _instance = nullptr;
}
