#include "layer.h"

Layer::Layer() {}
Layer::~Layer() {}

void Layer::init(Canvas *canvas) {}

void Layer::add(Primitive *primitive) {
  _primitives.push_back(primitive);
  _instances[primitive] = PrimitiveInstance{glm::mat4(1.0f), true};
}

void Layer::setVisible(Primitive *primitive, bool isVisible) {
  if (_instances.find(primitive) != _instances.end()) {
    _instances[primitive].isVisible = isVisible;
  }
}

void Layer::remove(Primitive *primitive) {
  auto it = std::find(_primitives.begin(), _primitives.end(), primitive);
  if (it != _primitives.end()) {
    _primitives.erase(it);
    _instances.erase(primitive);
  }
}

void Layer::draw(Canvas *canvas, glm::vec2 position, glm::vec2 region) {
  for (Primitive *primitive : _primitives) {
    if (primitive == nullptr) {
      remove(primitive);
      continue;
    }

    PrimitiveInstance &instance = _instances[primitive];
    if (instance.isVisible) {
      if (primitive->isDirty()) {
        primitive->render(nullptr, nullptr, position, region);
      }

      // TODO: Composite the primitive's texture onto the layer's texture using
      // the instance's transform
    }
  }
}

void Layer::dispose() {
  for (Primitive *primitive : _primitives) {
    delete primitive;
  }
  _primitives.clear();
  _instances.clear();
}