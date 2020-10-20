#ifndef CORE_INCLUDED
#define CORE_INCLUDED

#include <libdragon.h>

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t  s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef char string[128];

typedef struct {
    size_t size;
    u8* data;
} Buffer;

typedef struct controller_data N64ControllerState;

bool isRenderRequired();
void onRendered();
void onStateChanged();

#define TICKS_PER_SECOND 1000000
#endif