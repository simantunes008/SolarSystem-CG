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
    float nx;
    float ny;
    float nz;
    float tx;
    float tz;
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
    float position[3]   = {0.0f, 0.0f, 0.0f};
    float lookAt[3]     = {0.0f, 0.0f, 0.0f};
    float up[3]         = {0.0f, 0.0f, 0.0f};
    float projection[3] = {0.0f, 0.0f, 0.0f};
};

// Structure to store light settings
struct Light {
    string type;
    float position[4]  = {0.0f, 0.0f, 0.0f, 1.0f};
    float direction[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    float cutoff = 0.0f;
};

// Structure to store color settings
struct Color {
    float diffuse[4]  = {0.8f, 0.8f, 0.8f, 1.0f};
    float ambient[4]  = {0.2f, 0.2f, 0.2f, 1.0f};
    float specular[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    float emissive[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    float shininess = 0.0f;
};

// Structure to store a model
struct Model {
    string name;
    int vertexCount;
    int indexBegin;
    int textureID;
    Color color;
};

// Structure to store a xml group
struct Group {
    vector<Transformation> transformations; // Geometric Transforms
    vector<Model> models;                   // Models to be drawn
    vector<Group> subGroups;                // Subgroups of the group
};

struct World {
    Window *win = new Window;
    Camera *cam = new Camera;
    vector<Light> lights;
    vector<Group> groups;
};

#endif //ENGINE_STRUCTURES_H