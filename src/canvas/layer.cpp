#include "layer.h"
#include "Metal/MTLComputeCommandEncoder.hpp"
#include "Metal/MTLTexture.hpp"
#include "canvas.h"
#include "compute.h"
#include "engine.h"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float2.hpp"
#include "primitive.h"

// Shared
CompositeCompute layerCompositor = CompositeCompute();
ClearCompute layerScratchClear = ClearCompute();

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
  _scratch = Engine::createTexture(_width, _height, "LayerScratchTexture");
  layerCompositor.init();
  layerScratchClear.init();
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
                 glm::vec2 drawRegion) {
  glm::vec2 start = glm::vec2(0, 0);
  glm::vec2 end = glm::vec2(_width, _height);
  glm::mat4x4 inverseTransform = glm::inverse(transformPx);

  if (drawRegion.x > 0 && drawRegion.y > 0) {
    glm::vec2 center = inverseTransform * glm::vec4(0, 0, 0, 1);
    start = glm::vec2(center.x - drawRegion.x / 2, center.y - drawRegion.y / 2);
    end = glm::vec2(center.x + drawRegion.x / 2, center.y + drawRegion.y / 2);
  }

  Engine::beginCommandBuffer("Layer::draw");
  MTL::ComputeCommandEncoder *encoder =
      Engine::activeCommandBuffer()->computeCommandEncoder();

  layerScratchClear.setEncoder(encoder);
  layerScratchClear.run(_scratch, _width, _height, false);

  encoder->setTexture(_scratch, 0);
  encoder->setTexture(_texture, 1); // Input Texture

  glm::ivec2 startInt = glm::ivec2(start.x, start.y);
  glm::ivec2 endInt = glm::ivec2(end.x, end.y);
  encoder->setBytes(&startInt, sizeof(glm::ivec2), 0);
  encoder->setBytes(&endInt, sizeof(glm::ivec2), 1);
  encoder->setBytes(&inverseTransform, sizeof(glm::mat4x4), 2);

  primitive->setEncoder(encoder);
  primitive->run(_width, _height, false);

  layerCompositor.setEncoder(encoder);
  layerCompositor.run(
      {.src = _scratch, .dst = _texture, .start = startInt, .end = endInt},
      true);

  Engine::endCommandBuffer(false);
}

void Layer::dispose() {}