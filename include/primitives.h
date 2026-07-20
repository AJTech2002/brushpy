#pragma once
#include "Metal/MTLComputeCommandEncoder.hpp"
#include "compute_primitive.h"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float4.hpp"

class Square : public ComputePrimitive {
public:
  Square(glm::vec2 size, glm::vec4 color) : size(size), color(color) { init(); }

  glm::vec2 size = glm::vec2(50, 50);
  glm::vec4 color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

protected:
  const char *kernelName() const override { return "squareCompute"; }
  void bindUniforms(MTL::ComputeCommandEncoder *encoder) override {
    encoder->setBytes(&size, sizeof(glm::vec2), 3);
    encoder->setBytes(&color, sizeof(glm::vec4), 4);
  }
};

class Circle : public ComputePrimitive {
public:
  Circle(glm::vec2 size, glm::vec4 color) : size(size), color(color) { init(); }
  glm::vec2 size = glm::vec2(50, 50);
  glm::vec4 color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

protected:
  const char *kernelName() const override { return "circleCompute"; }
  void bindUniforms(MTL::ComputeCommandEncoder *encoder) override {
    encoder->setBytes(&size, sizeof(glm::vec2), 3);
    encoder->setBytes(&color, sizeof(glm::vec4), 4);
  };
};

class Image : public ComputePrimitive {

public:
  Image(const char *imagePath, glm::vec2 size = glm::vec2(0, 0)) {
    init();

    texture = Renderer::loadTexture(imagePath);
    if (size.x == 0 && size.y == 0) {
      this->size = glm::vec2(texture->width(), texture->height());
    } else {
      this->size = size;
    }
  }

private:
  MTL::Texture *texture = nullptr;
  glm::vec2 size = glm::vec2(0, 0);
  const char *kernelName() const override { return "imageCompute"; }
  void bindUniforms(MTL::ComputeCommandEncoder *encoder) override {
    encoder->setTexture(texture, 1);
    encoder->setBytes(&size, sizeof(glm::vec2), 3);
  }
};