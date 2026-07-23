#define NS_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#include "engine.h"
#include "stb_image.h"
#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

static inline const std::vector<std::string> shaderFiles = {
    "base.metal",
};

Engine *Engine::_instance = nullptr;
bool Engine::ready = false;

Engine::Engine() {
  _instance = this;
  _device = MTL::CreateSystemDefaultDevice();
}

void Engine::init() {
  _commandQueue = _device->newCommandQueue();
  _defaultLibrary = compileShaderLibrary();
  ready = true;
  std::cout << "BrushPY Engine ready, created Command Queue & Shader Library"
            << std::endl;
}

/*
  Begins (or joins) the current rasterization pass's command buffer. Nested
  callers share the same buffer so an entire pass - primitive rasterization,
  layer compositing, and (when presenting) the on-screen blit - can be
  recorded together, while a standalone caller with no outer pass still gets a
  correctly scoped buffer of its own.
*/
MTL::CommandBuffer *Engine::beginCommandBuffer(std::string label) {
  Engine &self = instance();

  // Supports nested calls to beginCommandBuffer() and endCommandBuffer()
  if (self._activeCommandBuffer == nullptr) {
    self._activeCommandBuffer = self._commandQueue->commandBuffer();
    self._activeCommandBuffer->setLabel(
        NS::String::string(label.c_str(), NS::ASCIIStringEncoding));
  }
  self._commandBufferDepth++;
  return self._activeCommandBuffer;
}

void Engine::endCommandBuffer(bool waitUntilCompleted) {
  Engine &self = instance();
  if (self._commandBufferDepth == 0) {
    std::cerr << "Error: Engine::endCommandBuffer() called without a "
                 "matching beginCommandBuffer()"
              << std::endl;
    return;
  }

  self._commandBufferDepth--;
  if (self._commandBufferDepth > 0)
    return;

  MTL::CommandBuffer *buffer = self._activeCommandBuffer;
  self._activeCommandBuffer = nullptr;
  buffer->commit();
  if (waitUntilCompleted)
    buffer->waitUntilCompleted();
}

/*
  Engine is a singleton that manages the Metal device, command queue, and shader
  library. It provides utility functions for creating textures and loading
  images.
*/
MTL::Library *Engine::compileShaderLibrary() {
  std::filesystem::path shaderDir =
      std::filesystem::path(__FILE__).parent_path().parent_path() / "shaders";

  std::string source;
  for (const auto &file : shaderFiles) {
    std::filesystem::path path = shaderDir / file;
    std::ifstream in(path);
    if (!in) {
      std::cerr << "Renderer: failed to open shader file " << path << std::endl;
      continue;
    }
    std::stringstream contents;
    contents << in.rdbuf();
    source += contents.str();
    source += "\n";
  }

  NS::Error *error = nullptr;
  MTL::Library *library = Engine::device()->newLibrary(
      NS::String::string(source.c_str(), NS::UTF8StringEncoding), nullptr,
      &error);

  if (!library) {
    std::cerr << "Renderer: failed to compile shader library: "
              << (error ? error->localizedDescription()->utf8String()
                        : "unknown error")
              << std::endl;
  }

  return library;
}

/*
  Creates a Metal texture with the specified width, height, and label.
  The texture is created with RGBA32Float pixel format and shared storage mode.
*/
MTL::Texture *Engine::createTexture(int width, int height, std::string label) {
  MTL::TextureDescriptor *desc = MTL::TextureDescriptor::alloc()->init();
  desc->setTextureType(MTL::TextureType2D);
  desc->setPixelFormat(MTL::PixelFormatRGBA32Float);
  desc->setWidth(width);
  desc->setHeight(height);
  desc->setStorageMode(MTL::StorageModeShared);
  desc->setUsage(MTL::TextureUsageRenderTarget | MTL::TextureUsageShaderRead |
                 MTL::TextureUsageShaderWrite);

  MTL::Texture *texture = instance()._device->newTexture(desc);
  texture->setLabel(NS::String::string(label.c_str(), NS::ASCIIStringEncoding));

  desc->release();
  return texture;
}

/*
  Loads an image from the specified path and creates a Metal texture from it.
  The image is loaded using stb_image and converted to RGBA8 format.
*/
MTL::Texture *Engine::loadTexture(const char *imagePath, std::string label) {
  int width, height, channels;
  stbi_uc *pixels = stbi_load(imagePath, &width, &height, &channels,
                              STBI_rgb_alpha); // force RGBA8
  if (!pixels)
    return nullptr;

  MTL::TextureDescriptor *desc = MTL::TextureDescriptor::alloc()->init();
  desc->setWidth(width);
  desc->setHeight(height);
  desc->setPixelFormat(MTL::PixelFormatRGBA8Unorm);
  desc->setTextureType(MTL::TextureType2D);
  desc->setStorageMode(MTL::StorageModeShared); // or Managed on macOS
  desc->setUsage(MTL::TextureUsageShaderRead);

  MTL::Texture *texture = instance()._device->newTexture(desc);
  desc->release();

  MTL::Region region = MTL::Region(0, 0, 0, width, height, 1);
  NS::UInteger bytesPerRow = width * 4;
  texture->replaceRegion(region, 0, pixels, bytesPerRow);

  stbi_image_free(pixels);
  return texture;
}