#include "canvas.h"
#include "compute.h"
#include "engine.h"
#include "layer.h"
#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <vector>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

CompositeCompute compositor = CompositeCompute();

/*
  Reads back an RGBA32Float texture, tone-maps it down to RGBA8, and writes
  it out as a PNG. Writes to a temp file first and renames it into place so
  a directory watcher (e.g. viewer.py) never observes a partially-written
  file - rename is atomic within the same filesystem.
*/
static void writeTextureToPNG(MTL::Texture *texture, int width, int height,
                              const std::string &path) {
  std::vector<float> floatPixels(static_cast<size_t>(width) * height * 4);
  texture->getBytes(floatPixels.data(), width * 4 * sizeof(float),
                    MTL::Region(0, 0, 0, width, height, 1), 0);

  std::vector<uint8_t> pixels(floatPixels.size());
  for (size_t i = 0; i < pixels.size(); i++) {
    float v = std::clamp(floatPixels[i], 0.0f, 1.0f);
    pixels[i] = static_cast<uint8_t>(v * 255.0f + 0.5f);
  }

  std::filesystem::path outPath(path);
  std::error_code ec;
  if (outPath.has_parent_path())
    std::filesystem::create_directories(outPath.parent_path(), ec);

  std::filesystem::path tmpPath = outPath;
  tmpPath += ".tmp";
  if (!stbi_write_png(tmpPath.string().c_str(), width, height, 4, pixels.data(),
                      width * 4)) {
    std::cerr << "Canvas::renderOut: failed to write " << tmpPath << std::endl;
    return;
  }

  std::filesystem::rename(tmpPath, outPath, ec);
  if (ec) {
    std::cerr << "Canvas::renderOut: failed to move " << tmpPath << " to "
              << outPath << ": " << ec.message() << std::endl;
  }
}

Canvas::Canvas(int width, int height) {
  _width = width;
  _height = height;
  init();
}

Canvas::~Canvas() { dispose(); }

void Canvas::init() {
  compositor.init();
  _outputTexture =
      Engine::createTexture(_width, _height, "CanvasOutputTexture");
  for (Layer *layer : _layers) {
    layer->init(this);
  }
}

void Canvas::render() {
  // std::cout << "Canvas::render() called, drawing canvas" << std::endl;
  Engine::beginCommandBuffer("Canvas::render");
  this->draw();
  Engine::endCommandBuffer(true);
};

void Canvas::renderOut(const std::string &path) {
  Engine::beginCommandBuffer("Canvas::renderOut");
  this->draw();
  MTL::CommandBuffer *buffer = Engine::activeCommandBuffer();

  // Retained so the texture stays alive for the completion handler even if
  // the Canvas is torn down before the GPU finishes.
  MTL::Texture *texture = _outputTexture;
  texture->retain();
  int width = _width;
  int height = _height;

  buffer->addCompletedHandler(
      [texture, width, height, path](MTL::CommandBuffer *) {
        writeTextureToPNG(texture, width, height, path);
        texture->release();
      });

  Engine::endCommandBuffer(false);
}

void Canvas::add(Layer *layer) {
  _layers.push_back(layer);
  layer->init(this);
}

void Canvas::draw() {
  Engine::beginCommandBuffer("Layer::draw");

  // Loop through Layers backwards and composite them onto the output texture
  for (int i = _layers.size() - 1; i >= 0; i--) {
    Layer *layer = _layers[i];

    // TODO: Optimize this by only running the compositor if the layer is dirty
    // and within dirty regions
    compositor.run(
        {
            .src = layer->texture(),
            .dst = _outputTexture,
            .start = glm::ivec2(0, 0),
            .end = glm::ivec2(width(), height()),
        },
        true);
  }

  Engine::endCommandBuffer(false);
}

void Canvas::dispose() {
  for (Layer *layer : _layers) {
    layer->dispose();
    delete layer;
  }
  _layers.clear();

  if (_outputTexture) {
    _outputTexture->release();
    _outputTexture = nullptr;
  }
}
