#include "animation.h"
#include "renderer.h"

#define MAX_FRAMES
static Frame* _frameQueue[MAX_FRAMES];
static u32 _frameQueueIndex = 0;

void initAnimations() {
    memset(_frameQueue, 0, sizeof(_frameQueue));
}

Animation* newAnimation(u32 numberOfFrames) {
    Animation* result = calloc(1, sizeof(Animation));
    result->frames = calloc(numberOfFrames, sizeof(Frame));
    return result;
}

static void setUpNextFrame(Frame* frame) {
    Animation* animation = frame->animation;
    u32 index = frame->animationIndex + 1;

    if (index > animation->numberOfFrames) {
        return 0;
    }

    Frame* nextFrame = animation->frames[index];
    nextFrame->animation = animation;
    nextFrame->animationIndex = index;
    nextFrame->queueIndex = frame->queueIndex;
    _frameQueue[nextFrame->queueIndex] = nextFrame;
}

static void drawAnimations() {
    for(u32 i = 0; i < MAX_FRAMES; i++) {
        Frame* frame = _frameQueue[i];
        if (frame) {
            drawScaledSprite(frame->sprite, frame->x, frame->y, frame->z, frame->scaleX, frame->scaleY);
            frame->remainingCycles--;

            if (frame->remainingCycles == 0) {
                setUpNextFrame(frame);
            }
        }
    }
}

static void pushFrame(Frame* frame) {
    frame->index = _frameQueueIndex;
    _frameQueue[_frameQueueIndex] = frame;

    _frameQueueIndex++;

    // Thought of coming up with a robust system to allocate new indices,
    // But I'm in a hurry so lets just assume it's not gonna be a big deal if we have to wrap around.
    if (_frameQueueIndex >= MAX_FRAMES) {
        _frameQueueIndex = 0;
    }
}

static void removeFrame(Frame* frame) {
    _frameQueue[frame->index] = 0;
}

void startAnimation(Animation* animation) {
    Frame* frame = animation->frames[0];
    pushFrame(frame);
}

void abandonAnimation(Animation* animation) {
    delete_timer(animation->timer);

    for (u32 i = 0; i < animation->numberOfFrames; i++) {
        removeFrame((Frame*) animation->frames + (i * sizeof(Frame)]);
    }

    free(animation->frames);
    free(animation);
}

