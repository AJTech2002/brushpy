#import <Cocoa/Cocoa.h>
#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>

static id<MTLDevice> gDevice;
static id<MTLCommandQueue> gCommandQueue;
static CAMetalLayer *gLayer;

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
}
