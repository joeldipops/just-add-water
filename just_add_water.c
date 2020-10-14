#include "weather.h"
#include "line.h"
#include <libdragon.h>

int main(void) {
    initLine();

    while(true) {
        newDayWeather();
        //updateClothQueue();
        updateHangingCloths(getCurrentWeather());
    }
}