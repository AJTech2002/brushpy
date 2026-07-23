#include "app.h"
#include "artwork.h"
#import "canvas.h"
#include "main.h"
#import <Cocoa/Cocoa.h>
#import <QuartzCore/CADisplayLink.h>
#import <QuartzCore/CAMetalLayer.h>

static CADisplayLink *gDisplayLink;
static id gDisplayLinkTarget;
static NSWindow *gWindow;
static bool gClosed = true;

// true when opened via openWindow() (e.g. from Python), where the caller
// drives its own loop with pollEvents() and the process must outlive the
// window; false when opened via display()+[NSApp run] (the standalone
// BrushPY binary), where closing the window should end the process.
static bool gEmbedded = false;

// Thin ObjC trampoline: CADisplayLink requires an ObjC target/selector.
// All rendering logic lives in the C++ engine (see app.cpp).
@interface DisplayLinkTarget : NSObject
@end
@implementation DisplayLinkTarget
- (void)onTick:(CADisplayLink *)link {
  tick();
}
@end

@interface AppDelegate : NSObject <NSApplicationDelegate, NSWindowDelegate>
@end

@implementation AppDelegate
- (BOOL)applicationShouldTerminateAfterLastWindowClosed:
    (NSApplication *)sender {
  return YES;
}
- (BOOL)windowShouldClose:(NSWindow *)sender {
  if (gEmbedded) {
    // Same reasoning as the Esc handler: don't let this fall through to
    // NSApp's terminate machinery under Python. We tear the window down
    // ourselves and tell AppKit not to also close it.
    closeWindow();
    return NO;
  }
  return YES;
}
- (void)applicationDidFinishLaunching:(NSNotification *)notification {
  [NSEvent addLocalMonitorForEventsMatchingMask:NSEventMaskKeyDown
                                        handler:^NSEvent *(NSEvent *event) {
                                          if (event.keyCode == 53) { // 53 = Esc
                                            if (gEmbedded) {
                                              // Under Python, the process
                                              // must survive the window
                                              // closing: just tear the
                                              // window down. Calling
                                              // [NSApp terminate:] here
                                              // would call exit(0), which
                                              // runs C++ static destructors
                                              // (e.g. Renderer::drawCallbacks,
                                              // which holds captured
                                              // pybind11::function objects)
                                              // while the Python interpreter
                                              // is mid-teardown, hanging and
                                              // then crashing.
                                              closeWindow();
                                            } else {
                                              [NSApp terminate:nil];
                                            }
                                            return nil;
                                          }
                                          return event;
                                        }];
}
- (void)applicationWillTerminate:(NSNotification *)notification {
  closeWindow();
}
@end

static void setupWindow(Canvas *canvas) {
  [NSApplication sharedApplication];
  [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
  AppDelegate *appDelegate = [[AppDelegate alloc] init];
  [NSApp setDelegate:appDelegate];

  NSWindow *window = [[NSWindow alloc]
      initWithContentRect:NSMakeRect(0, 0, canvas->width(), canvas->height())
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
  gWindow = window;
  gClosed = false;

  NSView *view = window.contentView;
  CAMetalLayer *layer = [CAMetalLayer layer];
  layer.frame = view.bounds;
  view.wantsLayer = YES;
  view.layer = layer;

  start_renderer((__bridge void *)layer, (int)view.bounds.size.width,
                 (int)view.bounds.size.height, canvas);

  gDisplayLinkTarget = [[DisplayLinkTarget alloc] init];
  gDisplayLink =
      [[NSScreen mainScreen] displayLinkWithTarget:gDisplayLinkTarget
                                          selector:@selector(onTick:)];
  [gDisplayLink addToRunLoop:[NSRunLoop mainRunLoop]
                     forMode:NSRunLoopCommonModes];

  [NSApp activateIgnoringOtherApps:YES];
}

int display(Canvas *canvas) {
  gEmbedded = false;
  setupWindow(canvas);
  [NSApp run];
  return 0;
}

void openWindow(Canvas *canvas) {
  gEmbedded = true;
  setupWindow(canvas);
  // Normally done inside -[NSApplication run]; needed here since we're
  // driving the event loop ourselves via pollEvents() instead.
  [NSApp finishLaunching];
}

// Tears the window/renderer/display-link down without touching the process
// (no [NSApp terminate:]/exit()). Idempotent so it's safe to call from the
// Esc handler, applicationWillTerminate:, and directly from Python.
void closeWindow(void) {
  if (gClosed)
    return;
  gClosed = true;

  [gDisplayLink invalidate];
  gDisplayLink = nil;
  gDisplayLinkTarget = nil;

  stop();

  [gWindow close];
  gWindow = nil;

  // Closing a window only takes visual effect once the run loop processes
  // the resulting WindowServer round-trip. Under Python, nothing may ever
  // call pollEvents() again after this, so the window would otherwise sit
  // on screen looking closed-but-frozen ("Application Not Responding").
  // Drain events ourselves for a brief window to let the close flush.
  NSDate *deadline = [NSDate dateWithTimeIntervalSinceNow:0.25];
  NSEvent *event;
  while ([deadline timeIntervalSinceNow] > 0 &&
         (event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                      untilDate:deadline
                                         inMode:NSDefaultRunLoopMode
                                        dequeue:YES])) {
    [NSApp sendEvent:event];
  }
  [NSApp updateWindows];

  if (gEmbedded) {
    // Stop presenting as a foreground GUI app now that there's no window
    // and no one left pumping events for it.
    [NSApp setActivationPolicy:NSApplicationActivationPolicyProhibited];
  }
}

void pollEvents(void) {
  NSEvent *event;
  while ((event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                     untilDate:[NSDate distantPast]
                                        inMode:NSDefaultRunLoopMode
                                       dequeue:YES])) {
    [NSApp sendEvent:event];
  }
  [NSApp updateWindows];
}
