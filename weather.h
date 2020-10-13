#ifndef WEATHER_INCLUDED
#define WEATHER_INCLUDED

typedef enum { WEATHER_SUNNY, WEATHER_CLOUDY, WEATHER_RAIN, WEATHER_STORM } WEATHER;

WEATHER getCurrentWeather();
WEATHER getCurrentForecast();
void newDay();

#endif