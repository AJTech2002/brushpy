
#include "Renderer.h"
#include <Metal/Metal.h>
#include <cstdio>
#include <iostream>

@implementation Renderer {
  id<MTLDevice> _device;
  CAMetalLayer *_layer;
  id<MTLCommandQueue> _commandQueue;
}

- (instancetype)initWithDevice:(id<MTLDevice>)device
                         layer:(CAMetalLayer *)layer {
  self = [super init];
  if (self) {
    _device = device;
    _layer = layer;
    _commandQueue = [_device newCommandQueue];
  }

  return self;
}

- (void)draw:(CADisplayLink *)displayLink {

  id<CAMetalDrawable> drawable = [_layer nextDrawable];
  if (!drawable)
    return;

  id<MTLCommandBuffer> command_buffer = [_commandQueue commandBuffer];

  MTLRenderPassDescriptor *descriptor =
      [MTLRenderPassDescriptor renderPassDescriptor];

  descriptor.colorAttachments[0].texture = drawable.texture;
  descriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
  descriptor.colorAttachments[0].clearColor =
      MTLClearColorMake(1.0, 0.0, 0.0, 1.0);
  descriptor.colorAttachments[0].storeAction = MTLStoreActionStore;

  id<MTLRenderCommandEncoder> encoder =
      [command_buffer renderCommandEncoderWithDescriptor:descriptor];

    

  [encoder endEncoding];
  [command_buffer presentDrawable:drawable];
  [command_buffer commit];
}

@end