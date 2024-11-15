#include <stdio.h>
#include <vector>
#include <GL/glew.h>
#include <GL/glut.h>

#define _USE_MATH_DEFINES
#include <math.h>

using std::vector;

GLuint buffers[3], vertices[3];

int time = 0;
int timebase = 0;	
int frame = 0;
float frames = 0;

float alfa = 0.0f, betA = 0.0f, radius = 5.0f;
float camX, camY, camZ;


void spherical2Cartesian() {
	camX = radius * cos(betA) * sin(alfa);
	camY = radius * sin(betA);
	camZ = radius * cos(betA) * cos(alfa);
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


/*-----------------------------------------------------------------------------------
	Draw Cylinder

		parameters: radius, height, sides

-----------------------------------------------------------------------------------*/

void cylinder(float radius, float height, int sides) {

	int i;
	float step;

	step = 360.0 / sides;

	glBegin(GL_TRIANGLES);

	// top
	for (i = 0; i < sides; i++) {
		glVertex3f(0, height*0.5, 0);
		glVertex3f(cos(i * step * M_PI / 180.0)*radius, height*0.5, -sin(i * step *M_PI / 180.0)*radius);
		glVertex3f(cos((i+1) * step * M_PI / 180.0)*radius, height*0.5, -sin((i + 1) * step *M_PI / 180.0)*radius);
	}

	// bottom
	for (i = 0; i < sides; i++) {
		glVertex3f(0, -height*0.5, 0);
		glVertex3f(cos((i + 1) * step * M_PI / 180.0)*radius, -height*0.5, -sin((i + 1) * step *M_PI / 180.0)*radius);
		glVertex3f(cos(i * step * M_PI / 180.0)*radius, -height*0.5, -sin(i * step *M_PI / 180.0)*radius);
	}

	// body
	for (i = 0; i <= sides; i++) {
		glVertex3f(cos(i * step * M_PI / 180.0)*radius, height*0.5, -sin(i * step *M_PI / 180.0)*radius);
		glVertex3f(cos(i * step * M_PI / 180.0)*radius, -height*0.5, -sin(i * step *M_PI / 180.0)*radius);
		glVertex3f(cos((i + 1) * step * M_PI / 180.0)*radius, height*0.5, -sin((i + 1) * step *M_PI / 180.0)*radius);

		glVertex3f(cos(i * step * M_PI / 180.0)*radius, -height*0.5, -sin(i * step *M_PI / 180.0)*radius);
		glVertex3f(cos((i + 1) * step * M_PI / 180.0)*radius, -height*0.5, -sin((i + 1) * step *M_PI / 180.0)*radius);
		glVertex3f(cos((i + 1) * step * M_PI / 180.0)*radius, height*0.5, -sin((i + 1) * step *M_PI / 180.0)*radius);
	}
	glEnd();
}


/*----------------------------------------------------------------------------------- 
	Draw Cylinder with strips and fans

	  parameters: radius, height, sides

-----------------------------------------------------------------------------------*/

void cylinder0(float radius, float height, int sides) {

	int i;
	float step;

	step = 360.0/sides;

	glColor3f(1,0,0);
	glBegin(GL_TRIANGLE_FAN);

		glVertex3f(0,height*0.5,0);
		for (i=0; i <= sides; i++) {
			glVertex3f(cos(i * step * M_PI/180.0)*radius,height*0.5,-sin(i * step *M_PI/180.0)*radius);
		}
	glEnd();

	glColor3f(0,1,0);
	glBegin(GL_TRIANGLE_FAN);

		glVertex3f(0,-height*0.5,0);
		for (i=0; i <= sides; i++) {
			glVertex3f(cos(i * step * M_PI/180.0)*radius,-height*0.5,sin(i * step *M_PI/180.0)*radius);
		}
	glEnd();

	glColor3f(0,0,1);
	glBegin(GL_TRIANGLE_STRIP);

		for (i=0; i <= sides; i++) {
			glVertex3f(cos(i * step * M_PI/180.0)*radius, height*0.5,-sin(i * step *M_PI/180.0)*radius);
			glVertex3f(cos(i * step * M_PI/180.0)*radius,-height*0.5,-sin(i * step *M_PI/180.0)*radius);
		}
	glEnd();
}


/*-----------------------------------------------------------------------------------
	Draw Cylinder with VBOs

		parameters: radius, height, sides

-----------------------------------------------------------------------------------*/

void cylinder1(float radius, float height, int sides) {

    // arrays for vertices
    vector<float> vertexTop;
    vector<float> vertexBottom;
    vector<float> vertexBody;

    int i;
    float step = 360.0 / sides;

    // Top
    for (i = 0; i < sides; i++) {
        vertexTop.push_back(0);
        vertexTop.push_back(height * 0.5);
        vertexTop.push_back(0);

        vertexTop.push_back(cos(i * step * M_PI / 180.0) * radius);
        vertexTop.push_back(height * 0.5);
        vertexTop.push_back(-sin(i * step * M_PI / 180.0) * radius);

        vertexTop.push_back(cos((i + 1) * step * M_PI / 180.0) * radius);
        vertexTop.push_back(height * 0.5);
        vertexTop.push_back(-sin((i + 1) * step * M_PI / 180.0) * radius);
    }

    // Bottom
    for (i = 0; i < sides; i++) {
        vertexBottom.push_back(0);
        vertexBottom.push_back(-height * 0.5);
        vertexBottom.push_back(0);

        vertexBottom.push_back(cos((i + 1) * step * M_PI / 180.0) * radius);
        vertexBottom.push_back(-height * 0.5);
        vertexBottom.push_back(-sin((i + 1) * step * M_PI / 180.0) * radius);

        vertexBottom.push_back(cos(i * step * M_PI / 180.0) * radius);
        vertexBottom.push_back(-height * 0.5);
        vertexBottom.push_back(-sin(i * step * M_PI / 180.0) * radius);
    }

    // Body
    for (i = 0; i <= sides; i++) {
        vertexBody.push_back(cos(i * step * M_PI / 180.0) * radius);
        vertexBody.push_back(height * 0.5);
        vertexBody.push_back(-sin(i * step * M_PI / 180.0) * radius);

        vertexBody.push_back(cos(i * step * M_PI / 180.0) * radius);
        vertexBody.push_back(-height * 0.5);
        vertexBody.push_back(-sin(i * step * M_PI / 180.0) * radius);

        vertexBody.push_back(cos((i + 1) * step * M_PI / 180.0) * radius);
        vertexBody.push_back(height * 0.5);
        vertexBody.push_back(-sin((i + 1) * step * M_PI / 180.0) * radius);

        vertexBody.push_back(cos(i * step * M_PI / 180.0) * radius);
        vertexBody.push_back(-height * 0.5);
        vertexBody.push_back(-sin(i * step * M_PI / 180.0) * radius);

        vertexBody.push_back(cos((i + 1) * step * M_PI / 180.0) * radius);
        vertexBody.push_back(-height * 0.5);
        vertexBody.push_back(-sin((i + 1) * step * M_PI / 180.0) * radius);

        vertexBody.push_back(cos((i + 1) * step * M_PI / 180.0) * radius);
        vertexBody.push_back(height * 0.5);
        vertexBody.push_back(-sin((i + 1) * step * M_PI / 180.0) * radius);
    }

    glGenBuffers(3, buffers);

    // Top VBO
    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexTop.size(), vertexTop.data(), GL_STATIC_DRAW);
    vertices[0] = vertexTop.size() / 3;

    // Bottom VBO
    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexBottom.size(), vertexBottom.data(), GL_STATIC_DRAW);
    vertices[1] = vertexBottom.size() / 3;

    // Body VBO
    glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexBody.size(), vertexBody.data(), GL_STATIC_DRAW);
    vertices[2] = vertexBody.size() / 3;
}



void renderScene(void) {
	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set the camera
	glLoadIdentity();
	gluLookAt(camX, camY, camZ,
		0.0, 0.0, 0.0,
		0.0f, 1.0f, 0.0f);

	glBindBuffer(GL_ARRAY_BUFFER,buffers[0]);
	glVertexPointer(3,GL_FLOAT,0,0);
	glDrawArrays(GL_TRIANGLES, 0, vertices[0]);

	glBindBuffer(GL_ARRAY_BUFFER,buffers[1]);
	glVertexPointer(3,GL_FLOAT,0,0);
	glDrawArrays(GL_TRIANGLES, 0, vertices[1]);

	glBindBuffer(GL_ARRAY_BUFFER,buffers[2]);
	glVertexPointer(3,GL_FLOAT,0,0);
	glDrawArrays(GL_TRIANGLES, 0, vertices[2]);
	
	// calculate fps
    char s[32];

    frame++;
    time = glutGet(GLUT_ELAPSED_TIME);
    if (time - timebase > 1000) {
        frames = frame*1000.0 / (time - timebase);
        timebase = time;
        frame = 0;
        sprintf(s, "CG@DI-UM %f fps", frames);
        glutSetWindowTitle(s);
    }

	// End of frame
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
		betA += 0.1f;
		if (betA > 1.5f)
			betA = 1.5f;
		break;

	case GLUT_KEY_DOWN:
		betA -= 0.1f;
		if (betA < -1.5f)
			betA = -1.5f;
		break;

	case GLUT_KEY_PAGE_DOWN: radius -= 0.1f;
		if (radius < 0.1f)
			radius = 0.1f;
		break;

	case GLUT_KEY_PAGE_UP: radius += 0.1f; break;
	}
	spherical2Cartesian();
	glutPostRedisplay();

}


void printInfo() {
	printf("Vendor: %s\n", glGetString(GL_VENDOR));
	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("Version: %s\n", glGetString(GL_VERSION));

	printf("\nUse Arrows to move the camera up/down and left/right\n");
	printf("Page Up and Page Down control the distance from the camera to the origin\n");
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
	glutIdleFunc(renderScene);
	glutReshapeFunc(changeSize);
	
// Callback registration for keyboard processing
	glutKeyboardFunc(processKeys);
	glutSpecialFunc(processSpecialKeys);

	// init GLEW
#ifndef __APPLE__
	glewInit();
#endif

// Do once during initialization of the app
	glEnableClientState(GL_VERTEX_ARRAY);

// Load cylinder 
	cylinder1(1,2,65536);

// Set global time
	timebase = glutGet(GLUT_ELAPSED_TIME);

//  OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT, GL_LINE);

	spherical2Cartesian();

	printInfo();

// enter GLUT's main cycle
	glutMainLoop();
	
	return 1;
}
