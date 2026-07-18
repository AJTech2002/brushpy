#include "canvas.h"
#include "Renderer.h"
#include "compute.h"
#include "layer.h"
#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>

CompositeCompute compositor = CompositeCompute();

Canvas::Canvas(int width, int height) : _width(width), _height(height) {}

Canvas::~Canvas() {}

void Canvas::init(const Renderer *renderer) {
  compositor.init();
  for (Layer *layer : _layers) {
    layer->init(this);
  }
}

void Canvas::draw(const Renderer *renderer) {
  for (Layer *layer : _layers) {
    layer->draw(this, glm::vec2(0.0f, 0.0f), glm::vec2(_width, _height));
  }

  compositor.setParams({
      .src = renderer->outputTexture(),
      .dst = renderer->outputTexture(),
      .start = glm::ivec2(0, 0),
      .end = glm::ivec2(_width, _height / 2),
  });

  compositor.run();
}

void Canvas::dispose() {
  for (Layer *layer : _layers) {
    layer->dispose();
  }
}
