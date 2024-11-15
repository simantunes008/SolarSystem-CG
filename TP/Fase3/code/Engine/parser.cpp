#include "parser.h"

// Given a .3d file loads all the points into a vector
vector<Point> loadPoints(string filename) {
    vector<Point> res;

    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening model " << filename << endl;
        return res;
    }

    int numPoints;
    file >> numPoints;

    // You can't draw 0 points
    if (numPoints <= 0) { 
        cerr << "Invalid number of points in model " << filename << endl;
        return res;
    }

    // Stores all the points
    for (int i = 0; i < numPoints; i++) {
        Point point;
        char separator;
        file >> point.x >> separator >> point.y >> separator >> point.z;
        res.push_back(point);
    }

    cout << "Model " << filename << " loaded successfully!" << endl;

    file.close();

    return res;
}


// Stores window data
Window* parseWindow(XMLElement * windowElement) {
    Window *win = new Window;

    windowElement -> QueryFloatAttribute("width", &win -> width);
    windowElement -> QueryFloatAttribute("height", &win -> height);

    return win;
}


// Stores all camera data
Camera* parseCamera(XMLElement * cameraElement) {
    Camera *cam = new Camera;

    XMLElement * positionElement = cameraElement -> FirstChildElement("position");
    XMLElement * lookAtElement = cameraElement -> FirstChildElement("lookAt");
    XMLElement * upElement = cameraElement -> FirstChildElement("up");
    XMLElement * projectionElement = cameraElement -> FirstChildElement("projection");

    if (positionElement && lookAtElement && upElement && projectionElement) {
        positionElement->QueryFloatAttribute("x", &cam -> position[0]);
        positionElement->QueryFloatAttribute("y", &cam -> position[1]);
        positionElement->QueryFloatAttribute("z", &cam -> position[2]);

        lookAtElement->QueryFloatAttribute("x", &cam -> lookAt[0]);
        lookAtElement->QueryFloatAttribute("y", &cam -> lookAt[1]);
        lookAtElement->QueryFloatAttribute("z", &cam -> lookAt[2]);

        upElement->QueryFloatAttribute("x", &cam -> up[0]);
        upElement->QueryFloatAttribute("y", &cam -> up[1]);
        upElement->QueryFloatAttribute("z", &cam -> up[2]);

        projectionElement->QueryFloatAttribute("fov", &cam-> projection[0]);
        projectionElement->QueryFloatAttribute("near", &cam -> projection[1]);
        projectionElement->QueryFloatAttribute("far", &cam -> projection[2]);
    }

    return cam;
}


map<string, pair<int, int>> files;
int i = 0;

//
Group parseGroup(XMLElement* groupElement, vector<float> *vertices) {
    Group group;

    XMLElement * transformElement = groupElement -> FirstChildElement("transform");
    if (transformElement) {

        XMLElement * transformationElement = transformElement -> FirstChildElement();
        while (transformationElement) {
            if (!strcmp("color", transformationElement->Name())) {
                Transformation t;
                t.name = "color";

                transformationElement->QueryFloatAttribute("r", &t.x);
                transformationElement->QueryFloatAttribute("g", &t.y);
                transformationElement->QueryFloatAttribute("b", &t.z);

                group.transformations.push_back(t);
            }
            
            if (!strcmp("translate", transformationElement->Name())) {
                Transformation t;
                t.name = "translate";
                t.time = 0;
                t.align = false;

                if (transformationElement -> Attribute("time")) {
                    transformationElement->QueryFloatAttribute("time", &t.time);
                    transformationElement->QueryBoolAttribute("align", &t.align);

                    XMLElement *pointElement = transformationElement -> FirstChildElement();
                    while (pointElement) {
                        Point p;

                        pointElement->QueryFloatAttribute("x", &p.x);
                        pointElement->QueryFloatAttribute("y", &p.y);
                        pointElement->QueryFloatAttribute("z", &p.z);

                        t.points.push_back(p);
                        pointElement = pointElement -> NextSiblingElement();
                   }
                } else {
                    transformationElement->QueryFloatAttribute("x", &t.x);
                    transformationElement->QueryFloatAttribute("y", &t.y);
                    transformationElement->QueryFloatAttribute("z", &t.z);
                }
            
                group.transformations.push_back(t);
            }

            if (!strcmp("rotate", transformationElement->Name())) {
                Transformation t;
                t.name = "rotate";
                t.time = -1;

                if (transformationElement->Attribute("time")) {
                    transformationElement->QueryFloatAttribute("time", &t.time);
                } else {
                    transformationElement->QueryFloatAttribute("angle", &t.angle);
                }

                transformationElement->QueryFloatAttribute("x", &t.x);
                transformationElement->QueryFloatAttribute("y", &t.y);
                transformationElement->QueryFloatAttribute("z", &t.z);

                group.transformations.push_back(t);
            }

            if (!strcmp("scale", transformationElement->Name())) {
                Transformation t;
                t.name = "scale";

                transformationElement->QueryFloatAttribute("x", &t.x);
                transformationElement->QueryFloatAttribute("y", &t.y);
                transformationElement->QueryFloatAttribute("z", &t.z);

                group.transformations.push_back(t);
            }

            transformationElement = transformationElement->NextSiblingElement();
        }
    }

    XMLElement * modelsElement = groupElement -> FirstChildElement("models");
    if (modelsElement) {
        XMLElement * modelElement = modelsElement -> FirstChildElement("model");
        while (modelElement) {

            const char * filename = modelElement -> Attribute("file");
            string modelFile = "../../Models/";
            modelFile += filename;

            if (files.find(modelFile) != files.end()) {
                auto it = files.find(modelFile);
                group.vboIndexes.push_back(it->second);

            } else {
            //  Load points into vector
                vector<Point> points = loadPoints(modelFile);

            //  Insert in map to avoid loading the same model again
                files.insert({modelFile, pair<int,int>{i,points.size()}});

                group.vboIndexes.emplace_back(i, points.size());
                
                i += points.size();
                for (Point p : points){
                    vertices->push_back(p.x);
                    vertices->push_back(p.y);
                    vertices->push_back(p.z);
                }
            }

            modelElement = modelElement -> NextSiblingElement("model");
        }
    }

    XMLElement * subGroupElement = groupElement -> FirstChildElement("group");
    while (subGroupElement) {
        group.subGroups.push_back(parseGroup(subGroupElement, vertices));
        subGroupElement = subGroupElement -> NextSiblingElement("group");
    }

    return group;
}


// XML parsing function
int parseXML(const char* filename, World *world, vector<float> *vert) {
    XMLDocument doc;
    if (doc.LoadFile(filename) != XML_SUCCESS) {
        cerr << "Error loading xml " << filename << endl;
        return 1;
    }

    XMLElement *worldElement = doc.FirstChildElement("world");
    if (worldElement) {

        XMLElement * windowElement = worldElement -> FirstChildElement("window");
        if (windowElement) {
            world -> win = parseWindow(windowElement);
        } else {
            return 1;
        }

        XMLElement* cameraElement = worldElement -> FirstChildElement("camera");
        if (cameraElement) {
            world -> cam = parseCamera(cameraElement);
        } else {
            return 1;
        }

        XMLElement *groupElement = worldElement -> FirstChildElement("group");
        while (groupElement) {
            (*world).groups.push_back(parseGroup(groupElement, vert));
            groupElement = groupElement -> NextSiblingElement("group");
        }
    } else {
        return 1;
    }

    return 0;
}