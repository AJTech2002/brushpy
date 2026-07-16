#include "canvas.h"
#include "layer.h"

Canvas::Canvas() {}

Canvas::~Canvas() {}

void Canvas::init(const Renderer *renderer) {
  for (Layer *layer : _layers) {
    layer->init(this);
  }
}

void Canvas::draw(const Renderer *renderer) {
  for (Layer *layer : _layers) {
    layer->draw(this, glm::vec2(0.0f, 0.0f), glm::vec2(width(), height()));
  }

  //TODO: Composite layers into a single output texture
}

void Canvas::dispose() {
  for (Layer *layer : _layers) {
    layer->dispose();
  }
}
