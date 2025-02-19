#include "raycaster.h"

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