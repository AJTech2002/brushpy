#include "primitive_shape.h"
#include "Metal/MTLComputeCommandEncoder.hpp"
#include "Metal/Metal.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float4.hpp"
#include "glm/ext/vector_int2.hpp"

void Square::init() {
  printf("Initializing Square Primitive\n");
  this->squareCompute.init("squareCompute");
}

void Square::render(MTL::Texture *outputTexture, glm::mat4 transform,
                    glm::vec2 start, glm::vec2 end) {

  this->squareCompute.setup();

  MTL::ComputeCommandEncoder *encoder = this->squareCompute.computeEncoder();
  encoder->setTexture(outputTexture, 0);

  glm::ivec2 startInt = glm::ivec2(start.x, start.y);
  encoder->setBytes(&startInt, sizeof(glm::ivec2), 0);

  glm::ivec2 endInt = glm::ivec2(end.x, end.y);
  encoder->setBytes(&endInt, sizeof(glm::ivec2), 1);

  // Square Uniforms

  encoder->setBytes(&size, sizeof(glm::vec2), 2);

  encoder->setBytes(&color, sizeof(glm::vec4), 3);

  encoder->setBytes(&transform, sizeof(glm::mat4x4), 4);

  printf("Dispatching Square Compute Shader: start(%d, %d), end(%d, %d)\n",
         startInt.x, startInt.y, endInt.x, endInt.y);

  this->squareCompute.dispatch(end.x - start.x, end.y - start.y, 16, 16);
}