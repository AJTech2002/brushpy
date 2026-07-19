#pragma once
#include "compute.h"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float4.hpp"
#include "primitive.h"

class Square : public Primitive {
public:
  Square(glm::vec2 size, glm::vec4 color) : Primitive() {
    this->size = size;
    this->color = color;
    init();
  }

  void init() override;
  void render(MTL::Texture *outputTexture, glm::mat4 transform, glm::vec2 start,
              glm::vec2 end) override;

  glm::vec2 size = glm::vec2(50, 50);
  glm::vec4 color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

protected:
  Compute squareCompute = Compute();
};