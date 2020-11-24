#ifndef ANIMATION_INCLUDED
#define ANIMATION_INCLUDED

#include <libdragon.h>

typedef struct Animation Animation;

typedef struct {
    u32 x;
    u32 y;
    u32 z;
    u32 scaleX;
    u32 scaleY;
    SpriteCode sprite;
    // How long this frame should remain visible for
    u32 remainingCycles;
    // Link back to parent animation so next frame can be inserted.
    Animation* animation; 
    // Position in the draw queue so it can be abandoned
    u32 queueIndex;
    // So we can find the next frame in the sequence.
    u32 animationIndex;
} Frame;

struct Animation {
    Frame* frames;
    u32 numberOfFrames;
    timer_link_t* timer;
};

Animation* newAnimation(u32 numberOfFrames);
void startAnimation(Animation* animation);
void abandonAnimation(Animation* animation);

#endif