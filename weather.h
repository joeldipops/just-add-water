#ifndef WEATHER_INCLUDED
#define WEATHER_INCLUDED

#include "core.h"

typedef enum { WEATHER_SUNNY, WEATHER_CLOUDY, WEATHER_RAIN, WEATHER_STORM } Weather;

void drawWeather();
void drawWeatherGuide(s32 position);
Weather getCurrentWeather();
Weather getForecast();
void newDayWeather();
void prepareNewDayWeather();

#endif