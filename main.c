#include "raycaster.h"

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

    glutCreateWindow("Raycaster");

    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(buttons);

    glutMainLoop();
    return 0;
}
