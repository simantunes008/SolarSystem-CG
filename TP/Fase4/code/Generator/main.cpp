#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <algorithm>

using namespace std;

// Structure to represent a point in 3D space
struct Point{
    float x;
    float y;
    float z;
    float nx;
    float ny;
    float nz;
    float tx;
    float tz;
};

void writePoint(Point point, ofstream& writer) {
    writer << point.x << ";" << point.y << ";" << point.z << ";" << point.nx << ";" << point.ny << ";" << point.nz << ";" << point.tx << ";" << point.tz << endl;
}

vector<vector<int>> patches;
vector<Point> controlPoints;


bool readFile(string file) {
    string line, number;
    int i = 0, numberPatch = 0;

    ifstream infile(file, ios::binary | ios::in);

    if (!infile) {
        cout << "Error opening patch file!\n";
        return false;
    }

    while (getline(infile, line, '\n')) {
        line.erase(std::remove_if(line.begin(), line.end(), [](char c) { return c == '\r'; }), line.end());

        if (i==0){
            numberPatch = stoi(line);
        }
        else if (i <= numberPatch){
            istringstream iss(line);
            vector<int> patch;
            while(getline(iss, number, ',')){
                patch.push_back(stoi(number));
            }
            patches.push_back(patch);
        }
        else if(i > numberPatch + 1){
            istringstream iss(line);
            int j = 0;
            float p[3];

            while (getline(iss, number, ',')){
                p[j] = stof(number);
                j++;
            }
            controlPoints.push_back({p[0], p[1], p[2]});
        }
        i++;
    }

    infile.close();
    return true;
}


void plane(float length, int divisions, string file) {
    string filePath = "../../Models/" + file;
    ofstream writer(filePath);

    writer << (divisions * divisions) * 6 << endl;

    float spacing = length / float(divisions);

    Point point1{}, point2{}, point3{}, point4{}, normal{};

    for (int i = 0; i < divisions; i++) {
        for (int j = 0; j < divisions; j++) {
            point1 = {(-length / 2) + float(i) * spacing, 0.0, (-length / 2) + float(j) * spacing};
            point2 = {(-length / 2) + float(i+1) * spacing, 0.0, (-length / 2) + float(j) * spacing};
            point3 = {(-length / 2) + float(i) * spacing, 0.0, (-length / 2) + float(j+1) * spacing};
            point4 = {(-length / 2) + float(i+1) * spacing, 0.0, (-length / 2) + float(j+1) * spacing};

            point1.nx = 0; point1.ny = 1; point1.nz = 0;
            point2.nx = 0; point2.ny = 1; point2.nz = 0;
            point3.nx = 0; point3.ny = 1; point3.nz = 0;
            point4.nx = 0; point4.ny = 1; point4.nz = 0;

            point1.tx = float(i) / divisions; point1.tz = float(j) / divisions;
            point2.tx = float(i+1) / divisions; point2.tz = float(j) / divisions;
            point3.tx = float(i) / divisions; point3.tz = float(j+1) / divisions;
            point4.tx = float(i+1) / divisions; point4.tz = float(j+1) / divisions;

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

    Point point1{}, point2{}, point3{}, point4{}, normal{};

    // Top
    for (int i = 0; i < divisions; i++) {
        for (int j = 0; j < divisions; j++) {
            point1 = {(-length / 2) + float(i) * spacing, (length / 2), (-length / 2) + float(j) * spacing, 0, 1, 0, float(i) / divisions, float(j) / divisions};
            point2 = {(-length / 2) + float(i+1) * spacing, (length / 2), (-length / 2) + float(j) * spacing, 0, 1, 0, float(i+1) / divisions, float(j) / divisions};
            point3 = {(-length / 2) + float(i) * spacing, (length / 2), (-length / 2) + float(j+1) * spacing, 0, 1, 0, float(i) / divisions, float(j+1) / divisions};
            point4 = {(-length / 2) + float(i+1) * spacing, (length / 2), (-length / 2) + float(j+1) * spacing, 0, 1, 0, float(i+1) / divisions, float(j+1) / divisions};

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
            point1 = {(-length / 2) + float(i) * spacing, (-length / 2), (-length / 2) + float(j) * spacing, 0, -1, 0, float(i) / divisions, float(j) / divisions};
            point2 = {(-length / 2) + float(i+1) * spacing, (-length / 2), (-length / 2) + float(j) * spacing, 0, -1, 0, float(i+1) / divisions, float(j) / divisions};
            point3 = {(-length / 2) + float(i) * spacing, (-length / 2), (-length / 2) + float(j+1) * spacing, 0, -1, 0, float(i) / divisions, float(j+1) / divisions};
            point4 = {(-length / 2) + float(i+1) * spacing, (-length / 2), (-length / 2) + float(j+1) * spacing, 0, -1, 0, float(i+1) / divisions, float(j+1) / divisions};

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
            point1 = {(length / 2), (-length / 2) + float(i) * spacing, (-length / 2) + float(j) * spacing, 1, 0, 0, float(i) / divisions, float(j) / divisions};
            point2 = {(length / 2), (-length / 2) + float(i+1) * spacing, (-length / 2) + float(j) * spacing, 1, 0, 0, float(i+1) / divisions, float(j) / divisions};
            point3 = {(length / 2), (-length / 2) + float(i) * spacing, (-length / 2) + float(j+1) * spacing, 1, 0, 0, float(i) / divisions, float(j+1) / divisions};
            point4 = {(length / 2), (-length / 2) + float(i+1) * spacing, (-length / 2) + float(j+1) * spacing, 1, 0, 0, float(i+1) / divisions, float(j+1) / divisions};

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
            point1 = {(-length / 2), (-length / 2) + float(i) * spacing, (-length / 2) + float(j) * spacing, -1, 0, 0, float(i) / divisions, float(j) / divisions};
            point2 = {(-length / 2), (-length / 2) + float(i+1) * spacing, (-length / 2) + float(j) * spacing, -1, 0, 0, float(i+1) / divisions, float(j) / divisions};
            point3 = {(-length / 2), (-length / 2) + float(i) * spacing, (-length / 2) + float(j+1) * spacing, -1, 0, 0, float(i) / divisions, float(j+1) / divisions};
            point4 = {(-length / 2), (-length / 2) + float(i+1) * spacing, (-length / 2) + float(j+1) * spacing, -1, 0, 0, float(i+1) / divisions, float(j+1) / divisions};

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
            point1 = {(-length / 2) + float(i) * spacing, (-length / 2) + float(j) * spacing, (length / 2), 0, 0, 1, float(i) / divisions, float(j) / divisions};
            point2 = {(-length / 2) + float(i+1) * spacing, (-length / 2) + float(j) * spacing, (length / 2), 0, 0, 1, float(i+1) / divisions, float(j) / divisions};
            point3 = {(-length / 2) + float(i) * spacing, (-length / 2) + float(j+1) * spacing, (length / 2), 0, 0, 1, float(i) / divisions, float(j+1) / divisions};
            point4 = {(-length / 2) + float(i+1) * spacing, (-length / 2) + float(j+1) * spacing, (length / 2), 0, 0, 1, float(i+1) / divisions, float(j+1) / divisions};

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
            point1 = {(-length / 2) + float(i) * spacing, (-length / 2) + float(j) * spacing, (-length / 2), 0, 0, -1, float(i) / divisions, float(j) / divisions};
            point2 = {(-length / 2) + float(i+1) * spacing, (-length / 2) + float(j) * spacing, (-length / 2), 0, 0, -1, float(i+1) / divisions, float(j) / divisions};
            point3 = {(-length / 2) + float(i) * spacing, (-length / 2) + float(j+1) * spacing, (-length / 2), 0, 0, -1, float(i) / divisions, float(j+1) / divisions};
            point4 = {(-length / 2) + float(i+1) * spacing, (-length / 2) + float(j+1) * spacing, (-length / 2), 0, 0, -1, float(i+1) / divisions, float(j+1) / divisions};

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

        point1 = {0, 0, 0, 0, -1, 0, 0.5, 0.5};
        point2 = {radius * sin(alpha1), 0, radius * cos(alpha1), 0, -1, 0, 0.5f + 0.5f * sin(alpha1), 0.5f + 0.5f * cos(alpha1)};
        point3 = {radius * sin(alpha2), 0, radius * cos(alpha2), 0, -1, 0, 0.5f + 0.5f * sin(alpha2), 0.5f + 0.5f * cos(alpha2)};

        writePoint(point3, writer);
        writePoint(point2, writer);
        writePoint(point1, writer);
    }

    // Top
    for (int i = 0; i < stacks; i++) {
        for (int j = 0; j < slices; j++) {
            float alpha = (2 * M_PI / slices) * j;
            float alpha2 = (2 * M_PI / slices) * (j + 1);

            float stackRadius = ((radius * (height - (i * (height / stacks)))) / height);
            float stackRadius2 = ((radius * (height - ((i + 1) * (height / stacks)))) / height);

            float nx1 = sin(alpha);
            float nz1 = cos(alpha);
            float ny1 = radius / sqrt(radius * radius + height * height);
            
            float nx2 = sin(alpha2);
            float nz2 = cos(alpha2);
            float ny2 = radius / sqrt(radius * radius + height * height);

            point1 = {stackRadius * sin(alpha), i * (height / stacks), stackRadius * cos(alpha), nx1, ny1, nz1, (float)j / slices, (float)i / stacks};
            point2 = {stackRadius2 * sin(alpha), (i + 1) * (height / stacks), stackRadius2 * cos(alpha), nx1, ny1, nz1, (float)j / slices, (float)(i + 1) / stacks};
            point3 = {stackRadius2 * sin(alpha2), (i + 1) * (height / stacks), stackRadius2 * cos(alpha2), nx2, ny2, nz2, (float)(j + 1) / slices, (float)(i + 1) / stacks};
            point4 = {stackRadius * sin(alpha2), i * (height / stacks), stackRadius * cos(alpha2), nx2, ny2, nz2, (float)(j + 1) / slices, (float)i / stacks};

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

            point1 = {radius * sin(beta1) * cos(alpha1), radius * cos(beta1), radius * sin(beta1) * sin(alpha1), sin(beta1) * cos(alpha1), cos(beta1), sin(beta1) * sin(alpha1), 1.0f - (float)i / slices, 1.0f - (float)j / stacks};
            point2 = {radius * sin(beta2) * cos(alpha1), radius * cos(beta2), radius * sin(beta2) * sin(alpha1), sin(beta2) * cos(alpha1), cos(beta2), sin(beta2) * sin(alpha1), 1.0f - (float)i / slices, 1.0f - (float)(j + 1) / stacks};
            point3 = {radius * sin(beta2) * cos(alpha2), radius * cos(beta2), radius * sin(beta2) * sin(alpha2), sin(beta2) * cos(alpha2), cos(beta2), sin(beta2) * sin(alpha2), 1.0f - (float)(i + 1) / slices, 1.0f - (float)(j + 1) / stacks};
            point4 = {radius * sin(beta1) * cos(alpha2), radius * cos(beta1), radius * sin(beta1) * sin(alpha2), sin(beta1) * cos(alpha2), cos(beta1), sin(beta1) * sin(alpha2), 1.0f - (float)(i + 1) / slices, 1.0f - (float)j / stacks};

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

            point1 = {(outerRadius + innerRadius * cos(beta1)) * cos(alpha1), innerRadius * sin(beta1), (outerRadius + innerRadius * cos(beta1)) * sin(alpha1), cos(beta1) * cos(alpha1), sin(beta1), cos(beta1) * sin(alpha1), (float)j / stacks, 1.0f - (float)i / slices};
            point2 = {(outerRadius + innerRadius * cos(beta2)) * cos(alpha1), innerRadius * sin(beta2), (outerRadius + innerRadius * cos(beta2)) * sin(alpha1), cos(beta2) * cos(alpha1), sin(beta2), cos(beta2) * sin(alpha1), (float)(j + 1) / stacks, 1.0f - (float)i / slices};
            point3 = {(outerRadius + innerRadius * cos(beta2)) * cos(alpha2), innerRadius * sin(beta2), (outerRadius + innerRadius * cos(beta2)) * sin(alpha2), cos(beta2) * cos(alpha2), sin(beta2), cos(beta2) * sin(alpha2), (float)(j + 1) / stacks, 1.0f - (float)(i + 1) / slices};
            point4 = {(outerRadius + innerRadius * cos(beta1)) * cos(alpha2), innerRadius * sin(beta1), (outerRadius + innerRadius * cos(beta1)) * sin(alpha2), cos(beta1) * cos(alpha2), sin(beta1), cos(beta1) * sin(alpha2), (float)j / stacks, 1.0f - (float)(i + 1) / slices};

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


Point multMatrixVector(float u[4], vector<vector<Point>> matrix, float v[4]){
    // aux = U * M
    vector<Point> aux(4);
    for (int i = 0; i < 4; ++i) {
        aux[i] = {0,0,0};
    }

    for (int j = 0; j < 4; ++j) {
        for (int k = 0; k < 4; ++k) {

            Point p = matrix[k][j];
            float x = p.x;
            float y = p.y;
            float z = p.z;
            p = {x*u[k], y*u[k], z*u[k]};

            Point p_aux = aux[j];
            aux[j] = {p.x+p_aux.x, p.y+p_aux.y, p.z+p_aux.z};
        }
    }

    // final = aux * V
    Point final = {0,0,0};

    for(int j =0; j<4;++j){
        final.x = final.x + (aux[j].x * v[j]);
        final.y = final.y + (aux[j].y * v[j]);
        final.z = final.z + (aux[j].z * v[j]);
    }
    return final;
}


vector<vector<Point>> matrixMultiplication(Point p[4][4], float m[4][4]){
    // res = M * P
    vector<vector<Point>> res(4,vector<Point>(4));
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            res[i][j] = {0,0,0};
        }
    }

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            for (int k = 0; k < 4; ++k) {
                Point point = p[k][j];
                float x = point.x;
                float y = point.y;
                float z = point.z;
                point = {x*m[i][k], y*m[i][k], z*m[i][k]};

                Point aux = res[i][j];
                res[i][j] = {point.x + aux.x, point.y + aux.y, point.z + aux.z};
            }
        }
    }

    // result = res * Mt
    vector<vector<Point>> result(4,vector<Point>(4));
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result[i][j] = {0,0,0};
        }
    }

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            for (int k = 0; k < 4; ++k) {
                Point point = res[i][k];
                float x = point.x;
                float y = point.y;
                float z = point.z;
                point = {x*m[k][j], y*m[k][j], z*m[k][j]};

                Point aux = result[i][j];
                result[i][j] = {point.x + aux.x, point.y + aux.y, point.z + aux.z};
            }
        }
    }
    return result;
}


vector<vector<Point>> calcBezier(Point p[4][4], int tessellation){
    float pu=0, pv=0;
    vector<vector<Point>> grid(tessellation+1, vector<Point>(tessellation+1));
    for (int j = 0; j <= tessellation; j++) {
        for (int k = 0; k <= tessellation; k++) {
            grid[j][k] = {0,0,0};
        }
    }
    float m[4][4] = {{-1, 3,  -3, 1},
                     {3, -6, 3, 0},
                     {-3, 3, 0, 0},
                     {1, 0, 0, 0}};

    vector<vector<Point>> matrix = matrixMultiplication(p, m);

    float step = 1.f/(float)tessellation;
    for (int i = 0; i <= tessellation; i++) {
        pu = (float)i*step;
        float u_vector[4] = {(float)pow(pu,3), (float)pow(pu,2),pu,1};
        
        for (int j = 0; j <= tessellation; j++) {
            pv = (float)j*step;
           
            float v_vector[4] = {(float)pow(pv,3), (float)pow(pv,2),pv,1};
            Point point = multMatrixVector(u_vector, matrix, v_vector);
            point.tx = pu; 
            point.tz = pv; 

            // Calculate the partial derivatives
            float u_derivative[4] = {3*(float)pow(pu,2), 2*pu, 1, 0};
            float v_derivative[4] = {3*(float)pow(pv,2), 2*pv, 1, 0};

            Point du = multMatrixVector(u_derivative, matrix, v_vector);
            Point dv = multMatrixVector(u_vector, matrix, v_derivative);

            // Calculate the cross product of the partial derivatives
            Point normal = {du.y*dv.z - du.z*dv.y, du.z*dv.x - du.x*dv.z, du.x*dv.y - du.y*dv.x};

            // Normalize the normal
            float length = sqrt(normal.x*normal.x + normal.y*normal.y + normal.z*normal.z);
            point.nx = normal.x / length;
            point.ny = normal.y / length;
            point.nz = normal.z / length;

            grid[i][j] = point;
        }
    }

    return grid;
}


void bezier(string inFile, int tessellation, string outFile) {
    if (tessellation < 0) return;

    if (!readFile(inFile)) return;

    ofstream outfile(outFile);
    if (!outfile) {
        cout << "Error opening output file!\n";
        return;
    }

    //número total de pontos
    int totalPoints = 0;
    for (int i = 0; i < patches.size(); i++) {
        totalPoints += tessellation * tessellation * 2 * 3; // Cada patch tem tessellation*tessellation * 2 triângulos com 3 pontos cada
    }

    // Escreve o número total de pontos no início do arquivo
    outfile << totalPoints << "\n";

    Point m[4][4] = {0};
    for (int i = 0; i < patches.size(); i++) {
        for (int column = 0; column < 4; column++) {
            for (int row = 0; row < 4; row++) {
                m[row][column] = controlPoints[patches[i][row + column * 4]];
            }
        }

        vector<vector<Point>> gridPoints = calcBezier(m, tessellation);
        for (int j = 0; j < tessellation; j++) {
            for (int k = 0; k < tessellation; k++) {
                writePoint(gridPoints[j][k], outfile);
                writePoint(gridPoints[j+1][k], outfile);
                writePoint(gridPoints[j][k+1], outfile);

                writePoint(gridPoints[j+1][k], outfile);
                writePoint(gridPoints[j+1][k+1], outfile);
                writePoint(gridPoints[j][k+1], outfile);
            }
        }
    }

    outfile.close();
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
    if (primitive == "bezier") {
        string infile = argv[2];
        int tesselation = stoi(argv[3]);
        string outfile = argv[4];
        bezier(infile, tesselation, outfile);
    }

    return 0;
}
