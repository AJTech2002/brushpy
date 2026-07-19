#include "compute.h"
#include "Metal/MTLComputeCommandEncoder.hpp"
#include "Metal/MTLTexture.hpp"
#include "Metal/Metal.hpp"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_int2.hpp"
#include "renderer.h"
#include <cassert>

void Compute::init(const char *shaderName) {
  MTL::Function *computeShader = Renderer::defaultLibrary()->newFunction(
      NS::String::string(shaderName, NS::ASCIIStringEncoding));

  MTL::ComputePipelineDescriptor *computePipelineDescriptor =
      MTL::ComputePipelineDescriptor::alloc()->init();
  computePipelineDescriptor->setComputeFunction(computeShader);

  NS::Error *error = nullptr;

  _metalComputePSO =
      Renderer::device()->newComputePipelineState(computeShader, &error);
  computeShader->release();
}

void Compute::setup() {
  if (Renderer::activeCommandBuffer() == nullptr) {
    assert("Active command buffer is null. Call Renderer::beginFrame() before "
           "setup()");
    return;
  }
  _computeEncoder = Renderer::activeCommandBuffer()->computeCommandEncoder();
  _computeEncoder->setComputePipelineState(_metalComputePSO);
}

void Compute::bind() { return; }

void Compute::dispatch(int width, int height, int threadGroupWidth,
                       int threadGroupHeight) {
  MTL::Size gridSize = MTL::Size(width, height, 1);
  MTL::Size threadGroupSize = MTL::Size(threadGroupWidth, threadGroupHeight, 1);
  _computeEncoder->dispatchThreads(gridSize, threadGroupSize);
  _computeEncoder->endEncoding();
}

// - Compositor Implementation -
// TODO: Move this to a new file
void CompositeCompute::bind() {

  if (_computeEncoder == nullptr) {
    assert("Compute encoder is null. Call setup() before bind()");
    return;
  }

  _computeEncoder->setTexture(params.dst, 0);
  _computeEncoder->setTexture(params.src, 1);

  glm::ivec2 start = params.start;
  glm::ivec2 end = params.end;

  _computeEncoder->setBytes(&start, sizeof(glm::ivec2), 0);
  _computeEncoder->setBytes(&end, sizeof(glm::ivec2), 1);

  // TODO: Make thread group size configurable
  setDispatchProperties(end.x - start.x, end.y - start.y, 16, 16);
}
