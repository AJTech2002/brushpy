#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define WIDTH 800
#define HEIGHT 600

// contentView is NSView* (passed as void* to stay ObjC-free in this header)
void start(void *contentView);
void stop(void);

#ifdef __cplusplus
}
#endif