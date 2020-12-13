#ifndef ANIMATION_INCLUDED
#define ANIMATION_INCLUDED

#include "core.h"
#include "text.h"
#include <libdragon.h>

#define MAX_FRAMES 32

typedef struct Animation Animation;

typedef struct {
    s32 x;
    s32 y;
    s32 z;
    float scaleX;
    float scaleY;
    SpriteCode sprite;
    // How long this frame should remain visible for
    s32 remainingCycles;
} Frame;

struct Animation {
    Frame frames[MAX_FRAMES];
    s32 currentFrameIndex;
    s32 queueIndex;
    s32 numberOfFrames;
};

void initAnimation();

void setSimpleFrame(Frame* frame, SpriteCode sprite, s32 x, s32 y, float seconds);

void drawAnimations();
Animation* newAnimation(s32 numberOfFrames);
void startAnimation(Animation* animation);
void abandonAnimation(Animation* animation);
void abandonAllAnimations();

#endif