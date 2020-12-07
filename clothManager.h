#ifndef CLOTH_MANAGER_INCLUDED
#define CLOTH_MANAGER_INCLUDED

#include "cloth.h"
#include <stdbool.h>

void initClothManager();
void increaseComplexity(u32 turnCount);
/**
 * Add a single cloth to the queue
 */
bool enqueueCloth();
/**
 * Add a set number of cloths to the queue depending on how far in to the game we are.
 */
bool enqueueClothsPerDay();
Cloth* dequeueCloth();
void processFinishedCloths();
void drawQueue();

#endif