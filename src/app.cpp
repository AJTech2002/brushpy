#include "app.h"
#include "Renderer.h"
#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>
#include <cstdio>

static CA::MetalLayer *gLayer;

void start(void *metalLayer, int width, int height) {
  gLayer = (CA::MetalLayer *)metalLayer;

  MTL::Device *device = MTL::CreateSystemDefaultDevice();
  gLayer->setDevice(device);
  gLayer->setPixelFormat(MTL::PixelFormatBGRA8Unorm);
  gLayer->setDrawableSize(CGSizeMake(width, height));

  std::printf("Metal ready: %s (%dx%d)\n", device->name()->utf8String(),
              width, height);

  Renderer::create(device, gLayer);
  Renderer::instance().init();
}

void stop(void) { Renderer::destroy(); }

void tick(void) { Renderer::instance().draw(); }
