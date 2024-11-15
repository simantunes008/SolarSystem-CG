#ifndef ENGINE_PARSER_H
#include <GL/glew.h>
#include <GL/glut.h>
#include <IL/il.h>
#define ENGINE_PARSER_H

#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include "tinyxml2.h"
#include "structures.h"

using namespace tinyxml2;
using namespace std;

int parseXML(const char* filename, World *world, vector<float> *vert, vector<float> *norm, vector<float> *text);

#endif //ENGINE_PARSER_H