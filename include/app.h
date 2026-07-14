#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// contentView is NSView* (passed as void* to stay ObjC-free in this header)
void start(void* contentView);

#ifdef __cplusplus
}
#endif