#define _USE_MATH_DEFINES
#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include "tinyxml2.h"
#include "catmull-rom.h"
#include "parser.h"

#define CAMERA_SPEED 0.01f
#define ZOOM_SPEED 0.5f

// 
World world;
int frame=0, time_=0, timebase=0;
vector<float> vertexVBO, normalVBO, textureVBO;
GLuint buffers[3];

//
bool axes = false;
bool lines = false;
bool paused = false;
bool curves = false;
bool helpMenu = false;

// Camera controls
bool lButton = false;
bool rButton = false;
float alpha, betA, radius;
float lastMouseX, lastMouseY;


void calcFPS() {
    char s[100];

    frame++;
    time_=glutGet(GLUT_ELAPSED_TIME);

    if (time_ - timebase > 1000) {
        sprintf(s,"Engine: FPS:%4.2f", frame*1000.0/(time_ - timebase));
        timebase = time_;
        frame = 0;
        glutSetWindowTitle(s);
    }
}


void spherical2Cartesian() {
    world.cam->position[0] = radius * cos(betA) * sin(alpha);
    world.cam->position[1] = radius * sin(betA);
    world.cam->position[2] = radius * cos(betA) * cos(alpha);
}


void setupCamera() {
    float dirX = world.cam->lookAt[0] - world.cam->position[0];
    float dirY = world.cam->lookAt[1] - world.cam->position[1];
    float dirZ = world.cam->lookAt[2] - world.cam->position[2];

    radius = sqrt(pow(dirX, 2) + pow(dirY, 2) + pow(dirZ, 2));
    alpha = asin(world.cam->position[0] / sqrt(pow(world.cam->position[2], 2) + pow(world.cam->position[0], 2)));
    betA = asin(world.cam->position[1] / radius);
}


void changeSize(int w, int h) {
    // Prevent a divide by zero, when window is too short
    // (you cant make a window with zero width).
    if(h == 0)
        h = 1;

    // compute window's aspect ratio
    float ratio = w * 1.0 / h;

    // Set the projection matrix as current
    glMatrixMode(GL_PROJECTION);
    // Load Identity Matrix
    glLoadIdentity();

    // Set the viewport to be the entire window
    glViewport(0, 0, w, h);

    // Set perspective
    gluPerspective(world.cam->projection[0],ratio,world.cam->projection[1],world.cam->projection[2]);

    // return to the model view matrix mode
    glMatrixMode(GL_MODELVIEW);
}


void animatedTranslate(Transformation t) {
    float yBefore[3] = { 0,1,0 };
    float turnCount = 0;

    float pos[3], deriv[3], direction[3], rotMatrix[16];

    if (curves) {
        glDisable(GL_LIGHTING);
        renderCatmullRomCurve(t.points);
        glEnable(GL_LIGHTING);    
    }

    float elapsedTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
    float actualTime = elapsedTime - t.time * turnCount;

    if (actualTime > t.time) {
        turnCount++;
        actualTime = elapsedTime - t.time*turnCount;
    }

    float gt = actualTime*(!paused)/t.time;

    getGlobalCatmullRomPoint(gt, pos, deriv, t.points);

    if (t.align) {
        normalize(deriv);
        cross(deriv, yBefore, direction);
        normalize(direction);
        float newY[3] {0,0,0};
        cross(direction, deriv, newY);
        normalize(newY);

        buildRotMatrix(deriv, newY, direction, rotMatrix);

        glTranslatef(pos[0], pos[1], pos[2]);
        glMultMatrixf(rotMatrix);
    } else {
        glTranslatef(pos[0], pos[1], pos[2]);
    }
}


void animatedRotate(Transformation t){
    float elapsedTime = glutGet(GLUT_ELAPSED_TIME);
    float angle = 360/(t.time * 1000);

    if (!paused) {
        glRotatef(elapsedTime * angle, t.x, t.y, t.z);
    }
}


void drawGroups(vector<Group> groups) {
    for(const Group& g : groups){
        glColor3f(1.0, 1.0, 1.0);
        glPushMatrix();
        for (const auto& t : g.transformations) {
            if (t.name=="translate") {
                if (t.time!=0) animatedTranslate(t);
                else glTranslatef(t.x, t.y, t.z);
            }

            if (t.name=="scale") {
                glScalef(t.x, t.y, t.z);
            }

            if (t.name=="rotate") {
                if (t.time!=-1) animatedRotate(t);
                else glRotatef(t.angle,t.x, t.y, t.z);
            }
        }

        for (Model m : g.models) {
            glMaterialfv(GL_FRONT, GL_DIFFUSE, m.color.diffuse);
            glMaterialfv(GL_FRONT, GL_AMBIENT, m.color.ambient);
            glMaterialfv(GL_FRONT, GL_SPECULAR, m.color.specular);
            glMaterialfv(GL_FRONT, GL_EMISSION, m.color.emissive);
            glMaterialf(GL_FRONT, GL_SHININESS, m.color.shininess);

            if (m.textureID != 0) {
                glBindTexture(GL_TEXTURE_2D, m.textureID);

                glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
                glVertexPointer(3, GL_FLOAT, 0, 0);

                glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
                glNormalPointer(GL_FLOAT, 0, 0);

                glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
                glTexCoordPointer(2, GL_FLOAT, 0, 0);

                glDrawArrays(GL_TRIANGLES, m.indexBegin, m.vertexCount);

                glBindTexture(GL_TEXTURE_2D, 0);
            } else {
                glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
                glVertexPointer(3, GL_FLOAT, 0, 0);

                glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
                glNormalPointer(GL_FLOAT, 0, 0);

                glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
                glTexCoordPointer(2, GL_FLOAT, 0, 0);

                glDrawArrays(GL_TRIANGLES, m.indexBegin, m.vertexCount);
            }
        }

        drawGroups(g.subGroups);

        glPopMatrix();
    }
}


void drawAxes() {
    glDisable(GL_LIGHTING);
    glBegin(GL_LINES);
	//  X axis in red
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(-100.0f, 0.0f, 0.0f);
		glVertex3f( 100.0f, 0.0f, 0.0f);
	//  Y Axis in Green
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(0.0f, -100.0f, 0.0f);
		glVertex3f(0.0f, 100.0f, 0.0f);
	//  Z Axis in Blue
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(0.0f, 0.0f, -100.0f);
		glVertex3f(0.0f, 0.0f, 100.0f);
	glEnd();
    glEnable(GL_LIGHTING);
}


vector<string> helpText(){
    vector<string> lines;
    lines.emplace_back("___________________________________________________________________________");
    lines.emplace_back("CAMERA CONTROLS"                                                            );
    lines.emplace_back("- Move the camera by dragging the mouse while holding down the left button.");
    lines.emplace_back("- Zoom in/out by dragging the mouse while holding down the right button."   );
    lines.emplace_back("___________________________________________________________________________");
    lines.emplace_back(""                                                                           );
    lines.emplace_back("___________________________________________________________________________");
    lines.emplace_back("OTHER CONTROLS"                                                             );
    lines.emplace_back("- Press 'a' to toggle the display of axes."                                 );
    lines.emplace_back("- Press 'l' to toggle the display of lines."                                );
    lines.emplace_back("- Press 'c' to toggle the display of curves."                               );
    lines.emplace_back("- Press 'p' to pause/unpause the scene."                                    );
    lines.emplace_back("___________________________________________________________________________");
    return lines;
}


void showHelpMenu(){
    int size, y = 50, x = -15;
    char* text;
    vector<string> lines = helpText();

    glRasterPos2f(x, y) ;
    y-=4;

    glDisable(GL_LIGHTING);
    glColor3f(1, 1, 1);
    for(string line : lines) {
        text = &line[0];
        size = line.size();
        glRasterPos2f(x, y);

        for (int i = 0; i < size; i++) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, text[i]);

        y -= 4;
    }
    glEnable(GL_LIGHTING);
}


void renderScene(void) {
    int i = 0;

	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set drawing mode
	if (lines) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // draw axes
    if (axes) drawAxes();

	glLoadIdentity();
    if (helpMenu) {
    //  set the camera
        gluLookAt(0.0f, 0.0f, 100.0f,
                  0.0f, 0.0f, 0.0f,
                  0.0f, 1.0f, 0.0f);

        showHelpMenu();
    } else {
    //  set the camera
    	gluLookAt(world.cam->position[0], world.cam->position[1], world.cam->position[2], 
    		      world.cam->lookAt[0],   world.cam->lookAt[1],   world.cam->lookAt[2],
    			  world.cam->up[0],       world.cam->up[1],       world.cam->up[2]);

        for (Light l : world.lights) {

            float amb[4]  = {0.2f, 0.2f, 0.2f, 1.0f};
            float diff[4] = {1.0f, 1.0f, 1.0f, 1.0f};
            float spec[4] = {1.0f, 1.0f, 1.0f, 1.0f};

            glLightfv(GL_LIGHT0 + i, GL_AMBIENT, amb);
            glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, diff);
            glLightfv(GL_LIGHT0 + i, GL_SPECULAR, spec);

            if (l.type == "point") {
                glLightfv(GL_LIGHT0 + i, GL_POSITION, l.position);
            }

            if (l.type == "directional") {
                glLightfv(GL_LIGHT0 + i, GL_POSITION, l.direction);
            }

            if (l.type == "spot") {
                glLightfv(GL_LIGHT0 + i, GL_POSITION, l.position);
                glLightfv(GL_LIGHT0 + i, GL_SPOT_DIRECTION, l.direction);
                glLightf(GL_LIGHT0 + i, GL_SPOT_CUTOFF, l.cutoff);
            }

            i++;
        }

        drawGroups(world.groups);

        calcFPS();
    }

	// End of frame
	glutSwapBuffers();
}


void processKeys(unsigned char c, int xx, int yy) {
    switch (c) {

    case 'l':
        if (lines) lines = false;
        else lines = true;
        break;

    case 'a':
        if (axes) axes = false;
        else axes = true;
        break;
    
    case 'p':
        if (paused) paused = false;
        else paused = true;
        break;
    
    case 'c':
        if (curves) curves = false;
        else curves = true;
        break;

    case 'h':
        if (helpMenu) helpMenu = false;
        else helpMenu = true;
        break;

    }
}


void processMouse(int button, int state, int x, int y) {
    if (state == GLUT_DOWN) {
        lastMouseX = x;
        lastMouseY = y;
        if (button == GLUT_LEFT_BUTTON) {
            // Botão esquerdo pressionado: movimento da câmera
            lButton = true;
        }
        else if (button == GLUT_RIGHT_BUTTON) {
            // Botão direito pressionado: alteração de zoom
            rButton = true;
        }
    } else if (state == GLUT_UP) {
        lButton = false;
        rButton = false;
    }
}


void processMouseActiveMotion(int x, int y) {
    int deltaX = x - lastMouseX;
    int deltaY = y - lastMouseY;

    if (lButton) {
        alpha -= deltaX * CAMERA_SPEED;
        if (alpha >= 2 * M_PI) alpha -= 2 * M_PI;
        if (alpha < 0) alpha += 2 * M_PI;

        betA += deltaY * CAMERA_SPEED;
    //  Impede de passar os 90 graus
        if (betA >= M_PI / 2) betA = M_PI / 2 - 0.01;
        if (betA <= -M_PI / 2) betA = -M_PI / 2 + 0.01;

        spherical2Cartesian();
    } else if (rButton) {
        radius -= deltaY * ZOOM_SPEED;
        spherical2Cartesian();
    }

    lastMouseX = x;
    lastMouseY = y;
}


void initGL(char * str) {
    parseXML(str, &world, &vertexVBO, &normalVBO, &textureVBO);
    glutReshapeWindow(world.win->width,world.win->height);

//  OpenGL settings
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

//  init
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

//  light
    glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_LIGHT2);
    glEnable(GL_LIGHT3);
    glEnable(GL_LIGHT4);
    glEnable(GL_LIGHT5);
    glEnable(GL_LIGHT6);
    glEnable(GL_LIGHT7);
    glEnable(GL_RESCALE_NORMAL);
    glEnable(GL_TEXTURE_2D);

	float amb[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, amb);

// 	Build the vertex, normal and texture arrays
    glGenBuffers(3, buffers);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexVBO.size(), vertexVBO.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * normalVBO.size(), normalVBO.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * textureVBO.size(), textureVBO.data(), GL_STATIC_DRAW);
}


int main(int argc, char** argv) {

//  init GLUT and the window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
    glutInitWindowPosition(100,100);
    glutInitWindowSize(320,320);
    glutCreateWindow("Engine: FPS:");

//  Required callback registry
    glutDisplayFunc(renderScene);
    glutReshapeFunc(changeSize);
    glutIdleFunc(renderScene);

//  Callback registration for keyboard processing
    glutKeyboardFunc(processKeys);
    glutMouseFunc(processMouse);
    glutMotionFunc(processMouseActiveMotion);

//  init GLEW
    glewInit();

    initGL(argv[1]);
    setupCamera();

//  enter GLUT's main cycle
    glutMainLoop();
    time_ = glutGet(GLUT_ELAPSED_TIME);
}
