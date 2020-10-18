#include "weather.h"
#include "text.h"
#include "config.h"

#include <stdlib.h>

#define FORECAST_SIZE 6
#define WEATHER_SIZE 8

const Weather ForecastDie[FORECAST_SIZE] = { 
    WEATHER_RAIN, WEATHER_RAIN, WEATHER_RAIN,
    WEATHER_SUNNY, WEATHER_SUNNY, WEATHER_SUNNY
};
const Weather WeatherDie[WEATHER_SIZE] = { 
    WEATHER_STORM,
    WEATHER_RAIN, WEATHER_RAIN, WEATHER_RAIN,
    WEATHER_SUNNY, WEATHER_SUNNY, WEATHER_SUNNY, WEATHER_SUNNY
};

static Weather nextForecast;
static Weather forecast;
static Weather weather;

static SpriteCode getWeatherSprite(Weather weather) {
    switch(weather) {
        case WEATHER_SUNNY:
            return SUN_SPRITE;
        case WEATHER_CLOUDY:
            return CLOUD_SPRITE;
        case WEATHER_RAIN:
            return RAIN_SPRITE;
        case WEATHER_STORM:
            return STORM_SPRITE;
        default:
            return DRENCHED_SPRITE;
    }
}

void drawWeather() {
    drawText(
        "TODAY", 
        LEFT_MARGIN,
        STANDARD_MARGIN,
        1
    );

    drawSprite(
        getWeatherSprite(getCurrentWeather()),
        LEFT_MARGIN,
        STANDARD_MARGIN * 2 + TILE_WIDTH,
        1
    );
    

    drawText(
        "PREDICTION", 
        LEFT_MARGIN + TILE_WIDTH * 5 + STANDARD_MARGIN * 2,
        STANDARD_MARGIN,
        1
    );

    drawSprite(
        getWeatherSprite(nextForecast),
        LEFT_MARGIN + TILE_WIDTH * 5 + STANDARD_MARGIN * 2,
        STANDARD_MARGIN * 2 + TILE_WIDTH,
        1
    );
}

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
    nextForecast = ForecastDie[rand() % FORECAST_SIZE];
    weather = WeatherDie[rand() % WEATHER_SIZE];
}