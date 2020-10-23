#define INIT_TURN_SECONDS 0
#define INIT_TURN_CLOTHS 2
// Cos we need to fit a '+' and a '1'
#define MIN_CLOTH_SIZE 2

#define OUTSIDE_LINE_SIZE 16
#define INSIDE_LINE_SIZE 10

//#define RESOLUTION RESOLUTION_320x240
#define RESOLUTION RESOLUTION_640x480
//#if RESOLUTION == RESOLUTION_320x240 
//    #define SCREEN_WIDTH 320
//  #define SCREEN_HEIGHT 240
//    #define TILE_WIDTH 8
//    #define CHARACTER_SIZE 6
//#else
    #define SCREEN_WIDTH 640
    #define SCREEN_HEIGHT 480
    #define TILE_WIDTH 16
    #define CHARACTER_SIZE 11
//#endif

#define STANDARD_MARGIN TILE_WIDTH

// Weather frame
// TODO

// Queue Frame
#define QUEUE_MARGIN TILE_WIDTH
#define QUEUE_MARGIN_TOP (5 * TILE_WIDTH)

// Lines frame
#define LEFT_MARGIN (8 * TILE_WIDTH)
#define OUTSIDE_LINE_POSITION (5 * TILE_WIDTH)
#define ROOF_POSITION (10 * TILE_WIDTH)
#define INSIDE_LINE_POSITION (15 * TILE_WIDTH)

#define CLOTH_QUEUE_SIZE 15
#define CLOTH_QUEUE_SHOWN 3

#define COLOUR_DEPTH DEPTH_16_BPP

//#define RANDOMISE_CLOTHS 1
#define SHOW_FRAME_COUNT 1