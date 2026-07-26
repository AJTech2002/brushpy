#pragma once
#include "Metal/MTLTexture.hpp"
#include "glm/ext/vector_int2.hpp"

namespace MTL {
class ComputePipelineState;
class ComputeCommandEncoder;
} // namespace MTL

class Compute {

public:
  virtual ~Compute() = default;

  void init(const char *shaderName);
  void init(const char *shaderName, MTL::ComputeCommandEncoder *encoder);
  virtual void setup();
  virtual void bind();
  virtual void dispatch(int width, int height, int threadGroupWidth,
                        int threadGroupHeight, bool endEncoding);
  const MTL::ComputePipelineState *metalComputePSO() const {
    return _metalComputePSO;
  }
  MTL::ComputeCommandEncoder *computeEncoder() const { return _computeEncoder; }
  void setEncoder(MTL::ComputeCommandEncoder *encoder) { _computeEncoder = encoder; }

protected:
  MTL::ComputePipelineState *_metalComputePSO = nullptr;
  MTL::ComputeCommandEncoder *_computeEncoder = nullptr;
};

struct CompositeComputeParams {
  MTL::Texture *src;
  MTL::Texture *dst;
  glm::ivec2 start = glm::ivec2(0, 0);
  glm::ivec2 end = glm::ivec2(0, 0);
};

class CompositeCompute : public Compute {

public:
  CompositeCompute() : Compute() {}
  void init() { Compute::init("compositeCompute"); }
  void run(CompositeComputeParams params, bool endEncoding) {
    this->params = params;
    setup();
    bind();
    dispatch(params.end.x - params.start.x, params.end.y - params.start.y, 8, 8,
             endEncoding);
  }

protected:
  void bind() override;

private:
  CompositeComputeParams params = {};
};

class ClearCompute : public Compute {

public:
  ClearCompute() : Compute() {}
  void init() { Compute::init("clearCompute"); }
  void run(MTL::Texture *target, int width, int height, bool endEncoding) {
    this->target = target;
    setup();
    bind();
    dispatch(width, height, 8, 8, endEncoding);
  }

protected:
  void bind() override;

private:
  MTL::Texture *target = nullptr;
};
