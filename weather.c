#include "weather.h"
#include "text.h"
#include "renderer.h"
#include "config.h"
#include "animation.h"
#include "math.h"

#include <stdlib.h>
#include <stdio.h>

#define FORECAST_SIZE 6
#define WEATHER_SIZE 12

const Weather WeatherDie[WEATHER_SIZE] = { 
    WEATHER_STORM, WEATHER_RAIN, WEATHER_RAIN, 
    WEATHER_SUNNY, WEATHER_SUNNY, WEATHER_SUNNY, 
    WEATHER_SUNNY, WEATHER_SUNNY, WEATHER_SUNNY, 
    WEATHER_SUNNY, WEATHER_SUNNY, WEATHER_SUNNY
};

static Weather _nextForecast = WEATHER_SUNNY;
//static Weather forecast;
static Weather _weather;

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
            return 0;
    }
}

static SpriteCode getWeatherBackground(Weather weather) {
    switch(weather) {
        case WEATHER_SUNNY:
            return SUN_BG;
        case WEATHER_CLOUDY:
            return CLOUD_BG;
        case WEATHER_RAIN:
            return RAIN_BG;
        case WEATHER_STORM:
            return STORM_BG;
        default:
            return 0;
    }
}


/**
 * Commented out while I settle on what the actual percentages should be
 */
void drawWeatherGuide(s32 position) {
    /*
    drawText(           "Forecast       Weather     Chance", LINES_MARGIN_LEFT, position, 1);
    position += STANDARD_MARGIN * 2;
    string text[4];
    sprintf(text[0],    "$%02x             $%02x           50%%",  SUN_SPRITE, SUN_SPRITE);
    sprintf(text[1],    "               $%02x           40%%",  CLOUD_SPRITE);
    sprintf(text[2],    "               $%02x           10%%",  RAIN_SPRITE);
    sprintf(text[3],    "               $%02x           0%%",  STORM_SPRITE);

    for (s32 i = 0; i < 4; i++) {
        drawText(text[i], LINES_MARGIN_LEFT, position, 1);
        position += STANDARD_MARGIN;
    }

    position += STANDARD_MARGIN;

    sprintf(text[0],    "$%02x             $%02x           0%%",  RAIN_SPRITE, SUN_SPRITE);
    sprintf(text[1],    "               $%02x           50%%",  CLOUD_SPRITE);
    sprintf(text[2],    "               $%02x           40%%",  RAIN_SPRITE);
    sprintf(text[3],    "               $%02x           10%%",  STORM_SPRITE);

    for (s32 i = 0; i < 4; i++) {
        drawText(text[i], LINES_MARGIN_LEFT, position, 1);
        position += STANDARD_MARGIN;
    }
    */
}

static bool _animationQueued = false;

void drawWeather() {
    Weather current = getCurrentWeather();
    drawBox(getWeatherBackground(current), LINES_MARGIN_LEFT, 0, LINE_SIZE * TILE_WIDTH, ROOF_POSITION);
    drawBox(INSIDE_BG, LINES_MARGIN_LEFT, ROOF_POSITION, LINE_SIZE * TILE_WIDTH, SCREEN_HEIGHT - ROOF_POSITION);
    drawSprite(
        INSIDE_SPRITE,
        LINES_MARGIN_LEFT + 4,
        ROOF_POSITION + 12,
        0, 1
    );

    drawSprite(
        getWeatherSprite(getCurrentWeather()),
        LINES_MARGIN_LEFT + 4,
        4,
        0, 1
    );

    // Goes over in the left-side "QUEUE" column
    drawSprite(
        getWeatherSprite(_nextForecast),
        QUEUE_MARGIN_LEFT,
        QUEUE_MARGIN_TOP + TILE_WIDTH,
        0, 1
    );

    if (_animationQueued) {
        _animationQueued = false;

        Animation* anim = newAnimation(8, 0);
        for (s32 i = 0; i < 8; i++) {
            setSimpleFrame(
                &anim->frames[i],
                getWeatherSprite(_nextForecast),
                (SCREEN_WIDTH - LINES_MARGIN_LEFT) / 2 + LINES_MARGIN_LEFT - i * 3,
                (INSIDE_LINE_POSITION - TOP_MARGIN) / 2 + TOP_MARGIN - i * 3,
                0.1
            );
            float scale = i * 0.5;
            anim->frames[i].scaleX = scale ? scale : 0.25;
            anim->frames[i].scaleY = scale ? scale : 0.25;
        }

        startAnimation(anim);
    }
}

Weather getCurrentWeather() {
    return _weather;
    /*
    switch (weather) {
        case WEATHER_STORM:
            return forecast == WEATHER_RAIN ? WEATHER_STORM : WEATHER_RAIN;
        case WEATHER_RAIN:
            return forecast == WEATHER_RAIN ? WEATHER_RAIN : WEATHER_CLOUDY;
        case WEATHER_SUNNY:
            return forecast == WEATHER_RAIN ? WEATHER_CLOUDY : WEATHER_SUNNY;
        default:
            // Should throw an error but that takes effort.
            return WEATHER_STORM;
    }
    */
}

Weather getForecast() {
    return _nextForecast;
}

void prepareNewDayWeather() {
    _animationQueued = true;
}

void newDayWeather() {
    _weather = _nextForecast;
    _nextForecast = WeatherDie[rand() % WEATHER_SIZE];
}
