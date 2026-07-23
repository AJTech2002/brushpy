#include "canvas.h"
#include "compute.h"
#include "engine.h"
#include "layer.h"
#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <iostream>

CompositeCompute compositor = CompositeCompute();

Canvas::Canvas(int width, int height) {
  _width = width;
  _height = height;
  init();
}

Canvas::~Canvas() {
  if (_outputTexture) {
    _outputTexture->release();
    _outputTexture = nullptr;
  }
}

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

void Canvas::add(Layer *layer) {
  _layers.push_back(layer);
  layer->init(this);
}

void Canvas::draw() {
  // Loop through Layers backwards and composite them onto the output texture
  for (int i = _layers.size() - 1; i >= 0; i--) {
    Layer *layer = _layers[i];

    // TODO: Optimize this by only running the compositor if the layer is dirty
    // and within dirty regions
    compositor.run({
        .src = layer->texture(),
        .dst = _outputTexture,
        .start = glm::ivec2(0, 0),
        .end = glm::ivec2(width(), height()),
    });
  }
}

void Canvas::dispose() {
  for (Layer *layer : _layers) {
    layer->dispose();
  }
}
