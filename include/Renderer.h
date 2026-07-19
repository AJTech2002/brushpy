#pragma once

#include "string"
#include <functional>
#include <vector>

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

class Canvas;

class Renderer {
public:
  static Renderer &instance();
  static void create(MTL::Device *device, CA::MetalLayer *layer);
  static void destroy();

  void draw();
  void init();

  static MTL::CommandBuffer *activeCommandBuffer() {
    return instance()._activeCommandBuffer;
  }

  static MTL::Library *defaultLibrary() { return instance()._defaultLibrary; }

  static MTL::Device *device() { return instance()._device; }

  static MTL::Texture *outputTexture() { return instance()._outputTexture; }

  static MTL::Texture *createTexture(int width, int height,
                                     std::string label = "RendererTexture");

  static Canvas *canvas() { return instance()._canvas; }

  static bool isReady() { return ready; }

  static void addDrawCallback(std::function<void(Renderer *)> callback) {
    drawCallbacks.push_back(callback);
  }

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

  MTL::Texture *_outputTexture;

  // List of draw callbacks
  static std::vector<std::function<void(Renderer *)>> drawCallbacks;

  Canvas *_canvas;
  static bool ready;

  void createRenderPipeline();
  void encodeRenderCommands(MTL::RenderCommandEncoder *renderCommandEncoder);
};
