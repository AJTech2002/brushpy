#include "canvas.h"
#include "Renderer.h"
#include "layer.h"
#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>

Canvas::Canvas(int width, int height) : _width(width), _height(height) {}

Canvas::~Canvas() {}

void Canvas::init(const Renderer *renderer) {

  MTL::Function *computeShader = Renderer::getDefaultLibrary()->newFunction(
      NS::String::string("computeShader", NS::ASCIIStringEncoding));

  MTL::ComputePipelineDescriptor *computePipelineDescriptor =
      MTL::ComputePipelineDescriptor::alloc()->init();
  computePipelineDescriptor->setComputeFunction(computeShader);

  NS::Error *error = nullptr;

  metalComputePSO =
      Renderer::getDevice()->newComputePipelineState(computeShader, &error);
  computeShader->release();

  for (Layer *layer : _layers) {
    layer->init(this);
  }
}

void Canvas::draw(const Renderer *renderer) {
  for (Layer *layer : _layers) {
    layer->draw(this, glm::vec2(0.0f, 0.0f), glm::vec2(_width, _height));
  }

  // TODO: Make general compositor

  MTL::ComputeCommandEncoder *computeEncoder =
      Renderer::activeCommandBuffer()->computeCommandEncoder();
  computeEncoder->setComputePipelineState(metalComputePSO);

  computeEncoder->setTexture(Renderer::getOutputTexture(), 0);

  MTL::Size gridSize = MTL::Size(width(), height(), 1);
  MTL::Size threadGroupSize = MTL::Size(16, 16, 1);
  computeEncoder->dispatchThreads(gridSize, threadGroupSize);
  computeEncoder->endEncoding();
}

void Canvas::dispose() {
  for (Layer *layer : _layers) {
    layer->dispose();
  }
}
