#include <GL/glut.h>

#define _USE_MATH_DEFINES
#include <math.h>

float alpha = 0.0f, beta = 0.0f, radius = 10.0f;
float camX, camY, camZ;
float lastMouseX, lastMouseY;

void spherical2Cartesian() {
	camX = radius * cos(beta) * sin(alpha);
	camY = radius * sin(beta);
	camZ = radius * cos(beta) * cos(alpha);
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
	gluPerspective(45.0f ,ratio, 1.0f ,1000.0f);

	// return to the model view matrix mode
	glMatrixMode(GL_MODELVIEW);
}


void drawCylinder(float radius, float height, int slices) {
    glBegin(GL_TRIANGLES);
    	for (int i = 0; i < slices; i++) {
    	    float alpha1 = ((2.0 * M_PI) / slices) * i;
    	    float alpha2 = ((2.0 * M_PI) / slices) * (i + 1);

    	    // top
    	    glColor3f(1.0f, 0.0f, 0.0f);
    	    glVertex3f(0.0f, height / 2, 0.0f);
    	    glVertex3f(radius * sin(alpha1), height / 2, radius * cos(alpha1));
    	    glVertex3f(radius * sin(alpha2), height / 2, radius * cos(alpha2));

			// bottom
    	    glColor3f(1.0f, 0.0f, 0.0f);
    	    glVertex3f(0.0f, -height / 2, 0.0f);
			glVertex3f(radius * sin(alpha2), -height / 2, radius * cos(alpha2));
    	    glVertex3f(radius * sin(alpha1), -height / 2, radius * cos(alpha1));

    	    // body
    	    glColor3f(0.0f, 0.0f, 1.0f);
    	    glVertex3f(radius * sin(alpha1), height / 2, radius * cos(alpha1));
    	    glVertex3f(radius * sin(alpha1), -height / 2, radius * cos(alpha1));
    	    glVertex3f(radius * sin(alpha2), height / 2, radius * cos(alpha2));

    	    glColor3f(0.0f, 0.0f, 1.0f);
    	    glVertex3f(radius * sin(alpha1), -height / 2, radius * cos(alpha1));
    	    glVertex3f(radius * sin(alpha2), -height / 2, radius * cos(alpha2));
    	    glVertex3f(radius * sin(alpha2), height / 2, radius * cos(alpha2));
    	}
    glEnd();
}



void renderScene(void) {
	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set drawing mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// set the camera
	glLoadIdentity();
	spherical2Cartesian();
	gluLookAt(camX,camY,camZ, 
		      0.0,0.0,0.0,
			  0.0f,1.0f,0.0f);

	drawCylinder(2,4,20);

	// End of frame
	glutSwapBuffers();
}


void processKeys(unsigned char c, int xx, int yy) {
    switch (c) {

    case '+':
        radius += 0.1f;
        break;

    case '-':
        radius -= 0.1f;
        break;
    }

	spherical2Cartesian();

    glutPostRedisplay();
}


void processSpecialKeys(int key, int xx, int yy) {
	switch (key) {

	case GLUT_KEY_RIGHT:
		alpha += 0.1f;
		break;

	case GLUT_KEY_LEFT:
		alpha -= 0.1f;
		break;

	case GLUT_KEY_UP:
		beta += 0.1f;
		if (beta >= M_PI / 2) beta = M_PI / 2;
		break;

	case GLUT_KEY_DOWN:
		beta -= 0.1f;
		if (beta <= -M_PI / 2) beta = -M_PI / 2;
		break;
	}

	spherical2Cartesian();

	glutPostRedisplay();
}

void processMouse(int button, int state, int x, int y) {
    if (state == GLUT_DOWN) {
        lastMouseX = x;
        lastMouseY = y;
    }
}

void processMouseActiveMotion(int x, int y) {
    int deltaX = x - lastMouseX;
    int deltaY = y - lastMouseY;

    alpha -= deltaX * 0.01f;
    if (alpha >= 2 * M_PI) alpha -= 2 * M_PI;
    if (alpha < 0) alpha += 2 * M_PI;

    beta += deltaY * 0.01f;
    if (beta >= M_PI / 2) beta = M_PI / 2;
    if (beta <= -M_PI / 2) beta = -M_PI / 2;

    lastMouseX = x;
    lastMouseY = y;

    glutPostRedisplay();
}

int main(int argc, char **argv) {
// init GLUT and the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(800,800);
	glutCreateWindow("CG@DI-UM");
		
// Required callback registry 
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
	
// Callback registration for keyboard processing
	glutKeyboardFunc(processKeys);
	glutSpecialFunc(processSpecialKeys);
	glutMouseFunc(processMouse);
    glutMotionFunc(processMouseActiveMotion);

//  OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	
// enter GLUT's main cycle
	glutMainLoop();
	
	return 1;
}	
