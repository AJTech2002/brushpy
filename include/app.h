#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define WIDTH 800
#define HEIGHT 600

// metalLayer is a CAMetalLayer* already attached to the window's content
// view (passed as void* to stay ObjC-free in this header)
void start(void *metalLayer, int width, int height);
void stop(void);

// Advances one frame; called by the platform's display link.
void tick(void);

#ifdef __cplusplus
}
#endif