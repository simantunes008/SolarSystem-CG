#include<stdio.h>
#include<stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include <IL/il.h>
#include <GL/glew.h>
#include <GL/glut.h>

using std::vector;


float camX = 0.0f, camY = 30.0f, camZ = 40.0f;
float alpha = M_PI, beta = 0.0f;
float startX, startY;

float verticalSpeed = 0.005f;
float horizontalSpeed = 0.01f;
float movementSpeed = 0.5f;	

float cX = 0.0f, cZ = 0.0f, r = 50.0f, ri = 35.0f, rc = 15.0f;
float teapotRotation = 0.0f;

unsigned int t, tw, th;
unsigned char *imageData;

GLuint buffers[1];

void changeSize(int w, int h) {

	// Prevent a divide by zero, when window is too short
	// (you cant make a window with zero width).
	if(h == 0)
		h = 1;

	// compute window's aspect ratio 
	float ratio = w * 1.0 / h;

	// Reset the coordinate system before modifying
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	// Set the viewport to be the entire window
    glViewport(0, 0, w, h);

	// Set the correct perspective
	gluPerspective(45,ratio,1,1000);

	// return to the model view matrix mode
	glMatrixMode(GL_MODELVIEW);
}



float h(int x, int z) {
	return imageData[x + z * tw] / 8.5f;
}



float hf(float x, float z) {
	
	float offsetW = (float)tw / 2;
	float offsetH = (float)th / 2;

	x += offsetW;
	z += offsetH;

	int x1 = floor(x); 
	int z1 = floor(z); 
	int x2 = x1 + 1;
	int z2 = z1 + 1;

	float fx = x - x1;
	float fz = z - z1;

	float h_x1_z = h(x1, z1) * (1 - fz) + h(x1, z2) * fz;
    float h_x2_z = h(x2, z1) * (1 - fz) + h(x2, z2) * fz;

	float height_xz = h_x1_z * (1 - fx) + h_x2_z * fx;

	return height_xz;
}



void drawTerrain() {

    // colocar aqui o código de desenho do terreno usando VBOs com TRIANGLE_STRIPS
	float offsetW = (float)tw / 2;
	float offsetH = (float)th / 2;

	glPushMatrix();
	glTranslatef(-offsetW, 0.0f, -offsetH);

	glColor3f(0.2f, 0.8f, 0.2f);
	for (int i = 0; i < th - 1; i++) {
        glBegin(GL_TRIANGLE_STRIP);
        for (int j = 0; j < tw; j++) {
            glVertex3f(j, h(j, i), i);
            glVertex3f(j, h(j, i + 1), i + 1);
        }
        glEnd();
    }

	// glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	// glVertexPointer(3, GL_FLOAT, 0, 0);
	// 
	// for (int i = 0; i < th - 1 ; i++) {
		// glDrawArrays(GL_TRIANGLE_STRIP, (tw) * 2 * i, (tw) * 2);
	// }

	glPopMatrix();
}



void renderScene(void) {

	float pos[4] = {-1.0, 1.0, 1.0, 0.0};

	glClearColor(0.0f,0.0f,0.0f,0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	gluLookAt(camX, hf(camX, camZ) + 5.0f, camZ, 
			  camX + sin(alpha), hf(camX, camZ) + 5.0f + sin(beta), camZ + cos(alpha), 
			  0, 1, 0);
	
	drawTerrain();

	// just so that it renders something before the terrain is built
	// to erase when the terrain is ready
	// glutWireTeapot(2.0);

	glColor3f(1.0f, 0.4f, 1.0f);
    glutSolidTorus(2.5, 5.0, 20, 20);

	srand(123);

	for (int i = 0; i < 500; i++) {
		float x = ((float)rand() / RAND_MAX) * 200.0f - 100.0f;
        float z = ((float)rand() / RAND_MAX) * 200.0f - 100.0f; 

		if (sqrt((x - cX) * (x - cX) + (z - cZ) * (z - cZ)) > r) {
			float green = ((float)rand() / RAND_MAX) * 0.5f + 0.5f;

			glPushMatrix();
            glTranslatef(x, hf(x, z) + 2.0f, z);
			glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
            glColor3f(0.0f, green, 0.0f);
            glutSolidCone(2.5, 10.0, 10, 10);	
            glPopMatrix();

			glPushMatrix();
            glTranslatef(x, hf(x, z) - 0.5f, z);
			glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
            glColor3f(0.5f, 0.35f, 0.05f);	
            glutSolidCone(1.0, 5.0, 10, 10);	
            glPopMatrix();
		}
	}

	for (int i = 0; i < 8; i++) {
        float angulo = i * (2 * M_PI / 8);

        glPushMatrix();
		glRotatef((angulo * 180) / M_PI, 0.0f, 1.0f, 0.0f);
		glRotatef(-teapotRotation, 0.0f, 1.0f, 0.0f);
        glTranslatef(rc, hf(rc, 0.0) + 2.0f, 0.0f);
		glColor3f(0.0f, 0.0f, 1.0f);
        glutSolidTeapot(2.0);
        glPopMatrix();
    }
	
	for (int i = 0; i < 16; i++) {
        float angulo = i * (2 * M_PI / 16);

        glPushMatrix();
		glRotatef((angulo * 180) / M_PI, 0.0f, 1.0f, 0.0f);
		glRotatef(teapotRotation, 0.0f, 1.0f, 0.0f);
        glTranslatef(0.0f, hf(0.0, ri) + 2.0f, ri);
		glColor3f(1.0f, 0.0f, 0.0f);
        glutSolidTeapot(2.0);
        glPopMatrix();
    }

	teapotRotation += 1.0f;

// End of frame
	glutSwapBuffers();
}



void processKeys(unsigned char key, int xx, int yy) {

// put code to process regular keys in here
    switch (key) {
        case 'w':
        case 'W':
            camX += sin(alpha) * movementSpeed;
            camZ += cos(alpha) * movementSpeed;
            break;
        case 's':
        case 'S':
            camX -= sin(alpha) * movementSpeed;
            camZ -= cos(alpha) * movementSpeed;
            break;
        case 'a':
        case 'A':
            camX += cos(alpha) * movementSpeed;
            camZ -= sin(alpha) * movementSpeed;
            break;
        case 'd':
        case 'D':
            camX -= cos(alpha) * movementSpeed;
            camZ += sin(alpha) * movementSpeed;
            break;
    }

}



void processMouseButtons(int button, int state, int xx, int yy) {
	
	if (state == GLUT_DOWN)  {
		startX = xx;
		startY = yy;
	}
}


void processMouseMotion(int xx, int yy) {

	float deltaX = xx - startX;
    float deltaY = yy - startY;

// Update alpha and beta angles based on mouse movement
    alpha -= deltaX * horizontalSpeed;
    beta -= deltaY * verticalSpeed;

// Limit beta angle to avoid flipping upside down
    if (beta > 85)
        beta = 85;
    else if (beta < -85)
        beta = -85;

// Update start position for next motion event
    startX = xx;
    startY = yy;

}


void init() {
	ilInit();
	glewInit();

// 	Load the height map "terreno.jpg"
	ilGenImages(1,&t);
	ilBindImage(t);
	// terreno.jpg is the image containing our height map
	ilLoadImage((ILstring)"terreno.jpg");
	// convert the image to single channel per pixel
	// with values ranging between 0 and 255
	ilConvertImage(IL_LUMINANCE, IL_UNSIGNED_BYTE);
	// important: check tw and th values
	// both should be equal to 256
	// if not there was an error loading the image
	// most likely the image could not be found
	tw = ilGetInteger(IL_IMAGE_WIDTH);
	th = ilGetInteger(IL_IMAGE_HEIGHT);
	// imageData is a LINEAR array with the pixel values
	imageData = ilGetData();

// 	Build the vertex arrays
	// glEnableClientState(GL_VERTEX_ARRAY);
	// 
	// vector<float> vertexB;
	// 
	// for (int i = 0; i < th - 1; i++) {
        // for (int j = 0; j < tw; j++) {
			// vertexB.push_back(j);
			// vertexB.push_back(h(j, i));
			// vertexB.push_back(i);
			// 
			// vertexB.push_back(j);
			// vertexB.push_back(h(j, i + 1));
			// vertexB.push_back(i + 1);
        // }
    // }
	// 
	// glGenBuffers(1, buffers);
	// glBindBuffer(GL_ARRAY_BUFFER,buffers[0]);
	// glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexB.size(), vertexB.data(), GL_STATIC_DRAW);

// 	OpenGL settings
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
}


int main(int argc, char **argv) {

// init GLUT and the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(320,320);
	glutCreateWindow("CG@DI-UM");
		

// Required callback registry 
	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);
	glutReshapeFunc(changeSize);

// Callback registration for keyboard processing
	glutKeyboardFunc(processKeys);
	glutMouseFunc(processMouseButtons);
	glutMotionFunc(processMouseMotion);

	init();	

// enter GLUT's main cycle
	glutMainLoop();
	
	return 0;
}

