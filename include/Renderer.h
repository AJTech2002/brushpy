#pragma once
#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>
#include <QuartzCore/QuartzCore.h>

@interface Renderer : NSObject
- (instancetype)initWithDevice:(id<MTLDevice>)device
                         layer:(CAMetalLayer *)layer;
- (void)draw:(CADisplayLink *)displayLink;
@end
