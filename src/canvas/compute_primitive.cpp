#include "compute_primitive.h"
#include "Metal/MTLComputeCommandEncoder.hpp"
#include "glm/ext/vector_int2.hpp"

void ComputePrimitive::init() { compute.init(kernelName()); }

void ComputePrimitive::render(MTL::Texture *outputTexture, glm::mat4 transform,
                              glm::vec2 start, glm::vec2 end) {
  compute.setup();

  MTL::ComputeCommandEncoder *encoder = compute.computeEncoder();
  encoder->setTexture(outputTexture, 0);

  glm::ivec2 startInt = glm::ivec2(start.x, start.y);
  glm::ivec2 endInt = glm::ivec2(end.x, end.y);
  encoder->setBytes(&startInt, sizeof(glm::ivec2), 0);
  encoder->setBytes(&endInt, sizeof(glm::ivec2), 1);
  encoder->setBytes(&transform, sizeof(glm::mat4x4), 2);

  bindUniforms(encoder);

  compute.dispatch(end.x - start.x, end.y - start.y, threadGroupWidth(),
                   threadGroupHeight());
}
