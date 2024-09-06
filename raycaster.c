#include <stdio.h>
#include <stdlib.h>
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

float px, py; // Player position (px, py)
float p_angle, pdx, pdy; // Player angle, player change in x, and player change in y
int speed = 5; // Player speed
int map[MAP_Y][MAP_X] = // 2D Map layout
{
    {1,1,1,1,1,1,1,1},
    {1,1,0,0,0,1,1,1},
    {1,0,0,0,0,0,1,1},
    {1,1,1,0,0,0,0,1},
    {1,0,0,1,0,0,1,1},
    {1,0,0,0,1,0,0,1},
    {1,1,0,0,0,0,1,1},
    {1,1,1,1,1,1,1,1},
};

void drawMap2D()
{
    // Note: Integer coordinates ensure precise alignment with the pixel grid.
    // Integer coordinates are clearer and more suitable for representing discrete
    // pixel positions, ensuring clarity and precision in rendering
    // tasks like drawing a map layout.

    int x, y; // Used to traverse the 2D map
    // These two represent the top left corner of every square (wall) we draw:
    int xi = 0;
    int yi = WINDOW_HEIGHT;
    for (y = 0; y < MAP_Y; y++)
    {
        xi = 0; // Resetting xi (for drawing in the next column)
        yi = WINDOW_HEIGHT - y*BLOCK_SIZE; // Iterates and resets yi (for drawing in the next row)
        for (x = 0; x < MAP_X; x++)
        {
            if (map[y][x] == 1) glColor3f(1,1,1);
            else glColor3f(0,0,0);
    
            // Note: the +-1's are for making grid lines.
            glBegin(GL_QUADS);
                glVertex2i(xi + 1, yi - 1);
                glVertex2i(xi + 1, (yi - BLOCK_SIZE) + 1);
                glVertex2i((xi + BLOCK_SIZE) - 1, (yi - BLOCK_SIZE) + 1);
                glVertex2i((xi + BLOCK_SIZE) - 1, yi - 1);
            glEnd();

            xi = xi + BLOCK_SIZE; // Iterates xi
        }
    }
}

// TODO: fix front and back wall detection - at certain angles the player can clip through the wall :(

void detectWallFront()
{
    // Detects walls and prevents player from going through them by simply adding/subtracting the player offset values pdx and pdy. 
    // Recall in "buttons" function, we add/subtract, for example, px with pdx and py with pdy when 'w' is pressed.
    // So, this function, if a wall was detected, would subtract pdx and pdy from px and py respectively, basically doing nothing
    // ie not moving the player at all - px += pdx -> WALL! -> px -= pdx.

    int mapx = (int)(px + pdx/speed*25) / 64;
    int mapy = 8 - ((int)(py + pdy/speed*25) / 64) - 1;
    if ((mapx >= 0 && mapy >= 0) && (mapx < MAP_X && mapy < MAP_Y) && map[mapy][mapx] == 1)
    {
        glColor3f(1,0,0);
        px -= pdx; py -= pdy;
    }
}

void detectWallBack()
{    
    // Works almost identically to "detectWallBack".

    int mapx = (int)(px) / 64;
    int mapy = 8 - ((int)(py) / 64) - 1;
    if ((mapx >= 0 && mapy >= 0) && (mapx < MAP_X && mapy < MAP_Y) && map[mapy][mapx] == 1)
    {
        glColor3f(1,0,0);
        px += pdx; py += pdy;
    } 
}

void drawPlayer()
{
    glColor3f(1,1,0);
    detectWallBack(); // Detects walls behind the player
    glPointSize(8.0);
    glBegin(GL_POINTS);
        glVertex2i(px,py);
    glEnd();

    detectWallFront(); // Detects walls infront of the player

    // Direction line:
    glLineWidth(3.0);
    glBegin(GL_LINES);
        glVertex2i(px,py);
        glVertex2i(px + pdx/speed*25, py + pdy/speed*25);
    glEnd();
}

// Helper function
float dist(float ax, float ay, float bx, float by)
{
    return sqrt((bx-ax)*(bx-ax) + (by-ay)*(by-ay));
}

void drawRays3D()
{
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

    float rx, ry; // The endpoints our ray
    float rxo, ryo; // The ray's x and y offsets

    // (Initialized to +30 degrees to be in the middle of our 60 degree ray blast)
    float r_angle = p_angle + DG*30; // The ray's angle
    if (r_angle > 2*PI) r_angle -= 2*PI;
    else if (r_angle < 0) r_angle += 2*PI;

    int r_count; // Number of rays to be casted
    int r_length; // Used to stay in bounds when drawing rays and to exit loops
    int mapx, mapy; // Our 2D map indexes corresponding to our rx and ry values
    float hx, hy, h_dist, vx, vy, v_dist; // Used to find which ray (horizontal or vertical) is shorter
    float final_dist; // The final line (to be 3D drawn) distance

    for (r_count = 0; r_count < 60; r_count++)
    {
        // Horizontal Lines Check:
        h_dist = 10000; hx = px; hy = py;
        r_length = 0;
        if (r_angle > PI) // Looking down (check unit circle)
        {
            ry = ((int)(py / 64)) * 64; // Rounds to the nearest (lower) horizontal grid line from player's y-position (py)
            rx = ((1/tan(r_angle)) * (ry-py)) + px; // Using right-angle triangles + SOHCAHTOA to find rx (2 points, 1 angle)
            ryo = -64; // Since each block size is 64
            rxo = (1/tan(r_angle)) * ryo; // Also done with the same procedure as rx, only simpler
        }
        else if (r_angle < PI) // Looking up (check unit circle)
        {
            ry = (((int)(py / 64)) * 64) + 64; // Rounds to the nearest (upper) horizontal grid line from player's y-position (py)
            rx = ((1/tan(r_angle)) * (ry-py)) + px; // Using right-angle triangles + SOHCAHTOA to find rx (2 points, 1 angle)
            ryo = 64; // Since each block size is 64
            rxo = (1/tan(r_angle)) * ryo;  // Also done with the same procedure as rx, only simpler
        }
        else // (r_angle == 0 || r_angle == PI) - Perfectly horizontal rays are parallel to horizontal grid lines, ie they never hit
        {
            rx = px; ry = py;
            r_length = 8; // Break
        }
        while (r_length < 8)
        {
            // Map indexes corresponding to rx and ry
            mapx = (int)(rx) / 64; mapy = 8 - ((int)(ry) / 64);
            if (r_angle < PI) mapy--;
            if ((mapx >= 0 && mapy >= 0) && (mapx < MAP_X && mapy < MAP_Y) && map[mapy][mapx] == 1) // Check for wall
            {
                hx = rx; hy = ry;
                h_dist = dist(px, py, hx, hy);
                r_length = 8; // Break
            }
            else // Check next block
            {
                rx += rxo; ry += ryo;
                r_length++;
            }
        }

        // Vertical Lines Check:
        v_dist = 10000; vx = px; vy = py;
        r_length = 0;
        if (r_angle > PI/2 && r_angle < 3*PI/2) // Looking left (check unit circle)
        {
            rx = ((int)(px / 64)) * 64; // Rounds to the nearest (left) vertical grid line from player's x-position (px)
            ry = ((tan(r_angle)) * (rx-px)) + py; // Using right-angle triangles + SOHCAHTOA to find ry (2 points, 1 angle)
            rxo = -64; // Since each block size is 64
            ryo = (tan(r_angle)) * rxo; // Also done with the same procedure as ry, only simpler
        }
        else if (r_angle < PI/2 || r_angle > 3*PI/2) // Looking right (check unit circle)
        {
            rx = (((int)(px / 64)) * 64) + 64; // Rounds to the nearest (right) vertical grid line from player's x-position (px)
            ry = ((tan(r_angle)) * (rx-px)) + py; // Using right-angle triangles + SOHCAHTOA to find ry (2 points, 1 angle)
            rxo = 64; // Since each block size is 64
            ryo = (tan(r_angle)) * rxo;  // Also done with the same procedure as ry, only simpler
        }
        else // (r_angle == 0 || r_angle == PI) - Perfectly vertical rays are parallel to vertical grid lines, ie they never hit
        {
            rx = px; ry = py;
            r_length = 8; // Break
        }
        while (r_length < 8)
        {
            // Map indexes corresponding to rx and ry
            mapx = (int)(rx) / 64; mapy = (8 - ((int)(ry) / 64)) - 1;
            if (r_angle > PI/2 && r_angle < 270) mapx--;
            if (r_angle > PI && r_angle > 3*PI/2) mapx++;
            if ((mapx >= 0 && mapy >= 0) && (mapx < MAP_X && mapy < MAP_Y) && map[mapy][mapx] == 1)
            {
                vx = rx; vy = ry;
                v_dist = dist(px, py, vx, vy);
                r_length = 8; // Break
            }
            else // Check next block
            {
                rx += rxo; ry += ryo;
                r_length++;
            }
        }

        // Check which ray hits the wall first (the shorter one will be drawn)
        if (h_dist < v_dist)
        {
        	glColor3f(0.4,0,0.7);
            final_dist = h_dist;
            rx = hx; ry = hy;
        }
        else
        {
            glColor3f(0.6,0,0.9);
            final_dist = v_dist;
            rx = vx; ry = vy;
        }

        // Draw 2D ray
        glLineWidth(3);
        glBegin(GL_LINES);
            glVertex2i(px, py);
            glVertex2i(rx, ry);
        glEnd();

        // Offset next ray in ray blast to -1 degree
        r_angle -= DG;
        if (r_angle > 2*PI) r_angle -= 2*PI;
        else if (r_angle < 0) r_angle += 2*PI;

        // Fix fisheye by basically making the lines more linear
        float c_angle = p_angle - r_angle;
        if (c_angle > 2*PI) c_angle -= 2*PI;
        else if (c_angle < 0) c_angle += 2*PI;
        final_dist = final_dist*cos(c_angle);

        // Draw 3D scene
        // Chose window height to be 448 pixels long
        // (Change 448 to some other multiple of 64 if you want to change 3D scene window)
        float line_height = (64 * 448)/final_dist; // The larger the distance the shorter the wall
        if (line_height > 448) line_height = 448; // Caps wall height
        float line_offset = 256-(line_height/2); // Offsets window to be in the middle of the screen

        // Draw 3D line
        glLineWidth(8); // Wall width - make it 12 if you want fullscreen
        glBegin(GL_LINES);
            glVertex2i(r_count*8 + 530, line_offset); // The *8 dictates how close the wall lines are
            glVertex2i(r_count*8 + 530, line_height + line_offset); // The +530 shiftes the scene to the right
        glEnd();
    }
}

void buttons(unsigned char key, int x, int y)
{
    // When 'a' (left) is pressed we rotate the player's angle (his fov - dictated by the ray)
    // in the counterclockwise direction (because we are using sine and cosine - think of unit circle)
    // and when 'd' (right) is pressed we rotate in the clockwise direction...
    // We do so by increasing (a/left) and decreasing (d/right) p_angle when 'a' or 'd' are pressed.
    // Then the actual movement (or change in position) is only forward and backward (depending on 
    // where the player is facing).
    // 'pdx' and 'pdy' are direction vectors (delta x and delta y ie change in x or change in y)
    // (pdx, pdy) or more like the points on the unit circle.
    // Like (cos(pi/6), sin(pi/6)) = (sqrt3/2, 1/2) --- check "direction line" in drawPlayer function.
    // Note: but since the values of sin and cosine are so small (between -1 and 1) then we multiply
    // pdx and pdy by 5 so that we can actually notice the change.
    // The actual movement or change in position of the player is basically just an increment
    // or decrement of by direction vectors ('w' increments - 's' decrements).
    // So for example if p_angle = pi then pdx = cos(pi)*5 ~= 5 so px += 5 (if 'w')
    // and pdy = sin(pi)*5 ~= 0.3 so pdy += 0.3 (if 'w').

    if (key == 'w')
    {
        px += pdx;
        py += pdy;
    }
    if (key == 's')
    {
        px -= pdx;
        py -= pdy;
    }
    if (key == 'd')
    {
        p_angle -= 0.1;
        if (p_angle < 0) p_angle += 2*PI;
        pdx = cos(p_angle)*speed;
        pdy = sin(p_angle)*speed;
    }
    if (key == 'a')
    {
        p_angle += 0.1;
        if (p_angle > 2*PI) p_angle -= 2*PI;
        pdx = cos(p_angle)*speed;
        pdy = sin(p_angle)*speed;
    }
    glutPostRedisplay();
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    drawMap2D();
    drawRays3D();
    drawPlayer();

    glutSwapBuffers();
}

void init()
{
    glClearColor(0.2,0.2,0.35,0);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);
    glMatrixMode(GL_MODELVIEW);

    px=300; py=300;
    p_angle = 2*PI;
    pdx = cos(p_angle)*speed;
    pdy = sin(p_angle)*speed;
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);

    glutCreateWindow("Raycaster Game");

    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(buttons);

    glutMainLoop();
    return 0;
}
