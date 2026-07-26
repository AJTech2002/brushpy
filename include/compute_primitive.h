#pragma once
#include "Metal/MTLComputeCommandEncoder.hpp"
#include "compute.h"
#include "primitive.h"

class ComputePrimitive : public Primitive {
public:
  void init() override {
    //
    compute.init(kernelName());
  }
  void init(MTL::ComputeCommandEncoder *encoder) {
    compute.init(kernelName(), encoder);
  }
  void run(int width, int height) override;
  void run(int width, int height, bool endEncoding) override;
  void setEncoder(MTL::ComputeCommandEncoder *encoder) override {
    compute.setEncoder(encoder);
  }

protected:
  virtual const char *kernelName() const = 0;
  virtual void bindUniforms(MTL::ComputeCommandEncoder *encoder) = 0;
  virtual int threadGroupWidth() const { return 16; }
  virtual int threadGroupHeight() const { return 16; }

  Compute compute = Compute();
};
