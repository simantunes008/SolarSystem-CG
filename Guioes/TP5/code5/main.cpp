#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#define _USE_MATH_DEFINES
#include <math.h>

float alfa = 0.0f, beta = 0.5f, radius = 100.0f;
float camX, camY, camZ;

float cX = 0.0f, cZ = 0.0f, r = 50.0f, ri = 35.0f, rc = 15.0f;

float teapotRotation = 0.0f;

void spherical2Cartesian() {
	camX = radius * cos(beta) * sin(alfa);
	camY = radius * sin(beta);
	camZ = radius * cos(beta) * cos(alfa);
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



void renderScene(void) {

	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set the camera
	glLoadIdentity();
	gluLookAt(camX, camY, camZ,
		0.0, 0.0, 0.0,
		0.0f, 1.0f, 0.0f);

	glColor3f(0.2f, 0.8f, 0.2f);
	glBegin(GL_TRIANGLES);
		glVertex3f(100.0f, 0, -100.0f);
		glVertex3f(-100.0f, 0, -100.0f);
		glVertex3f(-100.0f, 0, 100.0f);

		glVertex3f(100.0f, 0, -100.0f);
		glVertex3f(-100.0f, 0, 100.0f);
		glVertex3f(100.0f, 0, 100.0f);
	glEnd();

	srand(123);

	for (int i = 0; i < 500; i++) {
		float x = ((float)rand() / RAND_MAX) * 200.0f - 100.0f;
        float z = ((float)rand() / RAND_MAX) * 200.0f - 100.0f; 

		if (sqrt((x - cX) * (x - cX) + (z - cZ) * (z - cZ)) > r) {
			float green = ((float)rand() / RAND_MAX) * 0.5f + 0.5f;

			glPushMatrix();
            glTranslatef(x, 2.5f, z);
			glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
            glColor3f(0.0f, green, 0.0f);
            glutSolidCone(2.5, 10.0, 10, 10);	
            glPopMatrix();

			glPushMatrix();
            glTranslatef(x, 0.0f, z);
			glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
            glColor3f(0.5f, 0.35f, 0.05f);	
            glutSolidCone(1.0, 5.0, 10, 10);	
            glPopMatrix();
		}
	}

	glColor3f(1.0f, 0.4f, 1.0f);
    glutSolidTorus(2.5, 5.0, 20, 20);

    for (int i = 0; i < 8; i++) {
        float angulo = i * (2 * M_PI / 8);

        glPushMatrix();
		glRotatef((angulo * 180) / M_PI, 0.0f, 1.0f, 0.0f);
		glRotatef(-teapotRotation, 0.0f, 1.0f, 0.0f);
        glTranslatef(rc, 2.0f, 0.0f);
		glColor3f(0.0f, 0.0f, 1.0f);
        glutSolidTeapot(2.0);
        glPopMatrix();
    }
	
	for (int i = 0; i < 16; i++) {
        float angulo = i * (2 * M_PI / 16);

        glPushMatrix();
		glRotatef((angulo * 180) / M_PI, 0.0f, 1.0f, 0.0f);
		glRotatef(teapotRotation, 0.0f, 1.0f, 0.0f);
        glTranslatef(0.0f, 2.0f, ri);
		glColor3f(1.0f, 0.0f, 0.0f);
        glutSolidTeapot(2.0);
        glPopMatrix();
    }
	
	glutSwapBuffers();
}


void processKeys(unsigned char c, int xx, int yy) {

// put code to process regular keys in here

}


void processSpecialKeys(int key, int xx, int yy) {

	switch (key) {

	case GLUT_KEY_RIGHT:
		alfa -= 0.1; break;

	case GLUT_KEY_LEFT:
		alfa += 0.1; break;

	case GLUT_KEY_UP:
		beta += 0.1f;
		if (beta > 1.5f)
			beta = 1.5f;
		break;

	case GLUT_KEY_DOWN:
		beta -= 0.1f;
		if (beta < -1.5f)
			beta = -1.5f;
		break;

	case GLUT_KEY_PAGE_DOWN: radius -= 1.0f;
		if (radius < 1.0f)
			radius = 1.0f;
		break;

	case GLUT_KEY_PAGE_UP: radius += 1.0f; break;
	}
	spherical2Cartesian();
	glutPostRedisplay();

}


void idle() {
    // Update the teapot rotation angle
    teapotRotation += 1.0f; // Adjust the rotation speed as desired
    glutPostRedisplay();
}


void printInfo() {

	printf("Vendor: %s\n", glGetString(GL_VENDOR));
	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("Version: %s\n", glGetString(GL_VERSION));

	printf("\nUse Arrows to move the camera up/down and left/right\n");
	printf("Home and End control the distance from the camera to the origin");
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

// Register idle function
    glutIdleFunc(idle);

// OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	spherical2Cartesian();

	printInfo();

// enter GLUT's main cycle
	glutMainLoop();
	
	return 1;
}
