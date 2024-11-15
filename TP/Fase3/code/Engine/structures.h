#ifndef ENGINE_STRUCTURES_H
#include <GL/glew.h>
#include <GL/glut.h>
#define ENGINE_STRUCTURES_H

#include <vector>
#include <string>

using namespace std;

// Structure to represent a point in 3D space
struct Point {
    float x;
    float y;
    float z;
};

// Structure to represent any transformation
struct Transformation {
    string name;
    float x;
    float y;
    float z;
    float angle;
    float time;
    bool align;
    vector<Point> points;
};

// Structure to store window settings
struct Window {
    float width;
    float height;
};

// Structure to store camera settings
struct Camera {
    float position[3]   = {0, 0, 0};
    float lookAt[3]     = {0, 0, 0};
    float up[3]         = {0, 0, 0};
    float projection[3] = {0, 0, 0};
};

// Structure to store a xml group
struct Group {
    vector<Transformation> transformations; // Geometric Transforms
    vector<Group> subGroups;                // Subgroups of the group
    vector<pair<int,int>> vboIndexes;       // 
};

// 
struct World {
    Window *win = new Window;
    Camera *cam = new Camera;
    vector<Group> groups;
};

#endif //ENGINE_STRUCTURES_H