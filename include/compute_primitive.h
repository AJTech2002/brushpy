#pragma once
#include "compute.h"
#include "primitive.h"

class ComputePrimitive : public Primitive {
public:
  void init() override;
  void render(MTL::Texture *outputTexture, glm::mat4 transform, glm::vec2 start,
              glm::vec2 end) override;

protected:
  virtual const char *kernelName() const = 0;
  virtual void bindUniforms(MTL::ComputeCommandEncoder *encoder) = 0;
  virtual int threadGroupWidth() const { return 16; }
  virtual int threadGroupHeight() const { return 16; }

  Compute compute = Compute();
};
