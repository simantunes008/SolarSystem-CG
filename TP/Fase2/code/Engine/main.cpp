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

// Structure to represent a transformation
struct Transformation {
    float angle;
    float x;
    float y;
    float z;
};

// Structure to represent all transformations
struct Transformations {
    Transformation scale;
    Transformation translate;
    Transformation rotate;
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

// Structure to store a xml group
struct Group {
    Transformations transformations;
    vector<string> models;
    vector<Point> points;
    vector<Group> subGroups;
};

Window window;
Camera camera;

// Vector to store all the main groups read in the xml
vector<Group> groups;   

bool axes = false;
bool lines = false;
float alpha, betA, radius;
float lastMouseX, lastMouseY;


void spherical2Cartesian() {
    // Updates camera postition
	camera.posX = radius * cos(betA) * sin(alpha);
	camera.posY = radius * sin(betA);
	camera.posZ = radius * cos(betA) * cos(alpha);
}


Transformations initTransformations(){
    Transformations t;

    t.scale.angle = 0;
    t.scale.x = 1;
    t.scale.y = 1;
    t.scale.z = 1;

    t.translate.angle = 0;
    t.translate.x = 0;
    t.translate.y = 0;
    t.translate.z = 0;

    t.rotate.angle = 0;
    t.rotate.x = 0;
    t.rotate.y = 0;
    t.rotate.z = 0;

    return t;
}


void parseCamera(XMLElement * cameraElement) {
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


vector<Point> loadModels(const vector<string>& models) {
    vector<Point> points;

    for (const string& filename : models) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Error opening model " << filename << endl;
            continue;
        }

        int numPoints;
        file >> numPoints;

        // You can't draw 0 points
        if (numPoints <= 0) { 
            cerr << "Invalid number of points in model " << filename << endl;
            continue;
        }

        // Stores all the points
        for (int i = 0; i < numPoints; i++) {
            Point point;
            char separator;
            file >> point.x >> separator >> point.y >> separator >> point.z;
            points.push_back(point);
        }

        cout << "Model " << filename << " loaded successfully!" << endl;

        file.close();
    }

    return points;
}


Group parseGroups(XMLElement * groupElement) {
    Group group;
    group.transformations = initTransformations();

    XMLElement * transformElement = groupElement -> FirstChildElement("transform");
    if (transformElement) {
        XMLElement * scaleElement = transformElement->FirstChildElement("scale");
        XMLElement * translateElement = transformElement->FirstChildElement("translate");
        XMLElement * rotateElement = transformElement->FirstChildElement("rotate");

        if (rotateElement) {
            rotateElement->QueryFloatAttribute("angle", &group.transformations.rotate.angle);
            rotateElement->QueryFloatAttribute("x", &group.transformations.rotate.x);
            rotateElement->QueryFloatAttribute("y", &group.transformations.rotate.y);
            rotateElement->QueryFloatAttribute("z", &group.transformations.rotate.z);
        }

        if (translateElement) {
            translateElement->QueryFloatAttribute("angle", &group.transformations.translate.angle);
            translateElement->QueryFloatAttribute("x", &group.transformations.translate.x);
            translateElement->QueryFloatAttribute("y", &group.transformations.translate.y);
            translateElement->QueryFloatAttribute("z", &group.transformations.translate.z);
        }

        if (scaleElement) {
            scaleElement->QueryFloatAttribute("angle", &group.transformations.scale.angle);
            scaleElement->QueryFloatAttribute("x", &group.transformations.scale.x);
            scaleElement->QueryFloatAttribute("y", &group.transformations.scale.y);
            scaleElement->QueryFloatAttribute("z", &group.transformations.scale.z);
        }
    }

    XMLElement * modelsElement = groupElement -> FirstChildElement("models");
    if (modelsElement) {
        XMLElement * modelElement = modelsElement -> FirstChildElement("model");
        while (modelElement) {
            const char * filename = modelElement -> Attribute("file");
            if (filename) {
                string modelFile = "../../Models/";
                modelFile += filename;
                group.models.push_back(modelFile);
            }
            modelElement = modelElement -> NextSiblingElement("model");
        }
    }

    group.points = loadModels(group.models);

    XMLElement * subGroupElement = groupElement -> FirstChildElement("group");
    while (subGroupElement) {
        group.subGroups.push_back(parseGroups(subGroupElement));
        subGroupElement = subGroupElement -> NextSiblingElement("group");
    }

    return group;
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
        parseCamera(cameraElement);
    }

    XMLElement * groupElement = worldElement -> FirstChildElement("group");
    if (groupElement) {
        groups.push_back(parseGroups(groupElement));
        groupElement = groupElement -> NextSiblingElement("group");
    }
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


void drawPrimitives(vector<Point> points) {
    // Vertex counter
    int vc = 0;

    glBegin(GL_TRIANGLES);
    for (int i = 0; i < points.size(); i += 3) {
        // Alternating colors for each face
        if (lines) {
            glColor3f(1.0, 1.0, 1.0); // Lines are always displayed in white
        } else if (vc % 2 == 0) {
            glColor3f(1.00, 0.60, 0.93); // Pink
        } else {
            glColor3f(1.00, 1.00, 0.50); // Yellow
        }

        // Draw triangle
        for (int j = 0; j < 3; j++) {
            Point pt = points[i + j];
            glVertex3f(pt.x, pt.y, pt.z);
        }

        vc++;
    }
    glEnd();
}


void drawGroups(vector<Group> groups) {
    for(Group g : groups){
        glPushMatrix();

        // Aply Transformations
        glRotatef(g.transformations.rotate.angle, g.transformations.rotate.x, g.transformations.rotate.y, g.transformations.rotate.z);
        glTranslatef(g.transformations.translate.x, g.transformations.translate.y, g.transformations.translate.z);
        glScalef(g.transformations.scale.x, g.transformations.scale.y, g.transformations.scale.z);

        drawPrimitives(g.points);
        // Draw subgroups recursively
        drawGroups(g.subGroups);

        glPopMatrix();
    }
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

    drawGroups(groups);

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
        radius -= 0.1f;
        break;

    case '-':
        radius += 0.1f;
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

    betA += deltaY * 0.01f;
    if (betA >= M_PI / 2) betA = M_PI / 2;
    if (betA <= -M_PI / 2) betA = -M_PI / 2;

    lastMouseX = x;
    lastMouseY = y;

    spherical2Cartesian();

    glutPostRedisplay();
}


void printInfo() {
    cout << "\nCAMERA CONTROLS" << endl;
    cout << "- Use arrow keys to move the camera up/down and left/right." << endl;
    cout << "- Use '+' and '-' to control the distance from the camera to the origin." << endl;
    cout << "- Alternatively, you can move the camera by dragging the mouse while holding down the left button." << endl;

    cout << "\nOTHER CONTROLS" << endl;
    cout << "- Press 'a' to toggle the display of axes." << endl;
    cout << "- Press 'l' to toggle the display of lines." << endl;
}


int main(int argc, char **argv) {
    
    parseXML(argv[1]);

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
	
    // Callback registration for keyboard and mouse processing
	glutKeyboardFunc(processKeys);
	glutSpecialFunc(processSpecialKeys);
    glutMouseFunc(processMouse);
    glutMotionFunc(processMouseActiveMotion);

    // OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	
    // Enter GLUT's main cycle
	glutMainLoop();
	
	return 1;
}	
