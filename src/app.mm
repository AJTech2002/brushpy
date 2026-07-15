#import <Cocoa/Cocoa.h>
#include <Foundation/Foundation.h>
#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>
#import <QuartzCore/CADisplayLink.h>
#import "renderer.h"
#import <AppKit/NSScreen.h>

static id<MTLDevice> gDevice;
static id<MTLCommandQueue> gCommandQueue;
static CAMetalLayer *gLayer;
static Renderer *gRenderer;
static CADisplayLink *gDisplayLink;

#include "app.h"

void start(void *contentView) {
  NSView *view = (__bridge NSView *)contentView;

  gDevice = MTLCreateSystemDefaultDevice();
  gCommandQueue = [gDevice newCommandQueue];

  gLayer = [CAMetalLayer layer];
  gLayer.device = gDevice;
  gLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;
  gLayer.frame = view.bounds;

  view.wantsLayer = YES;
  [view setLayer:gLayer];

  NSLog(@"Yo! Metal is ready: %@ (%dx%d)", gDevice.name,
        (int)view.bounds.size.width, (int)view.bounds.size.height);
  
  gRenderer = [[Renderer alloc] initWithDevice:gDevice layer:gLayer];
  gDisplayLink = [[NSScreen mainScreen] displayLinkWithTarget:gRenderer selector:@selector(draw:)];
  
  [gDisplayLink addToRunLoop:[NSRunLoop mainRunLoop] forMode:NSRunLoopCommonModes];
}

void stop(void) {
  [gDisplayLink invalidate];
  gDisplayLink = nil;
  gRenderer = nil;
}
