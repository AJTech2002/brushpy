#include "app.h"
#include "artwork.h"
#include "main.h"
#import <Cocoa/Cocoa.h>
#import <QuartzCore/CADisplayLink.h>
#import <QuartzCore/CAMetalLayer.h>

static CADisplayLink *gDisplayLink;
static id gDisplayLinkTarget;

// Thin ObjC trampoline: CADisplayLink requires an ObjC target/selector.
// All rendering logic lives in the C++ engine (see app.cpp).
@interface DisplayLinkTarget : NSObject
@end
@implementation DisplayLinkTarget
- (void)onTick:(CADisplayLink *)link {
  tick();
}
@end

@interface AppDelegate : NSObject <NSApplicationDelegate>
@end

@implementation AppDelegate
- (BOOL)applicationShouldTerminateAfterLastWindowClosed:
    (NSApplication *)sender {
  return YES;
}
- (void)applicationDidFinishLaunching:(NSNotification *)notification {
  [NSEvent addLocalMonitorForEventsMatchingMask:NSEventMaskKeyDown
                                        handler:^NSEvent *(NSEvent *event) {
                                          if (event.keyCode == 53) { // 53 = Esc
                                            [NSApp terminate:nil];
                                            return nil;
                                          }
                                          return event;
                                        }];
}
- (void)applicationWillTerminate:(NSNotification *)notification {
  [gDisplayLink invalidate];
  gDisplayLink = nil;
  gDisplayLinkTarget = nil;
  stop();
}
@end

int start_brushpy() {
  [NSApplication sharedApplication];
  [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
  [NSApp setDelegate:[[AppDelegate alloc] init]];

  NSWindow *window = [[NSWindow alloc]
      initWithContentRect:NSMakeRect(0, 0, WIDTH, HEIGHT)
                styleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskClosable |
                          NSWindowStyleMaskResizable
                  backing:NSBackingStoreBuffered
                    defer:NO];
  [window setTitle:@"BrushPY"];
  // [window setFrameAutosaveName:@"BrushPYMainWindow"];
  if (![[NSUserDefaults standardUserDefaults]
          objectForKey:@"NSWindow Frame BrushPYMainWindow"]) {
    [window center];
  }
  [window makeKeyAndOrderFront:nil];

  NSView *view = window.contentView;
  CAMetalLayer *layer = [CAMetalLayer layer];
  layer.frame = view.bounds;
  view.wantsLayer = YES;
  view.layer = layer;

  start((__bridge void *)layer, (int)view.bounds.size.width,
        (int)view.bounds.size.height);

  gDisplayLinkTarget = [[DisplayLinkTarget alloc] init];
  gDisplayLink =
      [[NSScreen mainScreen] displayLinkWithTarget:gDisplayLinkTarget
                                          selector:@selector(onTick:)];
  [gDisplayLink addToRunLoop:[NSRunLoop mainRunLoop]
                     forMode:NSRunLoopCommonModes];

  [NSApp activateIgnoringOtherApps:YES];
  [NSApp run];

  return 0;
}
