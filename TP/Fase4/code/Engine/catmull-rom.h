#ifndef ENGINE_CATMULL_ROM_H
#include <GL/glew.h>
#include <GL/glut.h>
#define ENGINE_CATMULL_ROM_H

#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include "structures.h"

using namespace std;

void buildRotMatrix(float *x, float *y, float *z, float *m);

void cross(float *a, float *b, float *res);

void normalize(float *a);

void getGlobalCatmullRomPoint(float gt, float *pos, float *deriv, vector<Point> p);

void renderCatmullRomCurve(vector<Point> p);

#endif //ENGINE_CATMULL_ROM_H