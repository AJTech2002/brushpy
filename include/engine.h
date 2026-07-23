#pragma once
#include "string"

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

class Engine {

public:
  Engine();

  static Engine &instance() { return *_instance; }

  static MTL::Library *defaultLibrary() { return instance()._defaultLibrary; }

  static MTL::Device *device() { return instance()._device; }

  static MTL::Texture *createTexture(int width, int height,
                                     std::string label = "RendererTexture");

  static MTL::Texture *loadTexture(const char *imagePath,
                                   std::string label = "RendererTexture");

  static bool isReady() { return ready; }

  void init();

  static MTL::CommandQueue *commandQueue() { return instance()._commandQueue; }

  static MTL::CommandBuffer *activeCommandBuffer() {
    return instance()._activeCommandBuffer;
  }

  // Begins a command buffer for the calling rasterization stage (Compute,
  // Canvas, Layer, Renderer, ...). Calls nest: if one is already active it is
  // reused and returned, so a headless Canvas::render() call composes with an
  // outer Renderer::draw() pass without either knowing about the other.
  static MTL::CommandBuffer *
  beginCommandBuffer(std::string label = "CommandBuffer");

  // Matches a beginCommandBuffer() call. Only the outermost, unmatched call
  // actually commits (and optionally waits on) the buffer.
  static void endCommandBuffer(bool waitUntilCompleted = false);

private:
  static Engine *_instance;

  MTL::Device *_device;
  MTL::Library *_defaultLibrary;
  MTL::CommandQueue *_commandQueue = nullptr;
  MTL::CommandBuffer *_activeCommandBuffer = nullptr;
  int _commandBufferDepth = 0;
  static bool ready;
  MTL::Library *compileShaderLibrary();
};
