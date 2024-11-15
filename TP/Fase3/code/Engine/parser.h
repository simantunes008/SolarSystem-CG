#ifndef ENGINE_PARSER_H
#include <GL/glew.h>
#include <GL/glut.h>
#define ENGINE_PARSER_H

#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include "tinyxml2.h"
#include "structures.h"

using namespace tinyxml2;
using namespace std;

int parseXML(const char* file_name, World *w, vector<float> *vert);

#endif //ENGINE_PARSER_H