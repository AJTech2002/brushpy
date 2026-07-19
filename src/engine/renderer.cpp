#define NS_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#include "renderer.h"
#include "app.h"
#include "canvas.h"
#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>
#include <iostream>
#include <simd/simd.h>

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

Renderer::Renderer(MTL::Device *device, CA::MetalLayer *layer)
    : _device(device), _layer(layer) {
  _commandQueue = _device->newCommandQueue();
  _quadBuffer = _device->newBuffer(&quadVertices, sizeof(quadVertices),
                                   MTL::ResourceStorageModeShared);
  _defaultLibrary = _device->newDefaultLibrary();
  createRenderPipeline();

  std::cout << "BrushPY Renderer ready, created Quad Buffer & Default Library"
            << std::endl;
}

void Renderer::init() {
  _outputTexture = createTexture(WIDTH, HEIGHT);

  _canvas = new Canvas(WIDTH, HEIGHT);
  _canvas->init(this);
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
