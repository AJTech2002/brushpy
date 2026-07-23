#pragma once

#include "canvas.h"
#ifdef __cplusplus
extern "C" {
#endif

int display(Canvas *canvas);

// Builds the window/renderer/display-link and returns immediately, without
// entering [NSApp run]. Pair with pollEvents() for callers (e.g. Python) that
// need to keep driving their own loop while the window stays open.
void openWindow(Canvas *canvas);

// Non-blocking pump of one pass of pending Cocoa events. Call periodically
// after openWindow() to keep the window responsive.
void pollEvents(void);

void closeWindow(void);

#ifdef __cplusplus
}
#endif