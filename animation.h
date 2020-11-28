#ifndef ANIMATION_INCLUDED
#define ANIMATION_INCLUDED

#include "core.h"
#include "text.h"
#include <libdragon.h>

#define MAX_FRAMES 32

typedef struct Animation Animation;

typedef struct {
    u32 x;
    u32 y;
    u32 z;
    float scaleX;
    float scaleY;
    SpriteCode sprite;
    // How long this frame should remain visible for
    u32 remainingCycles;
} Frame;

struct Animation {
    Frame frames[MAX_FRAMES];
    u32 currentFrameIndex;
    u32 queueIndex;
    u32 numberOfFrames;
};

void initAnimation();

void setSimpleFrame(Frame* frame, SpriteCode sprite, u32 x, u32 y, float seconds);

void drawAnimations();
Animation* newAnimation();
void startAnimation(Animation* animation);
void abandonAnimation(Animation* animation);

#endif