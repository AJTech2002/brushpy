#include "canvas.h"
#include "layer.h"

Canvas::Canvas(int width, int height) : _width(width), _height(height) {}

Canvas::~Canvas() {}

void Canvas::init(const Renderer *renderer) {
  for (Layer *layer : _layers) {
    layer->init(this);
  }
}

void Canvas::draw(const Renderer *renderer) {
  for (Layer *layer : _layers) {
    layer->draw(this, glm::vec2(0.0f, 0.0f), glm::vec2(_width, _height));
  }

  //TODO: Composite layers into a single output texture
}

void Canvas::dispose() {
  for (Layer *layer : _layers) {
    layer->dispose();
  }
}
