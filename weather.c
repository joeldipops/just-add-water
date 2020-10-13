#include "weather.h"

const WEATHER ForecastDie[6] = { 
    WEATHER_RAIN, WEATHER_RAIN, WEATHER_RAIN,
    WEATHER_SUNNY, WEATHER_SUNNY, WEATHER_SUNNY
};
const WEATHER WeatherDie[8] = { 
    WEATHER_STORM,
    WEATHER_RAIN, WEATHER_RAIN, WEATHER_RAIN,
    WEATHER_SUNNY, WEATHER_SUNNY, WEATHER_SUNNY, WEATHER_SUNNY
};

static WEATHER forecast;
static WEATHER weather;

WEATHER getCurrentWeather() {
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

WEATHER getCurrentForecast() {
    return forecast;
}

void newDay() {
    // roll some dice.
}