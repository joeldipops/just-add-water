#include "animation.h"
#include "renderer.h"

#define MAX_ANIMATIONS 256
static Animation* _queue[MAX_ANIMATIONS];
static s32 _queueIndex = 0;

void initAnimation() {
    memset(_queue, 0, sizeof(Animation*) * MAX_ANIMATIONS);
}

/**
 * Constructor.
 * @param numberOfFrames.
 */
Animation* newAnimation(s32 numberOfFrames, u32 typeCode) {
    Animation* result = calloc(1, sizeof(Animation));
    result->currentFrameIndex = 0;
    result->numberOfFrames = numberOfFrames;
    result->typeCode = typeCode;

    // undefined.
    result->queueIndex = -1;

    return result;
}

void drawAnimations() {
    for(s32 i = 0; i < MAX_ANIMATIONS; i++) {
        Animation* animation = _queue[i];
        if (animation) {
            Frame* frame = &animation->frames[animation->currentFrameIndex];
            if (frame && frame->remainingCycles) {
                drawScaledSprite(frame->sprite, frame->x, frame->y, frame->z, frame->scaleX, frame->scaleY);
                frame->remainingCycles--;
                if (frame->remainingCycles == 0) {
                    animation->currentFrameIndex++;
                }
            } else {
                animation->currentFrameIndex++;
            }

            if (animation->currentFrameIndex >= animation->numberOfFrames) {
                abandonAnimation(animation);
            }
        }
    }
}

static void addToQueue(Animation* animation) {
    animation->currentFrameIndex = 0;
    animation->queueIndex = _queueIndex;
    _queue[_queueIndex] = animation;

    _queueIndex++;

    // Thought of coming up with a robust system to allocate new indices,
    // But I'm in a hurry so lets just assume it's not gonna be a big deal if we have to wrap around.
    if (_queueIndex >= MAX_ANIMATIONS) {
        _queueIndex = 0;
    }
}

static void removeFromQueue(Animation* animation) {
    _queue[animation->queueIndex] = 0;
    animation->queueIndex = 0;
}

void startAnimation(Animation* animation) {
    addToQueue(animation);
}

void abandonAnimation(Animation* animation) {
    removeFromQueue(animation);
    free(animation);
}

void abandonAnimationsOfType(u32 typeCode) {
    for (s32 i = 0; i < MAX_ANIMATIONS; i++) {
        if (_queue[i] && _queue[i]->typeCode == typeCode) {
            abandonAnimation(_queue[i]);
        }
    }
}

void abandonAllAnimations() {
    for (s32 i = 0; i < MAX_ANIMATIONS; i++) {
        if (_queue[i]) {
            abandonAnimation(_queue[i]);
        }
    }
}


void setSimpleFrame(Frame* frame, SpriteCode sprite, s32 x, s32 y, float seconds) {
    frame->sprite = sprite;
    frame->x = x;
    frame->y = y;
    frame->z = 3;
    frame->scaleX = 1;
    frame->scaleY = 1;
    frame->remainingCycles = seconds * 30; // assumes 30fps
}

