#pragma once
#include "glm/ext/matrix_float4x4.hpp"

namespace MTL {
class Texture;
};
class Canvas;
class BTexture;

/*
  Primitive is an abstract base class for all drawable objects in the canvas. It
  defines the interface for width, height, and blend function. The blend
  function can be overridden by derived classes to specify how the primitive
  should be blended with the background.
*/
class Primitive {
public:
  virtual ~Primitive();
  Primitive();
  virtual int width() const { return _width; }
  virtual int height() const { return _height; }
  virtual void init(int width, int height) = 0;
  virtual std::string blendFn() const {
    return "return float4(CUR, COL, COL.a);";
  }
  virtual bool isDirty() const { return _isDirty; }
  BTexture *texture() const { return _texture; }
  virtual void render(MTL::Texture *inputTexture, MTL::Texture *outputTexture,
                      glm::vec2 position, glm::vec2 region) = 0;

protected:
  int _width = 0;
  int _height = 0;
  std::string _blendFn;
  BTexture *_texture;
  bool _isDirty = true;
};

struct PrimitiveInstance {
  glm::mat4x4 transform; // Use to calculate bounds
  bool isVisible;        // Use to determine if the primitive should be rendered
};