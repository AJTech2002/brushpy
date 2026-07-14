#import <Cocoa/Cocoa.h>
#include "app.h"

int main() {
    [NSApplication sharedApplication];
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];

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