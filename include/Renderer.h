#pragma once

namespace MTL {
class Device;
class CommandQueue;
class CommandBuffer;
class RenderPassDescriptor;
class RenderCommandEncoder;
class Texture;
} // namespace MTL

namespace CA {
class MetalLayer;
}

class Renderer {
public:
  Renderer(MTL::Device *device, CA::MetalLayer *layer);
  ~Renderer();
  void draw();
  MTL::Texture *getOutputTexture() const;
  MTL::Texture *getInputTexture() const;

private:
  MTL::Device *_device;
  CA::MetalLayer *_layer;
  MTL::CommandQueue *_commandQueue;

  void createRenderPipeline();
  void encodeRenderCommands(MTL::RenderCommandEncoder *renderCommandEncoder);
};
