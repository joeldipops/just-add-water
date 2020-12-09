#ifndef DAY_INCLUDED
#define DAY_INCLUDED

#include <stdbool.h>
#include "core.h"

void initDay();
void startNewDay();
void startFirstDay();
void cancelDayTimers();
void drawDay();
bool isWaiting();

#endif