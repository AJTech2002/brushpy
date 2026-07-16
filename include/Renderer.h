#pragma once

namespace MTL {
class Device;
class CommandQueue;
class CommandBuffer;
class RenderPassDescriptor;
class RenderCommandEncoder;
class Texture;
class Library;
class RenderPipelineState;
class Buffer;
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

  static MTL::CommandBuffer *activeCommandBuffer() {
    return instance()._activeCommandBuffer;
  }

  static MTL::Library *getDefaultLibrary() {
    return instance()._defaultLibrary;
  }

  static MTL::Device *getDevice() { return instance()._device; }

  static MTL::Texture *getOutputTexture() { return instance().outputTexture; }

private:
  Renderer(MTL::Device *device, CA::MetalLayer *layer);
  ~Renderer();
  Renderer(const Renderer &) = delete;
  Renderer &operator=(const Renderer &) = delete;

  static Renderer *_instance;

  MTL::Device *_device;
  CA::MetalLayer *_layer;
  MTL::CommandQueue *_commandQueue;
  MTL::CommandBuffer *_activeCommandBuffer;
  MTL::Buffer *_quadBuffer;
  MTL::Library *_defaultLibrary;
  MTL::RenderPipelineState *_metalRenderPSO;

  MTL::Texture *outputTexture;

  void createRenderPipeline();
  void encodeRenderCommands(MTL::RenderCommandEncoder *renderCommandEncoder);
};
