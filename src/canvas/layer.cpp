#include "layer.h"
#include "Metal/MTLTexture.hpp"
#include "canvas.h"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float2.hpp"
#include "primitive.h"
#include "renderer.h"

Layer::Layer() {}

Layer::~Layer() {
  if (_texture) {
    _texture->release();
    _texture = nullptr;
  }
}

void Layer::init(Canvas *canvas) {
  _width = canvas->width();
  _height = canvas->height();
  _texture = Renderer::createTexture(_width, _height, "LayerTexture");
}

void Layer::add(Primitive *primitive, glm::mat4x4 transformPx,
                glm::vec2 sizePx) {
  glm::vec2 start = glm::vec2(0, 0);
  glm::vec2 end = glm::vec2(_width, _height);
  glm::mat4x4 inverseTransform = glm::inverse(transformPx);
  if (sizePx.x > 0 && sizePx.y > 0) {
    glm::vec2 center = inverseTransform * glm::vec4(0, 0, 0, 1);
    start = glm::vec2(center.x - sizePx.x / 2, center.y - sizePx.y / 2);
    end = glm::vec2(center.x + sizePx.x / 2, center.y + sizePx.y / 2);
  }

  primitive->render(_texture, inverseTransform, start, end);
}

void Layer::dispose() {}