#include "line.h"
#include "cloth.h"
#include "weather.h"

#define OUTSIDE_LINE_SIZE 16
#define INSIDE_LINE_SIZE 8

static Cloth* outsideCloths[OUTSIDE_LINE_SIZE];
static Cloth* insideCloths[INSIDE_LINE_SIZE];

void initLine() {}

void updateHangingCloths(Weather weather) {
    for(u32 i = 0; i < OUTSIDE_LINE_SIZE; i++) {
        if (outsideCloths[i]) {
            updateCloth(outsideCloths[i], weather);
        }
    }

    // Inside is always considered 'cloudy'
    for(u32 i = 0; i < INSIDE_LINE_SIZE; i++) {
        if (insideCloths[i]) {
            updateCloth(insideCloths[i], WEATHER_CLOUDY);
        }
    }


}