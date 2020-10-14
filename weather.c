#include "weather.h"
#include <stdlib.h>

const Weather ForecastDie[6] = { 
    WEATHER_RAIN, WEATHER_RAIN, WEATHER_RAIN,
    WEATHER_SUNNY, WEATHER_SUNNY, WEATHER_SUNNY
};
const Weather WeatherDie[8] = { 
    WEATHER_STORM,
    WEATHER_RAIN, WEATHER_RAIN, WEATHER_RAIN,
    WEATHER_SUNNY, WEATHER_SUNNY, WEATHER_SUNNY, WEATHER_SUNNY
};

static Weather nextForecast;
static Weather forecast;
static Weather weather;

Weather getCurrentWeather() {
    switch (weather) {
        case WEATHER_STORM:
            return forecast == WEATHER_RAIN ? WEATHER_STORM : WEATHER_RAIN;
        case WEATHER_RAIN:
            return forecast == WEATHER_RAIN ? WEATHER_RAIN : WEATHER_CLOUDY;
        case WEATHER_SUNNY:
            return WEATHER_SUNNY;
        default:
            // Should throw an error but that takes effort.
            return WEATHER_STORM;
    }
}

Weather getForecast() {
    return nextForecast;
}

void newDayWeather() {
    forecast = nextForecast;
    nextForecast = rand() % 6;
    weather = rand() % 8;
}