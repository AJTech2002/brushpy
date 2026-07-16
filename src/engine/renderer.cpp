#define NS_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#include "renderer.h"
#include "app.h"
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
MTL::ComputePipelineState *metalComputePSO;

MTL::Texture *outputBufferA;
MTL::Texture *outputBufferB;
bool renderBufferA = true;

Renderer *Renderer::_instance = nullptr;

Renderer &Renderer::instance() { return *_instance; }

void Renderer::init(MTL::Device *device, CA::MetalLayer *layer) {
  _instance = new Renderer(device, layer);
}

void Renderer::destroy() {
  delete _instance;
  _instance = nullptr;
}

Renderer::Renderer(MTL::Device *device, CA::MetalLayer *layer)
    : _device(device), _layer(layer) {
  _commandQueue = _device->newCommandQueue();
  quadBuffer = _device->newBuffer(&quadVertices, sizeof(quadVertices),
                                  MTL::ResourceStorageModeShared);
  defaultLibrary = _device->newDefaultLibrary();
  createRenderPipeline();

  MTL::TextureDescriptor *textureDescriptor =
      MTL::TextureDescriptor::alloc()->init();
  textureDescriptor->setTextureType(MTL::TextureType2D);
  textureDescriptor->setPixelFormat(MTL::PixelFormatRGBA32Float);
  textureDescriptor->setWidth(WIDTH);
  textureDescriptor->setHeight(HEIGHT);

  textureDescriptor->setStorageMode(MTL::StorageModeShared);

  // These textures will be used as output buffers for the compute shader
  outputBufferA = _device->newTexture(textureDescriptor);
  outputBufferB = _device->newTexture(textureDescriptor);

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

  MTL::Function *computeShader = defaultLibrary->newFunction(
      NS::String::string("computeShader", NS::ASCIIStringEncoding));

  MTL::ComputePipelineDescriptor *computePipelineDescriptor =
      MTL::ComputePipelineDescriptor::alloc()->init();
  computePipelineDescriptor->setComputeFunction(computeShader);

  metalComputePSO = _device->newComputePipelineState(computeShader, &error);
  computeShader->release();
}

void Renderer::encodeRenderCommands(
    MTL::RenderCommandEncoder *renderCommandEncoder) {
  renderCommandEncoder->setRenderPipelineState(metalRenderPSO);
  renderCommandEncoder->setVertexBuffer(quadBuffer, 0, 0);
  MTL::PrimitiveType type = MTL::PrimitiveTypeTriangle;
  NS::UInteger vertexStart = 0;
  NS::UInteger vertexCount = sizeof(quadVertices) / sizeof(simd::float3);

  // setup the output texture based on the current render buffer
  MTL::Texture *currentOutputBuffer =
      renderBufferA ? outputBufferA : outputBufferB;
  renderCommandEncoder->setFragmentTexture(currentOutputBuffer, 0);

  renderBufferA = !renderBufferA;

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

  MTL::ComputeCommandEncoder *computeEncoder = cmd->computeCommandEncoder();
  computeEncoder->setComputePipelineState(metalComputePSO);

  MTL::Texture *inputBuffer = renderBufferA ? outputBufferB : outputBufferA;
  MTL::Texture *currentOutputBuffer =
      renderBufferA ? outputBufferB : outputBufferA;

  renderBufferA = !renderBufferA;

  computeEncoder->setTexture(currentOutputBuffer, 0);
  computeEncoder->setTexture(inputBuffer, 1);

  MTL::Size gridSize = MTL::Size(WIDTH, HEIGHT, 1);
  MTL::Size threadGroupSize = MTL::Size(16, 16, 1);
  computeEncoder->dispatchThreads(gridSize, threadGroupSize);
  computeEncoder->endEncoding();

  MTL::RenderCommandEncoder *enc = cmd->renderCommandEncoder(descriptor);
  encodeRenderCommands(enc);
  enc->endEncoding();

  cmd->presentDrawable(drawable);
  cmd->commit();
  cmd->waitUntilCompleted();
}
