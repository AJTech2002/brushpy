#define NS_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>

#include "Renderer.h"

Renderer::Renderer(MTL::Device *device, CA::MetalLayer *layer)
    : _device(device), _layer(layer) {
  _commandQueue = _device->newCommandQueue();
}

Renderer::~Renderer() { _commandQueue->release(); }

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
  enc->endEncoding();

  cmd->presentDrawable(drawable);
  cmd->commit();
  // cmd and descriptor are autoreleased (not new/copy/alloc) — do not release.
}
