#import <Cocoa/Cocoa.h>
#include "app.h"
#include "artwork.h"

@interface AppDelegate : NSObject <NSApplicationDelegate>
@end

@implementation AppDelegate
- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender {
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
    stop();
}
@end

int main() {
    [NSApplication sharedApplication];
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
    [NSApp setDelegate:[[AppDelegate alloc] init]];

    NSWindow* window = [[NSWindow alloc]
        initWithContentRect:NSMakeRect(0, 0, WIDTH, HEIGHT)
                  styleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskResizable
                    backing:NSBackingStoreBuffered
                      defer:NO];
    [window setTitle:@"BrushPY"];
    // [window setFrameAutosaveName:@"BrushPYMainWindow"];
    if (![[NSUserDefaults standardUserDefaults] objectForKey:@"NSWindow Frame BrushPYMainWindow"]) {
        [window center];
    }
    [window makeKeyAndOrderFront:nil];

    start((__bridge void*)window.contentView);
    artwork1();

    [NSApp activateIgnoringOtherApps:YES];
    [NSApp run];
}