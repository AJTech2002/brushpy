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
  static Renderer &instance();
  static void init(MTL::Device *device, CA::MetalLayer *layer);
  static void destroy();

  void draw();
  MTL::Texture *getOutputTexture() const;
  MTL::Texture *getInputTexture() const;

private:
  Renderer(MTL::Device *device, CA::MetalLayer *layer);
  ~Renderer();
  Renderer(const Renderer &) = delete;
  Renderer &operator=(const Renderer &) = delete;

  static Renderer *_instance;

  MTL::Device *_device;
  CA::MetalLayer *_layer;
  MTL::CommandQueue *_commandQueue;

  void createRenderPipeline();
  void encodeRenderCommands(MTL::RenderCommandEncoder *renderCommandEncoder);
};
