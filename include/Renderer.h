#pragma once

namespace MTL {
class Device;
class CommandQueue;
class CommandBuffer;
class RenderPassDescriptor;
class RenderCommandEncoder;

} // namespace MTL

namespace CA {
class MetalLayer;
}

class Renderer {
public:
  Renderer(MTL::Device *device, CA::MetalLayer *layer);
  ~Renderer();
  void draw();

private:
  MTL::Device *_device;
  CA::MetalLayer *_layer;
  MTL::CommandQueue *_commandQueue;

  void createRenderPipeline();
  void encodeRenderCommands(MTL::RenderCommandEncoder* renderCommandEncoder);
};
