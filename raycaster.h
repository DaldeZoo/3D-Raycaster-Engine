#ifndef RAYCASTER_H
#define RAYCASTER_H

#include <GL/glut.h>
#include <math.h>

#define PI 3.1415926535
#define DG 0.0174533 // 1 degree in radians (since OpenGL uses radians)
#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 512

#define MAP_X 8
#define MAP_Y 8
#define BLOCK_SIZE 64
// Note: Since the Window's dimensions are 1024x512 pixels and each block size is 64,
// then we can have a total of 1024/64 = 16 blocks horizontally and 512/64 = 8 blocks
// vertically (if we want to cover the whole screen).

extern float px, py; // Player position (px, py)
extern float p_angle, pdx, pdy; // Player angle, player change in x, and player change in y
extern int speed; // Player speed
extern int map[MAP_Y][MAP_X]; // 2D Map layout

// Note: Integer coordinates ensure precise alignment with the pixel grid.
// Integer coordinates are clearer and more suitable for representing discrete
// pixel positions, ensuring clarity and precision in rendering
// tasks like drawing a map layout.
void drawMap2D();
// This function basically works by checking each grid line (ie index in 2D array map)
// for walls and continues to increase the ray's length until a wall is detected.
// We do this by first setting (rx, ry) to the nearest grid line from the player based on their angle
// and postion (px, py). Then we set the increment/decrement or offset values (rxo, ryo) to their respective
// values depending on the players viewing angle. Then as long as the ray's length doesnt go beyond the map's
// boundaries (set to 8x's and 8y's in this raycaster) "r_length", we keep offsetting (ie increasing/decreasing)
// the values (rx, ry) by their offset values (rxo, ryo) until a wall is encountered.
// Then we simply draw the ray from the player's position to the final ray endpoint we reached (rx, ry).
// We do this in two seperate checks: a Horizontical grid line check and a Vertical one.
// Note: since we only want one ray for each angle (not both the horizontal and vertical at once),
// we just need the shorter one of the two (the one which hits the wall first),
// so we find both rays' respective distances and only draw the shorter one.
// Note: all 64s are actually BLOCK_SIZE - better to put BLOCK_SIZE instead but 64 looks neater :) -
void drawRays3D();
void drawPlayer();


float dist(float ax, float ay, float bx, float by);

// Detects walls and prevents player from going through them by simply adding/subtracting the player offset values pdx and pdy.
// Recall in "buttons" function, we add/subtract, for example, px with pdx and py with pdy when 'w' is pressed.
// So, this function, if a wall was detected, would subtract pdx and pdy from px and py respectively, basically doing nothing
// ie not moving the player at all - px += pdx -> WALL! -> px -= pdx.
void detectWallFront();
// Works almost identically to "detectWallBack"
void detectWallBack();

#endif