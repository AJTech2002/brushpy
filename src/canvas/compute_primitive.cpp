#include "compute_primitive.h"
#include "Metal/MTLComputeCommandEncoder.hpp"

void ComputePrimitive::run(int width, int height, bool endEncoding) {
  compute.setup();

  MTL::ComputeCommandEncoder *encoder = compute.computeEncoder();
  bindUniforms(encoder);

  compute.dispatch(width, height, threadGroupWidth(), threadGroupHeight(),
                   endEncoding);
}

void ComputePrimitive::run(int width, int height) { run(width, height, true); }
