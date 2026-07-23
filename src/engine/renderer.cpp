
#include "renderer.h"
#include "canvas.h"
#define STB_IMAGE_IMPLEMENTATION
#include "engine.h"
#include "stb_image.h"
#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>
#include <iostream>
#include <simd/simd.h>

// Shared variables
simd::float3 quadVertices[] = {{-1.0f, -1.0f, 0.0f}, {1.0f, -1.0f, 0.0f},
                               {-1.0f, 1.0f, 0.0f},  {1.0f, 1.0f, 0.0f},
                               {-1.0f, 1.0f, 0.0f},  {1.0f, -1.0f, 0.0f}};
Renderer *Renderer::_instance = nullptr;
std::vector<std::function<void(Renderer *)>> Renderer::drawCallbacks;
bool Renderer::ready = false;

void Renderer::create(Canvas *canvas, CA::MetalLayer *layer) {
  _instance = new Renderer(canvas, layer);
}
// --

Renderer::Renderer(Canvas *canvas, CA::MetalLayer *layer) {
  _canvas = canvas;
  _layer = layer;
  _quadBuffer = Engine::device()->newBuffer(&quadVertices, sizeof(quadVertices),
                                            MTL::ResourceStorageModeShared);
  createRenderPipeline();

  std::cout << "BrushPY Renderer ready, created Quad Buffer & Shader Library"
            << std::endl;
}

Renderer &Renderer::instance() {
  // assert(_instance != nullptr &&
  //  "Renderer::init() must be called before instance()");
  if (_instance == nullptr) {
    std::cerr << "Error: Renderer::init() must be called before instance()"
              << std::endl;
  }
  return *_instance;
}

void Renderer::init() {
  _width = _canvas->width();
  _height = _canvas->height();
  ready = true;
}

void Renderer::createRenderPipeline() {
  // This automatically finds the vertex function in any metal files
  MTL::Function *vertexShader = Engine::defaultLibrary()->newFunction(
      NS::String::string("vertexShader", NS::ASCIIStringEncoding));
  MTL::Function *fragmentShader = Engine::defaultLibrary()->newFunction(
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
  _metalRenderPSO = Engine::device()->newRenderPipelineState(
      renderPipelineDescriptor, &error);

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

  renderCommandEncoder->setFragmentTexture(_canvas->outputTexture(), 0);
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

  MTL::CommandBuffer *commandBuffer =
      Engine::beginCommandBuffer("Renderer::draw");

  for (auto &callback : drawCallbacks) {
    callback(this);
  }

  _canvas->draw();

  drawCallbacks.clear();

  MTL::RenderCommandEncoder *enc =
      commandBuffer->renderCommandEncoder(descriptor);
  encodeRenderCommands(enc);
  enc->endEncoding();

  commandBuffer->presentDrawable(drawable);
  Engine::endCommandBuffer(true);
}

Renderer::~Renderer() {}

void Renderer::destroy() {
  // _instance->_canvas->dispose();
  delete _instance;
  _instance = nullptr;
}
