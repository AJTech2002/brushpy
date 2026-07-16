#include "app.h"
#include "Renderer.h"
#import <AppKit/NSScreen.h>
#import <Cocoa/Cocoa.h>
#import <Metal/Metal.h>
#import <QuartzCore/CADisplayLink.h>
#import <QuartzCore/CAMetalLayer.h>

static id<MTLDevice> gDevice;
static CAMetalLayer *gLayer;
static CADisplayLink *gDisplayLink;

// Thin ObjC trampoline: CADisplayLink requires an ObjC target/selector.
// All rendering logic lives in the C++ Renderer.
@interface DisplayLinkTarget : NSObject
@end
@implementation DisplayLinkTarget
- (void)tick:(CADisplayLink *)link {
  Renderer::instance().draw();
}
@end
static DisplayLinkTarget *gTarget;

void start(void *contentView) {
  NSView *view = (__bridge NSView *)contentView;

  gDevice = MTLCreateSystemDefaultDevice();

  gLayer = [CAMetalLayer layer];
  gLayer.device = gDevice;
  gLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;
  gLayer.frame = view.bounds;

  view.wantsLayer = YES;
  [view setLayer:gLayer];

  NSLog(@"Metal ready: %@ (%dx%d)", gDevice.name, (int)view.bounds.size.width,
        (int)view.bounds.size.height);

  // Cast ObjC pointers to metal-cpp C++ types (same pointer, different type).
  MTL::Device *device = (MTL::Device *)(__bridge void *)gDevice;
  CA::MetalLayer *layer = (CA::MetalLayer *)(__bridge void *)gLayer;
  Renderer::init(device, layer);

  gTarget = [[DisplayLinkTarget alloc] init];
  gDisplayLink = [[NSScreen mainScreen] displayLinkWithTarget:gTarget
                                                     selector:@selector(tick:)];
  [gDisplayLink addToRunLoop:[NSRunLoop mainRunLoop]
                     forMode:NSRunLoopCommonModes];
}

void stop(void) {
  [gDisplayLink invalidate];
  gDisplayLink = nil;
  gTarget = nil;
  Renderer::destroy();
}
