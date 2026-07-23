#include "app.h"
#include "Renderer.h"
#include "engine.h"
#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>
#include <iostream>
#include <ostream>

void start_renderer(void *metalLayer, int width, int height, Canvas *canvas) {
  if (!Engine::isReady()) {
    std::cerr
        << "Error: Engine is not ready. Call start() before start_renderer()"
        << std::endl;
  }
  CA::MetalLayer *gLayer = (CA::MetalLayer *)metalLayer;
  gLayer->setDevice(Engine::device());
  gLayer->setPixelFormat(MTL::PixelFormatBGRA8Unorm);
  gLayer->setDrawableSize(CGSizeMake(width, height));

  Renderer::create(canvas, gLayer);
  Renderer::instance().init();
}

void stop(void) { Renderer::destroy(); }

void tick(void) { Renderer::instance().draw(); }
