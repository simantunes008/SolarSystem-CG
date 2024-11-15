#define _USE_MATH_DEFINES
#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <GL/glut.h>
#include "tinyxml2.h"

using namespace tinyxml2;
using namespace std;

// Structure to represent a point in 3D space
struct Point {
    float x;
    float y;
    float z;
};

// Structure to store window settings
struct Window {
    float width, height;
};

// Structure to store camera settings
struct Camera {
    float posX, posY, posZ;
    float lookAtX, lookAtY, lookAtZ;
    float upX, upY, upZ;
    float fov, nearPlane, farPlane;
};

Window window;
Camera camera;

// Vector to store all the models read in the xml
vector<string> models;

// Vector to store all the points to be drawn
vector<Point> points;

bool axes = false;
bool lines = false;
float alpha, betA, radius;


void spherical2Cartesian() {
    // Updates camera postition
	camera.posX = radius * cos(betA) * sin(alpha);
	camera.posY = radius * sin(betA);
	camera.posZ = radius * cos(betA) * cos(alpha);
}


void parseXML(char * filename) {
    XMLDocument doc;
    if (doc.LoadFile(filename) != XML_SUCCESS) {
        cerr << "Error loading xml " << filename << endl;
        return;
    }

    XMLElement * worldElement = doc.FirstChildElement("world");

    XMLElement * windowElement = worldElement->FirstChildElement("window");
    if (windowElement) {
        windowElement->QueryFloatAttribute("width", &window.width);
        windowElement->QueryFloatAttribute("height", &window.height);
    }

    XMLElement * cameraElement = worldElement -> FirstChildElement("camera");
    if (cameraElement) {
        XMLElement * positionElement = cameraElement -> FirstChildElement("position");
        XMLElement * lookAtElement = cameraElement -> FirstChildElement("lookAt");
        XMLElement * upElement = cameraElement -> FirstChildElement("up");
        XMLElement * projectionElement = cameraElement -> FirstChildElement("projection");

        if (positionElement && lookAtElement && upElement && projectionElement) {
            positionElement -> QueryFloatAttribute("x", &camera.posX);
            positionElement -> QueryFloatAttribute("y", &camera.posY);
            positionElement -> QueryFloatAttribute("z", &camera.posZ);

            lookAtElement -> QueryFloatAttribute("x", &camera.lookAtX);
            lookAtElement -> QueryFloatAttribute("y", &camera.lookAtY);
            lookAtElement -> QueryFloatAttribute("z", &camera.lookAtZ);

            upElement -> QueryFloatAttribute("x", &camera.upX);
            upElement -> QueryFloatAttribute("y", &camera.upY);
            upElement -> QueryFloatAttribute("z", &camera.upZ);

            projectionElement -> QueryFloatAttribute("fov", &camera.fov);
            projectionElement -> QueryFloatAttribute("near", &camera.nearPlane);
            projectionElement -> QueryFloatAttribute("far", &camera.farPlane);
        }

        float dirX = camera.lookAtX - camera.posX;
        float dirY = camera.lookAtY - camera.posY;
        float dirZ = camera.lookAtZ - camera.posZ;

        radius = sqrt(pow(dirX, 2) + pow(dirY, 2) + pow(dirZ, 2));
        alpha = asin(camera.posX / sqrt(pow(camera.posZ, 2) + pow(camera.posX, 2)));
        betA = asin(camera.posY / radius);
    }

    XMLElement * groupElement = worldElement -> FirstChildElement("group");
    if (groupElement) {
        XMLElement * modelsElement = groupElement -> FirstChildElement("models");
        if (modelsElement) {
            XMLElement * modelElement = modelsElement -> FirstChildElement("model");
            while (modelElement) {
                const char * filename = modelElement -> Attribute("file");
                if (filename) {
                    string modelFile = "../../Models/";
                    modelFile += filename;
                    models.push_back(modelFile);
                }
                modelElement = modelElement -> NextSiblingElement("model");
            }
        }
    }
}


void loadModel(string filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening model " << filename << endl;
        return;
    }

    int numPoints;
    file >> numPoints;

    // You cant draw 0 points
    if (numPoints <= 0) { 
        cerr << "Invalid number of points in model " << filename << endl;
        return;
    }

    // Stores all the points
    for (int i = 0; i < numPoints; i++) {
        Point point;
        char separator;
        file >> point.x >> separator >> point.y >> separator >> point.z;
        points.push_back(point);
    }

    cout << "Model " << filename << " loaded with sucess!" << endl;

    file.close();
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
    gluPerspective(camera.fov, ratio, camera.nearPlane, camera.farPlane);

    // return to the model view matrix mode
    glMatrixMode(GL_MODELVIEW);
}


void drawAxes() {
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
}


void drawPrimitives() {
    // Vertex counter
    int c = 0;

    glBegin(GL_TRIANGLES);
    for (int i = 0; i < points.size(); i += 3) {
        // Alternating colors for each face
        if (lines) {
            glColor3f(1.0, 1.0, 1.0); // Lines are always displayed in white
        } else if (c % 2 == 0) {
            glColor3f(1.00, 0.60, 0.93); // Pink
        } else {
            glColor3f(1.00, 1.00, 0.50); // Yellow
        }

        // Draw triangle
        for (int j = 0; j < 3; j++) {
            Point pt = points[i + j];
            glVertex3f(pt.x, pt.y, pt.z);
        }

        c++;
    }
    glEnd();
}


void renderScene(void) {
	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set drawing mode
	if (lines) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// set the camera
	glLoadIdentity();
	gluLookAt(camera.posX,camera.posY,camera.posZ, 
		      camera.lookAtX,camera.lookAtY,camera.lookAtZ,
			  camera.upX,camera.upY,camera.upZ);

	if (axes) drawAxes();

    drawPrimitives();

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
		betA += 0.1f;
		if (betA >= M_PI / 2) betA = M_PI / 2;
		break;

	case GLUT_KEY_DOWN:
		betA -= 0.1f;
		if (betA <= -M_PI / 2) betA = -M_PI / 2;
		break;
	}

	spherical2Cartesian();

	glutPostRedisplay();
}


void printInfo() {
    cout << "\nUse arrow keys to move the camera up/down and left/right." << endl;
    cout << "Use + and - to control the distance from the camera to the origin." << endl;
    cout << "Press 'a' to toggle the display of axes." << endl;
    cout << "Press 'l' to toggle the display of lines." << endl;
}


int main(int argc, char **argv) {
    
    parseXML(argv[1]);

    for(int i = 0; i < models.size(); i++){
        loadModel(models[i]);
    }

    printInfo();

    // Init GLUT and the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(window.width,window.height);
	glutCreateWindow(argv[1]);
		
    // Required callback registry 
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
	
    // Callback registration for keyboard processing
	glutKeyboardFunc(processKeys);
	glutSpecialFunc(processSpecialKeys);

    // OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	
    // Enter GLUT's main cycle
	glutMainLoop();
	
	return 1;
}	
