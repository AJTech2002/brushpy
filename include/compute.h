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
  void setDispatchProperties(int w, int h, int tW, int tH) {
    this->width = w;
    this->height = h;
    this->threadGroupWidth = tW;
    this->threadGroupHeight = tH;
  }
  virtual void setup();
  virtual void bind();
  virtual void dispatch(int width, int height, int threadGroupWidth,
                        int threadGroupHeight);
  const MTL::ComputePipelineState *metalComputePSO() const {
    return _metalComputePSO;
  }
  MTL::ComputeCommandEncoder *computeEncoder() const { return _computeEncoder; }

protected:
  MTL::ComputePipelineState *_metalComputePSO;
  MTL::ComputeCommandEncoder *_computeEncoder;
  int width;
  int height;
  int threadGroupWidth;
  int threadGroupHeight;
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
  void init() { Compute::init("compositeCompute"); }
  void run(CompositeComputeParams params) {
    this->params = params;
    setup();
    bind();
    dispatch(params.end.x - params.start.x, params.end.y - params.start.y, 8,
             8);
  }

protected:
  void bind() override;

private:
  CompositeComputeParams params = {};
};
