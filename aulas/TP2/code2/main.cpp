#include <GL/glut.h>
#include <math.h>

float pyramidX = 0.0f;
float pyramidZ = 0.0f;
float rotationAngle = 0.0f;
float scale = 1.0f;
int drawingMode = GL_FILL;



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
	gluLookAt(5.0,5.0,5.0, 
		      0.0,0.0,0.0,
			  0.0f,1.0f,0.0f);

// put axis drawing in here
	glBegin(GL_LINES);
		// X axis in red
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(-100.0f, 0.0f, 0.0f);
		glVertex3f( 100.0f, 0.0f, 0.0f);
		// Y Axis in Green
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(0.0f, -100.0f, 0.0f);
		glVertex3f(0.0f, 100.0f, 0.0f);
		// Z Axis in Blue
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(0.0f, 0.0f, -100.0f);
		glVertex3f(0.0f, 0.0f, 100.0f);
	glEnd();

// put the geometric transformations here
	glTranslatef(pyramidX, 0.0f, 0.0f); // Translate in X
	glTranslatef(0.0f, 0.0f, pyramidZ); // Translate in Z
    glRotatef(rotationAngle, 0.0f, 1.0f, 0.0f); // Rotate around Y
	glScalef(scale, scale, scale); // Scale height
	glPolygonMode(GL_FRONT_AND_BACK, drawingMode); // Change drawing

// put pyramid drawing instructions here
	glBegin(GL_TRIANGLES);
		// Face 1 in Red
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex3f(1.0f, 0.0f, 1.0f);
        glVertex3f(1.0f, 0.0f, -1.0f);
        glVertex3f(0.0f, 2.0f, 0.0f);
        // Face 2 in Yellow
        glColor3f(1.0f, 1.0f, 0.0f);
        glVertex3f(-1.0f, 0.0f, 1.0f);
        glVertex3f(1.0f, 0.0f, 1.0f);
        glVertex3f(0.0f, 2.0f, 0.0f);
        // Face 3 in Blue
        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex3f(-1.0f, 0.0f, -1.0f);
        glVertex3f(-1.0f, 0.0f, 1.0f);
        glVertex3f(0.0f, 2.0f, 0.0f);
        // Face 4 in Green
        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex3f(1.0f, 0.0f, -1.0f);
        glVertex3f(-1.0f, 0.0f, -1.0f);
        glVertex3f(0.0f, 2.0f, 0.0f);
        // Face 5 in white
        glColor3f(1.0f, 1.0f, 1.0f);
        glVertex3f(-1.0f, 0.0f, -1.0f);
        glVertex3f(-1.0f, 0.0f, 1.0f);
        glVertex3f(1.0f, 0.0f, 1.0f);
        glVertex3f(-1.0f, 0.0f, -1.0f);
        glVertex3f(1.0f, 0.0f, 1.0f);
        glVertex3f(1.0f, 0.0f, -1.0f);
	glEnd();

	// End of frame
	glutSwapBuffers();
}



// write function to process keyboard events	
void processSpecialKeys(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_LEFT:
        pyramidX -= 0.1f; // Move left
        break;
    case GLUT_KEY_RIGHT:
        pyramidX += 0.1f; // Move right
        break;
    case GLUT_KEY_UP:
        pyramidZ += 0.1f; // Move up
        break;
    case GLUT_KEY_DOWN:
        pyramidZ -= 0.1f; // Move down
        break;
    case GLUT_KEY_PAGE_UP:
        rotationAngle += 5.0f; // Rotate clockwise
        break;
    case GLUT_KEY_PAGE_DOWN:
        rotationAngle -= 5.0f; // Rotate counterclockwise
        break;
    }
    glutPostRedisplay();
}



void processNormalKeys(unsigned char key, int x, int y) {
    switch (key) {
    case '+':
        scale += 0.1f; // Scale up
        break;
    case '-':
        scale -= 0.1f; // Scale down
        if (scale < 0.1f)
            scale = 0.1f;
        break;
	case '1':
        drawingMode = GL_FILL; // Fill
        break;
    case '2':
        drawingMode = GL_LINE; // Lines
        break;
    case '3':
        drawingMode = GL_POINT; // points
        break;
	case 27:
		exit(0);
    }
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

	
// put here the registration of the keyboard callbacks
	glutSpecialFunc(processSpecialKeys);
	glutKeyboardFunc(processNormalKeys);


//  OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	
// enter GLUT's main cycle
	glutMainLoop();
	
	return 1;
}
