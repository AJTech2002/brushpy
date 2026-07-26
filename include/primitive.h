#pragma once
#include "engine.h"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float2.hpp"
#include <iostream>

namespace MTL {
class Texture;
class ComputeCommandEncoder;
}; // namespace MTL

/*
  Primitive is an abstract base class for all drawable objects in the canvas.
  It represents a renderable that can paint itself into a given region of an
  output texture. Size and texture storage belong to the Layer.
*/
class Primitive {
public:
  virtual ~Primitive();

  Primitive() {
    if (Engine::isReady() == false) {
      std::cerr << "Error: Renderer is not ready. Call Renderer::init() before "
                   "creating "
                   "Primitives"
                << std::endl;
    }

    init();
  }

  virtual void init();
  virtual void run(int width, int height);
  virtual void run(int width, int height, bool endEncoding);
  virtual void setEncoder(MTL::ComputeCommandEncoder *encoder) {}
};
