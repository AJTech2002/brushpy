#pragma once
#include "glm/ext/vector_float2.hpp"
#include "primitive.h"
#include "unordered_map"

namespace MTL {
class Texture;
};

class Canvas;

/*
  Layer owns its output texture (always canvas-sized) and composites
  all of its primitives into it each frame.
*/
class Layer {
public:
  Layer();
  ~Layer();
  void init(Canvas *canvas);
  // void draw(Canvas *canvas, glm::vec2 position, glm::vec2 region);
  void dispose();
  void draw(Primitive *primitive, glm::mat4x4 transformPx = glm::mat4x4(1.0f),
            glm::vec2 drawRegion = glm::vec2(0.0f, 0.0f));

  int width() const { return _width; }
  int height() const { return _height; }
  MTL::Texture *texture() const { return _texture; }

private:
  int _width = 0;
  int _height = 0;
  MTL::Texture *_texture = nullptr;
  MTL::Texture *_scratch = nullptr;
};