#pragma once
#include "Metal/MTLTexture.hpp"
#include "glm/ext/vector_int2.hpp"
#include <objc/objc.h>
#include <string>

namespace MTL {
class ComputePipelineState;
class ComputeCommandEncoder;
} // namespace MTL

class Compute {

public:
  void init(const char *shaderName);
  void run();
  void setDispatchProperties(int w, int h, int tW, int tH) {
    this->width = w;
    this->height = h;
    this->threadGroupWidth = tW;
    this->threadGroupHeight = tH;
  }

protected:
  MTL::ComputePipelineState *_metalComputePSO;
  MTL::ComputeCommandEncoder *computeEncoder;
  int width;
  int height;
  int threadGroupWidth;
  int threadGroupHeight;

  virtual void setup();
  virtual void bind();
  virtual void dispatch(int width, int height, int threadGroupWidth,
                        int threadGroupHeight);
};

typedef struct {
  MTL::Texture *src;
  MTL::Texture *dst;
  glm::ivec2 start = glm::ivec2(0, 0);
  glm::ivec2 end = glm::ivec2(0, 0);
} CompositeComputeParams;

class CompositeCompute : public Compute {

public:
  CompositeCompute() : Compute() {}
  void setParams(const CompositeComputeParams params) { this->params = params; }
  void init() { Compute::init("compositeCompute"); }

protected:
  void bind() override;

private:
  CompositeComputeParams params = {};
};