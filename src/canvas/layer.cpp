#include "layer.h"
#include "Metal/MTLTexture.hpp"
#include "canvas.h"
#include "engine.h"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float2.hpp"
#include "primitive.h"

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
  _texture = Engine::createTexture(_width, _height, "LayerTexture");

  // fill with red
  // for (int y = 0; y < _height; y++) {
  //   for (int x = 0; x < _width; x++) {
  //     float pixel[4] = {1.0f, 0.0f, 0.0f, 1.0f};
  //     _texture->replaceRegion(MTL::Region(x, y, 1, 1), 0, pixel,
  //                             sizeof(float) * 4);
  //   }
  // }
}

void Layer::draw(Primitive *primitive, glm::mat4x4 transformPx,
                 glm::vec2 sizePx) {
  glm::vec2 start = glm::vec2(0, 0);
  glm::vec2 end = glm::vec2(_width, _height);
  glm::mat4x4 inverseTransform = glm::inverse(transformPx);
  if (sizePx.x > 0 && sizePx.y > 0) {
    glm::vec2 center = inverseTransform * glm::vec4(0, 0, 0, 1);
    start = glm::vec2(center.x - sizePx.x / 2, center.y - sizePx.y / 2);
    end = glm::vec2(center.x + sizePx.x / 2, center.y + sizePx.y / 2);
  }

  Engine::beginCommandBuffer("Layer::draw");
  primitive->render(_texture, inverseTransform, start, end);
  Engine::endCommandBuffer(true);
}

void Layer::dispose() {}