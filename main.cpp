#include <GL/glut.h>
#include <cmath>

// ---------- Global State ----------
// Train
float trainSpeed   = 0.02f;   // how fast train moves
float trainHeadX   = -6.0f;   // x-position of engine front

// Background scrolling
float worldOffset  = 0.0f;    // how much city has scrolled
const float citySectionWidth = 8.0f; // width of one repeating city block

// Day / Night
float skyBlend        = 1.0f; // 1 = day, 0 = night
float targetSkyBlend  = 1.0f; // where we want to go (day or night)

// ---------- Helpers ----------
void drawCircle(float cx, float cy, float r, int segments = 40) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= segments; ++i) {
        float ang = 2.0f * 3.14159f * i / segments;
        glVertex2f(cx + r * cosf(ang), cy + r * sinf(ang));
    }
    glEnd();
}

// ---------- Background: Sky, Day/Night, Stars, Moon ----------
void drawSky() {
    // Day & night colors
    float dayR = 0.45f, dayG = 0.75f, dayB = 1.0f;
    float nightR = 0.02f, nightG = 0.02f, nightB = 0.08f;

    float r = nightR + skyBlend * (dayR - nightR);
    float g = nightG + skyBlend * (dayG - nightG);
    float b = nightB + skyBlend * (dayB - nightB);

    glBegin(GL_QUADS);
    glColor3f(r, g, b);
    glVertex2f(-4.0f,  2.0f);
    glVertex2f( 4.0f,  2.0f);
    glVertex2f( 4.0f, -0.2f);
    glVertex2f(-4.0f, -0.2f);
    glEnd();

    // Sun (day)
    if (skyBlend > 0.2f) {
        glColor3f(1.0f, 0.95f, 0.3f);
        drawCircle(2.8f, 1.4f, 0.25f);
    }

    // Moon + stars (night)
    if (skyBlend < 0.8f) {
        float alpha = 1.0f - skyBlend;

        // Moon
        glColor3f(0.9f * alpha, 0.9f * alpha, 1.0f * alpha);
        drawCircle(-2.8f, 1.4f, 0.22f);

        // Stars
        glPointSize(3.0f);
        glBegin(GL_POINTS);
        glColor3f(0.9f * alpha, 0.9f * alpha, 1.0f * alpha);
        glVertex2f(-3.0f, 1.8f);
        glVertex2f(-2.2f, 1.6f);
        glVertex2f(-1.0f, 1.7f);
        glVertex2f( 0.2f, 1.5f);
        glVertex2f( 1.1f, 1.9f);
        glVertex2f( 2.5f, 1.6f);
        glVertex2f( 3.3f, 1.8f);
        glEnd();
    }
}

// ---------- Ground & Track ----------
void drawGround() {
    glColor3f(0.25f, 0.7f, 0.3f);
    glBegin(GL_QUADS);
    glVertex2f(-4.0f, -2.0f);
    glVertex2f( 4.0f, -2.0f);
    glVertex2f( 4.0f, -0.2f);
    glVertex2f(-4.0f, -0.2f);
    glEnd();
}

void drawTrack() {
    // Base
    glColor3f(0.35f, 0.35f, 0.35f);
    glBegin(GL_QUADS);
    glVertex2f(-4.0f, -0.8f);
    glVertex2f( 4.0f, -0.8f);
    glVertex2f( 4.0f, -0.6f);
    glVertex2f(-4.0f, -0.6f);
    glEnd();

    // Rails
    glColor3f(0.8f, 0.8f, 0.8f);
    glBegin(GL_LINES);
    glVertex2f(-4.0f, -0.62f);
    glVertex2f( 4.0f, -0.62f);
    glVertex2f(-4.0f, -0.78f);
    glVertex2f( 4.0f, -0.78f);
    glEnd();

    // Sleepers
    for (float x = -4.0f; x <= 4.0f; x += 0.5f) {
        glColor3f(0.5f, 0.3f, 0.1f);
        glBegin(GL_QUADS);
        glVertex2f(x - 0.08f, -0.8f);
        glVertex2f(x + 0.08f, -0.8f);
        glVertex2f(x + 0.08f, -0.9f);
        glVertex2f(x - 0.08f, -0.9f);
        glEnd();
    }
}

// ---------- One City Section (Buildings + Restaurant + Trees) ----------
void drawCitySection() {
    // Window color blends with day/night
    float dayR = 0.85f, dayG = 0.95f, dayB = 1.0f;
    float nightR = 1.0f, nightG = 0.9f, nightB = 0.4f;

    float wr = nightR + skyBlend * (dayR - nightR);
    float wg = nightG + skyBlend * (dayG - nightG);
    float wb = nightB + skyBlend * (dayB - nightB);

    // 4 tall buildings
    for (int i = 0; i < 4; ++i) {
        float bx = -3.6f + i * 2.0f;
        glColor3f(0.45f + 0.1f * (i % 2),
                  0.45f,
                  0.5f + 0.05f * (i % 3));
        glBegin(GL_QUADS);
        glVertex2f(bx,   -0.2f);
        glVertex2f(bx+1.0f, -0.2f);
        glVertex2f(bx+1.0f,  1.3f + 0.3f*(i%2));
        glVertex2f(bx,    1.3f + 0.3f*(i%2));
        glEnd();

        for (float wy = 0.0f; wy <= 1.1f; wy += 0.3f) {
            for (float wx = bx + 0.15f; wx <= bx + 0.75f; wx += 0.25f) {
                glColor3f(wr, wg, wb);
                glBegin(GL_QUADS);
                glVertex2f(wx,         -0.05f + wy);
                glVertex2f(wx + 0.18f, -0.05f + wy);
                glVertex2f(wx + 0.18f,  0.15f + wy);
                glVertex2f(wx,          0.15f + wy);
                glEnd();
            }
        }
    }

    // Restaurant
    glColor3f(0.8f, 0.25f, 0.15f);
    glBegin(GL_QUADS);
    glVertex2f(-3.0f, -0.2f);
    glVertex2f(-1.7f, -0.2f);
    glVertex2f(-1.7f, 0.4f);
    glVertex2f(-3.0f, 0.4f);
    glEnd();

    // Board
    glColor3f(0.1f, 0.1f, 0.1f);
    glBegin(GL_QUADS);
    glVertex2f(-2.9f, 0.35f);
    glVertex2f(-1.8f, 0.35f);
    glVertex2f(-1.8f, 0.55f);
    glVertex2f(-2.9f, 0.55f);
    glEnd();

    glColor3f(1.0f, 1.0f, 0.0f);
    glRasterPos2f(-2.8f, 0.4f);
    const char* rest = "RESTAURANT";
    for (int i = 0; rest[i] != '\0'; ++i)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, rest[i]);

    // Trees
    for (float x = -1.2f; x <= 3.6f; x += 1.2f) {
        glColor3f(0.45f, 0.25f, 0.1f);
        glBegin(GL_QUADS);
        glVertex2f(x-0.05f, -0.2f);
        glVertex2f(x+0.05f, -0.2f);
        glVertex2f(x+0.05f,  0.2f);
        glVertex2f(x-0.05f,  0.2f);
        glEnd();

        glColor3f(0.1f, 0.6f, 0.1f);
        drawCircle(x, 0.35f, 0.25f);
    }
}

// Draw 3 city sections so scroll looks continuous
void drawCity() {
    for (int i = -1; i <= 1; ++i) {
        glPushMatrix();
        glTranslatef(worldOffset + i * citySectionWidth, 0.0f, 0.0f);
        drawCitySection();
        glPopMatrix();
    }
}

// ---------- Train ----------
void drawCoach(float x) {
    glPushMatrix();
    glTranslatef(x, -0.9f, 0.0f);

    glColor3f(0.2f, 0.2f, 0.8f);
    glBegin(GL_QUADS);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(0.9f, 0.0f);
    glVertex2f(0.9f, 0.4f);
    glVertex2f(0.0f, 0.4f);
    glEnd();

    glColor3f(0.9f, 0.95f, 1.0f);
    for (float wx = 0.08f; wx <= 0.7f; wx += 0.18f) {
        glBegin(GL_QUADS);
        glVertex2f(wx,        0.15f);
        glVertex2f(wx + 0.12f,0.15f);
        glVertex2f(wx + 0.12f,0.3f);
        glVertex2f(wx,        0.3f);
        glEnd();
    }

    glColor3f(0.0f, 0.0f, 0.0f);
    drawCircle(0.2f, -0.08f, 0.07f);
    drawCircle(0.7f, -0.08f, 0.07f);

    glPopMatrix();
}

void drawEngine(float x) {
    glPushMatrix();
    glTranslatef(x, -0.9f, 0.0f);

    glColor3f(0.8f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(1.0f, 0.0f);
    glVertex2f(1.0f, 0.5f);
    glVertex2f(0.0f, 0.5f);
    glEnd();

    glColor3f(0.1f, 0.1f, 0.4f);
    glBegin(GL_QUADS);
    glVertex2f(0.6f, 0.5f);
    glVertex2f(1.0f, 0.5f);
    glVertex2f(1.0f, 0.8f);
    glVertex2f(0.6f, 0.8f);
    glEnd();

    glColor3f(0.8f, 0.95f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(0.7f, 0.58f);
    glVertex2f(0.95f,0.58f);
    glVertex2f(0.95f,0.75f);
    glVertex2f(0.7f, 0.75f);
    glEnd();

    glColor3f(0.0f, 0.0f, 0.0f);
    drawCircle(0.25f, -0.1f, 0.08f);
    drawCircle(0.75f, -0.1f, 0.08f);

    glPopMatrix();
}

void drawTrain() {
    float gap = 0.95f;

    // 8 coaches behind engine
    for (int i = 0; i < 8; ++i) {
        float coachX = trainHeadX - (i + 1) * gap;
        drawCoach(coachX);
    }

    // Engine
    drawEngine(trainHeadX);
}

// ---------- Display ----------
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    drawSky();
    drawGround();
    drawCity();
    drawTrack();
    drawTrain();

    glutSwapBuffers();
}

// ---------- Timer (animation) ----------
void timer(int) {
    // Move train
    trainHeadX += trainSpeed;
    if (trainHeadX > 6.0f)
        trainHeadX = -6.0f;

    // Move city in opposite direction
    worldOffset -= trainSpeed * 0.8f;   // 0.8 for nice relative speed
    if (worldOffset < -citySectionWidth)
        worldOffset += citySectionWidth;
    if (worldOffset >  citySectionWidth)
        worldOffset -= citySectionWidth;

    // Smooth day/night blending
    if (skyBlend < targetSkyBlend) {
        skyBlend += 0.01f;
        if (skyBlend > targetSkyBlend) skyBlend = targetSkyBlend;
    } else if (skyBlend > targetSkyBlend) {
        skyBlend -= 0.01f;
        if (skyBlend < targetSkyBlend) skyBlend = targetSkyBlend;
    }

    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

// ---------- Keyboard ----------
// W / w : speed up
// S / s : slow down
// D / d : Day mode
// N / n : Night mode
// ESC   : Exit
void keyboard(unsigned char key, int, int) {
    switch (key) {
    case 'w':
    case 'W':
        trainSpeed += 0.005f;
        break;
    case 's':
    case 'S':
        trainSpeed -= 0.005f;
        if (trainSpeed < 0.0f) trainSpeed = 0.0f;
        break;
    case 'd':
    case 'D':
        targetSkyBlend = 1.0f;
        break;
    case 'n':
    case 'N':
        targetSkyBlend = 0.0f;
        break;
    case 27: // ESC
        exit(0);
    }
}

// ---------- main & WinMain ----------
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(1200, 600);
    glutInitWindowPosition(50, 50);
    glutCreateWindow("Running Train with Moving City & Day/Night");

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    gluOrtho2D(-4.0, 4.0, -2.0, 2.0);

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(0, timer, 0);

    glutMainLoop();
    return 0;
}

#ifdef _WIN32
#include <windows.h>
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int) {
    return main(__argc, __argv);
}
#endif
