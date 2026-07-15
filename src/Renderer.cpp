#define NS_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#include "Renderer.h"
#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>
#include <iostream>
#include <simd/simd.h>

// Two triangles to make a quad
simd::float3 quadVertices[] = {{-1.0f, -1.0f, 0.0f}, {1.0f, -1.0f, 0.0f},
                               {-1.0f, 1.0f, 0.0f},  {1.0f, 1.0f, 0.0f},
                               {-1.0f, 1.0f, 0.0f},  {1.0f, -1.0f, 0.0f}};

MTL::Buffer *quadBuffer;
MTL::Library *defaultLibrary;
MTL::RenderPipelineState *metalRenderPSO;

Renderer::Renderer(MTL::Device *device, CA::MetalLayer *layer)
    : _device(device), _layer(layer) {
  _commandQueue = _device->newCommandQueue();
  quadBuffer = _device->newBuffer(&quadVertices, sizeof(quadVertices),
                                  MTL::ResourceStorageModeShared);
  defaultLibrary = _device->newDefaultLibrary();
  createRenderPipeline();
  std::cout << "BrushPY Renderer ready, created Quad Buffer & Default Library"
            << std::endl;
}

Renderer::~Renderer() { _commandQueue->release(); }

void Renderer::createRenderPipeline() {
  // This automatically finds the vertex function in any metal files
  MTL::Function *vertexShader = defaultLibrary->newFunction(
      NS::String::string("vertexShader", NS::ASCIIStringEncoding));
  MTL::Function *fragmentShader = defaultLibrary->newFunction(
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
  metalRenderPSO =
      _device->newRenderPipelineState(renderPipelineDescriptor, &error);

  vertexShader->release();
  fragmentShader->release();
  renderPipelineDescriptor->release();
}

void Renderer::encodeRenderCommands(
    MTL::RenderCommandEncoder *renderCommandEncoder) {
  renderCommandEncoder->setRenderPipelineState(metalRenderPSO);
  renderCommandEncoder->setVertexBuffer(quadBuffer, 0, 0);
  MTL::PrimitiveType type = MTL::PrimitiveTypeTriangle;
  NS::UInteger vertexStart = 0;
  NS::UInteger vertexCount = sizeof(quadVertices) / sizeof(simd::float3);
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

  MTL::CommandBuffer *cmd = _commandQueue->commandBuffer();
  MTL::RenderCommandEncoder *enc = cmd->renderCommandEncoder(descriptor);
  encodeRenderCommands(enc);
  enc->endEncoding();

  cmd->presentDrawable(drawable);
  cmd->commit();
  cmd->waitUntilCompleted();
}
