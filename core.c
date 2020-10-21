#include <stdbool.h>

static bool _isRenderRequired = true;

bool isRenderRequired() {
    return _isRenderRequired;
}

void onRendered() {
    //_isRenderRequired = false;
}

void onStateChanged() {
    _isRenderRequired = true;
}

