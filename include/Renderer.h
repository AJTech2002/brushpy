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
  static void create(Canvas *attached, CA::MetalLayer *layer);
  static void destroy();

  void draw();
  void init();

  static int width() { return instance()._width; }
  static int height() { return instance()._height; }

  static void addDrawCallback(std::function<void(Renderer *)> callback) {
    drawCallbacks.push_back(callback);
  }

private:
  Renderer(Canvas *attached, CA::MetalLayer *layer);
  ~Renderer();
  Renderer(const Renderer &) = delete;
  Renderer &operator=(const Renderer &) = delete;

  static Renderer *_instance;

  MTL::Buffer *_quadBuffer;
  MTL::RenderPipelineState *_metalRenderPSO;
  CA::MetalLayer *_layer;

  // List of draw callbacks
  static std::vector<std::function<void(Renderer *)>> drawCallbacks;

  static bool ready;
  int _width;
  int _height;

  Canvas *_canvas;

  void createRenderPipeline();
  void encodeRenderCommands(MTL::RenderCommandEncoder *renderCommandEncoder);

  // Compiles shaderFiles into a single Metal library at runtime, instead of
  // relying on a default.metallib next to the executable (which doesn't
  // exist when running embedded in the Python interpreter).
};
