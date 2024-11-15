#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>
#include <fstream>

using namespace std;

// Structure to represent a point in 3D space
struct Point {
    float x;
    float y;
    float z;
};

void writePoint(Point point, ofstream& writer) {
    writer << point.x << ";" << point.y << ";" << point.z << endl;
}

void plane(float length, int divisions, string file) {
    string filePath = "../../Models/" + file;
    ofstream writer(filePath);

    writer << (divisions * divisions) * 6 << endl;

    float spacing = length / float(divisions);

    Point point1{}, point2{}, point3{}, point4{};

    for (int i = 0; i < divisions; i++) {
        for (int j = 0; j < divisions; j++) {
            point1 = {(-length / 2) + float(i) * spacing, 0.0, (-length / 2) + float(j) * spacing};
            point2 = {(-length / 2) + float(i+1) * spacing, 0.0, (-length / 2) + float(j) * spacing};
            point3 = {(-length / 2) + float(i) * spacing, 0.0, (-length / 2) + float(j+1) * spacing};
            point4 = {(-length / 2) + float(i+1) * spacing, 0.0, (-length / 2) + float(j+1) * spacing};

            writePoint(point1, writer);
            writePoint(point3, writer);
            writePoint(point2, writer);
            writePoint(point4, writer);
            writePoint(point2, writer);
            writePoint(point3, writer);
        }
    }

    writer.close();
}


void box(float length, int divisions, string file) {
    string filePath = "../../Models/" + file;
    ofstream writer(filePath);

    writer << ((divisions * divisions) * 6) * 6 << endl;

    float spacing = length / float(divisions);

    Point point1{}, point2{}, point3{}, point4{};

    // Top
    for (int i = 0; i < divisions; i++) {
        for (int j = 0; j < divisions; j++) {
            point1 = {(-length / 2) + float(i) * spacing, (length / 2), (-length / 2) + float(j) * spacing};
            point2 = {(-length / 2) + float(i+1) * spacing, (length / 2), (-length / 2) + float(j) * spacing};
            point3 = {(-length / 2) + float(i) * spacing, (length / 2), (-length / 2) + float(j+1) * spacing};
            point4 = {(-length / 2) + float(i+1) * spacing, (length / 2), (-length / 2) + float(j+1) * spacing};

            writePoint(point1, writer);
            writePoint(point3, writer);
            writePoint(point2, writer);
            writePoint(point4, writer);
            writePoint(point2, writer);
            writePoint(point3, writer);
        }
    }

    // Bottom
    for (int i = 0; i < divisions; i++) {
        for (int j = 0; j < divisions; j++) {
            point1 = {(-length / 2) + float(i) * spacing, (-length / 2), (-length / 2) + float(j) * spacing};
            point2 = {(-length / 2) + float(i+1) * spacing, (-length / 2), (-length / 2) + float(j) * spacing};
            point3 = {(-length / 2) + float(i) * spacing, (-length / 2), (-length / 2) + float(j+1) * spacing};
            point4 = {(-length / 2) + float(i+1) * spacing, (-length / 2), (-length / 2) + float(j+1) * spacing};

            writePoint(point1, writer);
            writePoint(point2, writer);
            writePoint(point3, writer);
            writePoint(point3, writer);
            writePoint(point2, writer);
            writePoint(point4, writer);
        }
    }
    
    // Left side
    for (int i = 0; i < divisions; i++) {
        for (int j = 0; j < divisions; j++) {
            point1 = {(length / 2), (-length / 2) + float(i) * spacing, (-length / 2) + float(j) * spacing};
            point2 = {(length / 2), (-length / 2) + float(i+1) * spacing, (-length / 2) + float(j) * spacing};
            point3 = {(length / 2), (-length / 2) + float(i) * spacing, (-length / 2) + float(j+1) * spacing};
            point4 = {(length / 2), (-length / 2) + float(i+1) * spacing, (-length / 2) + float(j+1) * spacing};

            writePoint(point1, writer);
            writePoint(point2, writer);
            writePoint(point3, writer);
            writePoint(point3, writer);
            writePoint(point2, writer);
            writePoint(point4, writer);
        }
    }

    // Right side
    for (int i = 0; i < divisions; i++) {
        for (int j = 0; j < divisions; j++) {
            point1 = {(-length / 2), (-length / 2) + float(i) * spacing, (-length / 2) + float(j) * spacing};
            point2 = {(-length / 2), (-length / 2) + float(i+1) * spacing, (-length / 2) + float(j) * spacing};
            point3 = {(-length / 2), (-length / 2) + float(i) * spacing, (-length / 2) + float(j+1) * spacing};
            point4 = {(-length / 2), (-length / 2) + float(i+1) * spacing, (-length / 2) + float(j+1) * spacing};

            writePoint(point1, writer);
            writePoint(point3, writer);
            writePoint(point2, writer);
            writePoint(point2, writer);
            writePoint(point3, writer);
            writePoint(point4, writer);
        }
    }

    // Front
    for (int i = 0; i < divisions; i++) {
        for (int j = 0; j < divisions; j++) {
            point1 = {(-length / 2) + float(i) * spacing, (-length / 2) + float(j) * spacing, (length / 2)};
            point2 = {(-length / 2) + float(i+1) * spacing, (-length / 2) + float(j) * spacing, (length / 2)};
            point3 = {(-length / 2) + float(i) * spacing, (-length / 2) + float(j+1) * spacing, (length / 2)};
            point4 = {(-length / 2) + float(i+1) * spacing, (-length / 2) + float(j+1) * spacing, (length / 2)};

            writePoint(point1, writer);
            writePoint(point2, writer);
            writePoint(point3, writer);
            writePoint(point2, writer);
            writePoint(point4, writer);
            writePoint(point3, writer);
        }
    }

    // Back
    for (int i = 0; i < divisions; i++) {
        for (int j = 0; j < divisions; j++) {
            point1 = {(-length / 2) + float(i) * spacing, (-length / 2) + float(j) * spacing, (-length / 2)};
            point2 = {(-length / 2) + float(i+1) * spacing, (-length / 2) + float(j) * spacing, (-length / 2)};
            point3 = {(-length / 2) + float(i) * spacing, (-length / 2) + float(j+1) * spacing, (-length / 2)};
            point4 = {(-length / 2) + float(i+1) * spacing, (-length / 2) + float(j+1) * spacing, (-length / 2)};

            writePoint(point1, writer);
            writePoint(point3, writer);
            writePoint(point2, writer);
            writePoint(point4, writer);
            writePoint(point2, writer);
            writePoint(point3, writer);
        }
    }

    writer.close();
}

void cone(float radius, float height, int slices, int stacks, string file) {
    string filePath = "../../Models/" + file;
    ofstream writer(filePath);

    writer << (slices * 3) + ((stacks * 6) * slices) << endl;

    Point point1{}, point2{}, point3{}, point4{};

    // Base
    for (int i = 0; i < slices; i++) {
        float alpha1 = ((2.0 * M_PI) / slices) * i;
        float alpha2 = ((2.0 * M_PI) / slices) * (i + 1);

        point1 = {0, 0, 0};
        point2 = {radius * sin(alpha1), 0, radius * cos(alpha1)};
        point3 = {radius * sin(alpha2), 0, radius * cos(alpha2)};

        writePoint(point3, writer);
        writePoint(point2, writer);
        writePoint(point1, writer);
    }

    // Top
    for (int i = 0; i < stacks; i++) {
        for (int j = 0; j < slices; j++) {
            float alpha = (2 * M_PI / slices) * j;
            float alpha2 = (2 * M_PI / slices) * (j + 1);

            float currentRadius = radius * (height - (i * (height / stacks))) / height;
            float nextRadius = radius * (height - ((i + 1) * (height / stacks))) / height;

            point1 = {currentRadius * sin(alpha), i * (height / stacks), currentRadius * cos(alpha)};
            point2 = {nextRadius * sin(alpha), (i + 1) * (height / stacks), nextRadius * cos(alpha)};
            point3 = {nextRadius * sin(alpha2), (i + 1) * (height / stacks), nextRadius * cos(alpha2)};
            point4 = {currentRadius * sin(alpha2), i * (height / stacks), currentRadius * cos(alpha2)};

            writePoint(point1, writer);
            writePoint(point4, writer);
            writePoint(point3, writer);
            writePoint(point3, writer);
            writePoint(point2, writer);
            writePoint(point1, writer);
        }
    }

    writer.close();
}

void sphere(float radius, int slices, int stacks, string file) {
    string filePath = "../../Models/" + file;
    ofstream writer(filePath);

    writer << slices * stacks * 6 << endl;

    Point point1{}, point2{}, point3{}, point4{};

    for (int i = 0; i < slices; i++) {
        float alpha1 = ((2.0 * M_PI) / slices) * i;
        float alpha2 = ((2.0 * M_PI) / slices) * (i + 1);

        for (int j = 0; j < stacks; j++) {
            float beta1 = (M_PI / stacks) * j;
            float beta2 = (M_PI / stacks) * (j + 1);

            point1 = {radius * sin(beta1) * cos(alpha1), radius * cos(beta1), radius * sin(beta1) * sin(alpha1)};
            point2 = {radius * sin(beta2) * cos(alpha1), radius * cos(beta2), radius * sin(beta2) * sin(alpha1)};
            point3 = {radius * sin(beta2) * cos(alpha2), radius * cos(beta2), radius * sin(beta2) * sin(alpha2)};
            point4 = {radius * sin(beta1) * cos(alpha2), radius * cos(beta1), radius * sin(beta1) * sin(alpha2)};

            writePoint(point2, writer);
            writePoint(point1, writer);
            writePoint(point3, writer);
            writePoint(point4, writer);
            writePoint(point3, writer);
            writePoint(point1, writer);
        }
    }

    writer.close();
}

void torus(float innerRadius, float outerRadius, int slices, int stacks, string file) {
    string filePath = "../../Models/" + file;
    ofstream writer(filePath);

    writer << slices * stacks * 6 << endl;

    Point point1{}, point2{}, point3{}, point4{};

    outerRadius += innerRadius;
    for (int i = 0; i < slices; i++) {
        float alpha1 = ((2.0 * M_PI) / slices) * i;
        float alpha2 = ((2.0 * M_PI) / slices) * (i + 1);

        for (int j = 0; j < stacks; j++) {
            float beta1 = ((2.0 * M_PI) / stacks) * j;
            float beta2 = ((2.0 * M_PI) / stacks) * (j + 1);

            point1 = {(outerRadius + innerRadius * cos(beta1)) * cos(alpha1), innerRadius * sin(beta1), (outerRadius + innerRadius * cos(beta1)) * sin(alpha1)};
            point2 = {(outerRadius + innerRadius * cos(beta2)) * cos(alpha1), innerRadius * sin(beta2), (outerRadius + innerRadius * cos(beta2)) * sin(alpha1)};
            point3 = {(outerRadius + innerRadius * cos(beta2)) * cos(alpha2), innerRadius * sin(beta2), (outerRadius + innerRadius * cos(beta2)) * sin(alpha2)};
            point4 = {(outerRadius + innerRadius * cos(beta1)) * cos(alpha2), innerRadius * sin(beta1), (outerRadius + innerRadius * cos(beta1)) * sin(alpha2)};

            writePoint(point1, writer);
            writePoint(point2, writer);
            writePoint(point3, writer);
            writePoint(point3, writer);
            writePoint(point4, writer); 
            writePoint(point1, writer);
        }
    }

    writer.close();
}

int main(int argc, char ** argv) {
    string primitive = argv[1];

    if (primitive == "plane") {
        float lenght = stof(argv[2]);
        int divisions = stoi(argv[3]);
        string file = argv[4];
        plane(lenght, divisions, file);
    }
    if (primitive == "box") {
        float length = stof(argv[2]);
        int divisions = stoi(argv[3]);
        string file = argv[4];
        box(length, divisions, file);
    }
    if (primitive == "cone") {
        float radius = stof(argv[2]);
        float height = stof(argv[3]);
        int slices = stoi(argv[4]);
        int stacks = stoi(argv[5]);
        string file = argv[6];
        cone(radius, height, slices, stacks, file);
    }
    if (primitive == "sphere") {
        float radius = stof(argv[2]);
        int slices = stoi(argv[3]);
        int stacks = stoi(argv[4]);
        string file = argv[5];
        sphere(radius, slices, stacks, file);
    }
    if (primitive == "torus") {
        float inerRadius = stof(argv[2]);
        float outerRadius = stof(argv[3]);
        int slices = stoi(argv[4]);
        int stacks = stoi(argv[5]);
        string file = argv[6];
        torus(inerRadius, outerRadius, slices, stacks, file);
    }

    return 0;
}
