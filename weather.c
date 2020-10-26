#include "weather.h"
#include "text.h"
#include "config.h"

#include <stdlib.h>
#include <stdio.h>

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

void drawWeatherGuide(u32 position) {
    drawText(           "Forecast       Weather     Chance", LEFT_MARGIN, position, 1);
    position += STANDARD_MARGIN * 2;
    string text[4];
    sprintf(text[0],    "$%02x             $%02x           50%%",  SUN_SPRITE, SUN_SPRITE);
    sprintf(text[1],    "               $%02x           40%%",  CLOUD_SPRITE);
    sprintf(text[2],    "               $%02x           10%%",  RAIN_SPRITE);
    sprintf(text[3],    "               $%02x           0%%",  STORM_SPRITE);

    for (u32 i = 0; i < 4; i++) {
        drawText(text[i], LEFT_MARGIN, position, 1);
        position += STANDARD_MARGIN;
    }

    position += STANDARD_MARGIN;

    sprintf(text[0],    "$%02x             $%02x           0%%",  RAIN_SPRITE, SUN_SPRITE);
    sprintf(text[1],    "               $%02x           50%%",  CLOUD_SPRITE);
    sprintf(text[2],    "               $%02x           40%%",  RAIN_SPRITE);
    sprintf(text[3],    "               $%02x           10%%",  STORM_SPRITE);

    for (u32 i = 0; i < 4; i++) {
        drawText(text[i], LEFT_MARGIN, position, 1);
        position += STANDARD_MARGIN;
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