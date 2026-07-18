#pragma once
#include "glm/ext/vector_float2.hpp"
#include "primitive.h"
#include "unordered_map"

namespace MTL {
class Texture;
};

class Canvas;

/*
  Layer is a simple primitive that represents a layer in the canvas. It can be
  used to group other primitives together and apply transformations to them.
  Limitation is that the texture is always the same size as the canvas.
*/
class Layer : public Primitive {
public:
  Layer();
  ~Layer();
  void init(Canvas *canvas);
  void draw(Canvas *canvas, glm::vec2 position, glm::vec2 region);
  void dispose();
  void add(Primitive *primitive);
  void remove(Primitive *primitive);
  void setVisible(Primitive *primitive, bool isVisible);

private:
  std::unordered_map<Primitive *, PrimitiveInstance> _instances;
  std::vector<Primitive *> _primitives;
};