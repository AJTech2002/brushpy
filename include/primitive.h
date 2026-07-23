#pragma once
#include "engine.h"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float2.hpp"
#include <iostream>

namespace MTL {
class Texture;
};

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

  virtual std::string blendFn() const {
    return "return float4(CUR, COL, COL.a);";
  }
  virtual void init();
  virtual void render(MTL::Texture *outputTexture, glm::mat4 transform,
                      glm::vec2 start, glm::vec2 end) = 0;
};
