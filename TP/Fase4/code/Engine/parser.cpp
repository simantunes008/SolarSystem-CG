#include "parser.h"

vector<float> vertexPoints, normalPoints, texturePoints;
map<string, pair<int, int>> files;
map<string, int> textureIDs;
int i = 0;


int loadTexture(string s) {
    unsigned int t,tw,th;
    unsigned char *texData;
    unsigned int texID;

    // Iniciar o DevIL
    ilInit();

    // Colocar a origem da textura no canto inferior esquerdo
    ilEnable(IL_ORIGIN_SET);
    ilOriginFunc(IL_ORIGIN_LOWER_LEFT);

    // Carregar a textura para memória
    ilGenImages(1,&t);
    ilBindImage(t);
    ilLoadImage((ILstring)s.c_str());
    tw = ilGetInteger(IL_IMAGE_WIDTH);
    th = ilGetInteger(IL_IMAGE_HEIGHT);

    // Assegurar que a textura se encontra em RGBA (Red, Green, Blue, Alpha) com um byte (0 - 255) por componente
    ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
    texData = ilGetData();

    // Gerar a textura para a placa gráfica
    glGenTextures(1,&texID);

    glBindTexture(GL_TEXTURE_2D,texID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    // Upload dos dados de imagem
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tw, th, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    return texID;
}


// Given a .3d file loads all the points into a vector
int loadPoints(string filename) {

    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening model " << filename << endl;
        return 0;
    }

    int numPoints;
    file >> numPoints;

    // You can't draw 0 points
    if (numPoints <= 0) { 
        cerr << "Invalid number of points in model " << filename << endl;
        return 0;
    }

    // Stores all the points
    for (int i = 0; i < numPoints; i++) {
        Point point;
        char separator;
        
        file >> point.x >> separator >> point.y >> separator >> point.z >> separator >> point.nx >> separator >> point.ny >> separator >> point.nz >> separator >> point.tx >> separator >> point.tz;
        
        vertexPoints.push_back(point.x);
        vertexPoints.push_back(point.y);
        vertexPoints.push_back(point.z);

        normalPoints.push_back(point.nx);
        normalPoints.push_back(point.ny);
        normalPoints.push_back(point.nz);

        texturePoints.push_back(point.tx);
        texturePoints.push_back(point.tz);
    }

    cout << "Model " << filename << " loaded successfully!" << endl;

    file.close();

    return numPoints;
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


// Stores all light data
vector<Light> parseLights(XMLElement * lightsElement) {
    vector<Light> lights;

    XMLElement * lightElement = lightsElement -> FirstChildElement("light");
    while (lightElement) {
        Light l;

        const char * type = lightElement -> Attribute("type");
        l.type = type;

        if (!strcmp(type, "point"))  {
            lightElement->QueryFloatAttribute("posx", &l.position[0]);
            lightElement->QueryFloatAttribute("posy", &l.position[1]);
            lightElement->QueryFloatAttribute("posz", &l.position[2]);
        }

        if (!strcmp(type, "directional")) {
            lightElement->QueryFloatAttribute("dirx", &l.direction[0]);
            lightElement->QueryFloatAttribute("diry", &l.direction[1]);
            lightElement->QueryFloatAttribute("dirz", &l.direction[2]);
        }

        if (!strcmp(type, "spot")) {
            lightElement->QueryFloatAttribute("posx", &l.position[0]);
            lightElement->QueryFloatAttribute("posy", &l.position[1]);
            lightElement->QueryFloatAttribute("posz", &l.position[2]);
            
            lightElement->QueryFloatAttribute("dirx", &l.direction[0]);
            lightElement->QueryFloatAttribute("diry", &l.direction[1]);
            lightElement->QueryFloatAttribute("dirz", &l.direction[2]);
            
            lightElement->QueryFloatAttribute("cutoff", &l.cutoff);
        }

        lights.push_back(l);
        lightElement = lightElement -> NextSiblingElement("light");
    }

    return lights;
}


// Stores all color data
Color parseColor(XMLElement * colorElement) {
    Color color;

    XMLElement * diffuseElement = colorElement->FirstChildElement("diffuse");
    XMLElement * ambientElement = colorElement->FirstChildElement("ambient");
    XMLElement * specularElement = colorElement->FirstChildElement("specular");
    XMLElement * emissiveElement = colorElement->FirstChildElement("emissive");
    XMLElement * shininessElement = colorElement->FirstChildElement("shininess");

    if (diffuseElement) {
        float r, g, b;
        diffuseElement->QueryFloatAttribute("R", &r);
        diffuseElement->QueryFloatAttribute("G", &g);
        diffuseElement->QueryFloatAttribute("B", &b);
        color.diffuse[0] = r / 255.0f;
        color.diffuse[1] = g / 255.0f;
        color.diffuse[2] = b / 255.0f;
    }

    if (ambientElement) {
        float r, g, b;
        ambientElement->QueryFloatAttribute("R", &r);
        ambientElement->QueryFloatAttribute("G", &g);
        ambientElement->QueryFloatAttribute("B", &b);
        color.ambient[0] = r / 255.0f;
        color.ambient[1] = g / 255.0f;
        color.ambient[2] = b / 255.0f;
    }

    if (specularElement) {
        float r, g, b;
        specularElement->QueryFloatAttribute("R", &r);
        specularElement->QueryFloatAttribute("G", &g);
        specularElement->QueryFloatAttribute("B", &b);
        color.specular[0] = r / 255.0f;
        color.specular[1] = g / 255.0f;
        color.specular[2] = b / 255.0f;
    }

    if (emissiveElement) {
        float r, g, b;
        emissiveElement->QueryFloatAttribute("R", &r);
        emissiveElement->QueryFloatAttribute("G", &g);
        emissiveElement->QueryFloatAttribute("B", &b);
        color.emissive[0] = r / 255.0f;
        color.emissive[1] = g / 255.0f;
        color.emissive[2] = b / 255.0f;
    }

    if (shininessElement) {
        shininessElement->QueryFloatAttribute("value", &color.shininess);
    }

    return color;
}


// 
Group parseGroup(XMLElement* groupElement) {
    Group group;

    XMLElement * transformElement = groupElement -> FirstChildElement("transform");
    if (transformElement) {

        XMLElement * transformationElement = transformElement -> FirstChildElement();
        while (transformationElement) {
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

            Model m;
            const char * modelName = modelElement -> Attribute("file");
            string modelPath = "../../Models/";
            modelPath += modelName;
            m.name = modelName;

            if (files.find(m.name) != files.end()) {
                auto it = files.find(m.name);
                m.indexBegin = it->second.first;
                m.vertexCount = it->second.second;
            } else {
                m.vertexCount = loadPoints(modelPath);
                files.insert({m.name, pair<int,int>{i, m.vertexCount}});
                m.indexBegin = i;
                i += m.vertexCount;
            }

            XMLElement * textureElement = modelElement -> FirstChildElement("texture");
            if (textureElement) {
                const char * textureName = textureElement -> Attribute("file");
                string texturePath = "../../Textures/";
                texturePath += textureName;

                if (textureIDs.find(textureName) != textureIDs.end()){
                    auto it = textureIDs.find(textureName);
                    m.textureID = it->second;
                } else {
                    m.textureID = loadTexture(texturePath);
                    textureIDs.insert({textureName, m.textureID});
                }
            }

            XMLElement * colorElement = modelElement -> FirstChildElement("color");
            if (colorElement) {
                m.color = parseColor(colorElement);
            }

            group.models.push_back(m);

            modelElement = modelElement -> NextSiblingElement("model");
        }
    }

    XMLElement * subGroupElement = groupElement -> FirstChildElement("group");
    while (subGroupElement) {
        group.subGroups.push_back(parseGroup(subGroupElement));
        subGroupElement = subGroupElement -> NextSiblingElement("group");
    }

    return group;
}


// XML parsing function
int parseXML(const char* filename, World *world, vector<float> *vert, vector<float> *norm, vector<float> *text) {
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

        XMLElement * lightsElement = worldElement -> FirstChildElement("lights");
        if (lightsElement) {
            (*world).lights = parseLights(lightsElement);
        }

        XMLElement *groupElement = worldElement -> FirstChildElement("group");
        while (groupElement) {
            (*world).groups.push_back(parseGroup(groupElement));
            groupElement = groupElement -> NextSiblingElement("group");
        }

        *vert = vertexPoints;
        *norm = normalPoints;
        *text = texturePoints;
    } else {
        return 1;
    }

    return 0;
}