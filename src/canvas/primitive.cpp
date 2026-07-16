#include "primitive.h"

Primitive::Primitive() {}

Primitive::~Primitive() {
  if (_texture) {
    delete _texture;
    _texture = nullptr;
  }
}

void Primitive::init(int width, int height) {
  _width = width;
  _height = height;
  _isDirty = true;
}

void Primitive::render(MTL::Texture *inputTexture, MTL::Texture *outputTexture,
                       glm::vec2 position, glm::vec2 region) {
  // TODO: Connect this to Renderer to queue for rendering
}