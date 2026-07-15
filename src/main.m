#import <Cocoa/Cocoa.h>
#include "app.h"

@interface AppDelegate : NSObject <NSApplicationDelegate>
@end

@implementation AppDelegate
- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender {
    return YES;
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
        initWithContentRect:NSMakeRect(0, 0, 800, 600)
                  styleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskResizable
                    backing:NSBackingStoreBuffered
                      defer:NO];
    [window setTitle:@"BrushPY"];
    [window center];
    [window makeKeyAndOrderFront:nil];

    start((__bridge void*)window.contentView);

    [NSApp activateIgnoringOtherApps:YES];
    [NSApp run];
}