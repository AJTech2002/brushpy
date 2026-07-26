#include "compute.h"
#include "engine.h"
#include "stb_image.h"
#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>
#include <iostream>
#include <ostream>
#include <simd/simd.h>
#include <string>
#include <unordered_map>

// Process-wide cache of compiled pipeline states, keyed by shader name.
// Owns every entry for the lifetime of the process; Compute instances only
// ever hold a non-owning pointer into this map.
static std::unordered_map<std::string, MTL::ComputePipelineState *>
    pipelineStateCache;

void Compute::init(const char *shaderName,
                   MTL::ComputeCommandEncoder *encoder) {
  init(shaderName);
  _computeEncoder = encoder;
}

void Compute::init(const char *shaderName) {
  auto cached = pipelineStateCache.find(shaderName);
  if (cached != pipelineStateCache.end()) {
    _metalComputePSO = cached->second;
    return;
  }

  MTL::Function *computeShader = Engine::defaultLibrary()->newFunction(
      NS::String::string(shaderName, NS::ASCIIStringEncoding));

  MTL::ComputePipelineDescriptor *computePipelineDescriptor =
      MTL::ComputePipelineDescriptor::alloc()->init();
  computePipelineDescriptor->setComputeFunction(computeShader);

  NS::Error *error = nullptr;

  _metalComputePSO =
      Engine::device()->newComputePipelineState(computeShader, &error);
  computeShader->release();
  computePipelineDescriptor->release();

  pipelineStateCache[shaderName] = _metalComputePSO;
}

void Compute::setup() {
  if (Engine::activeCommandBuffer() == nullptr) {
    std::cerr << "Error: no active command buffer. Call "
                 "Engine::beginCommandBuffer() before using Compute"
              << std::endl;
    return;
  }

  // _computeEncoder = Engine::activeCommandBuffer()->computeCommandEncoder();
  if (_computeEncoder == nullptr) {
    _computeEncoder = Engine::activeCommandBuffer()->computeCommandEncoder();
  }
  _computeEncoder->setComputePipelineState(_metalComputePSO);
}

void Compute::bind() { return; }

void Compute::dispatch(int width, int height, int threadGroupWidth,
                       int threadGroupHeight, bool endEncoding) {
  MTL::Size gridSize = MTL::Size(width, height, 1);
  MTL::Size threadGroupSize = MTL::Size(threadGroupWidth, threadGroupHeight, 1);
  _computeEncoder->dispatchThreads(gridSize, threadGroupSize);
  if (endEncoding) {
    _computeEncoder->endEncoding();
  }
  _computeEncoder = nullptr;
}

// - Compositor Implementation -
// TODO: Move this to a new file
void CompositeCompute::bind() {

  if (_computeEncoder == nullptr) {
    std::cerr << "Error: Compute encoder is null. Call setup() before bind()"
              << std::endl;
    return;
  }

  _computeEncoder->setTexture(params.dst, 0);
  _computeEncoder->setTexture(params.src, 1);

  glm::ivec2 start = params.start;
  glm::ivec2 end = params.end;

  _computeEncoder->setBytes(&start, sizeof(glm::ivec2), 0);
  _computeEncoder->setBytes(&end, sizeof(glm::ivec2), 1);
}

// - Clear Implementation -
void ClearCompute::bind() {
  if (_computeEncoder == nullptr) {
    std::cerr << "Error: Compute encoder is null. Call setup() before bind()"
              << std::endl;
    return;
  }

  _computeEncoder->setTexture(target, 0);
}
