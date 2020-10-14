#ifndef CLOTH_MANAGER_INCLUDED
#define CLOTH_MANAGER_INCLUDED

#include "cloth.h"
#include <stdbool.h>

void initClothManager();
bool enqueueCloth();
Cloth* dequeueCloth();
void processFinishedCloths();


#endif