#include "catmull-rom.h"

void buildRotMatrix(float *x, float *y, float *z, float *m) {

	m[0] = x[0]; m[1] = x[1]; m[2] = x[2]; m[3] = 0;
	m[4] = y[0]; m[5] = y[1]; m[6] = y[2]; m[7] = 0;
	m[8] = z[0]; m[9] = z[1]; m[10] = z[2]; m[11] = 0;
	m[12] = 0; m[13] = 0; m[14] = 0; m[15] = 1;
}


void cross(float *a, float *b, float *res) {

	res[0] = a[1]*b[2] - a[2]*b[1];
	res[1] = a[2]*b[0] - a[0]*b[2];
	res[2] = a[0]*b[1] - a[1]*b[0];
}


void normalize(float *a) {

	float l = sqrt(a[0]*a[0] + a[1] * a[1] + a[2] * a[2]);
	a[0] = a[0]/l;
	a[1] = a[1]/l;
	a[2] = a[2]/l;
}


float length(float *v) {

	float res = sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
	return res;

}

void multMatrixVector(float *m, float *v, float *res) {

	for (int j = 0; j < 4; ++j) {
		res[j] = 0;
		for (int k = 0; k < 4; ++k) {
			res[j] += v[k] * m[j * 4 + k];
		}
	}

}


void getCatmullRomPoint(float t, float *p0, float *p1, float *p2, float *p3, float *pos, float *deriv) {

	// catmull-rom matrix
	float m[4][4] = {	{-0.5f,  1.5f, -1.5f,  0.5f},
						{ 1.0f, -2.5f,  2.0f, -0.5f},
						{-0.5f,  0.0f,  0.5f,  0.0f},
						{ 0.0f,  1.0f,  0.0f,  0.0f}};
			
	// Compute A = M * P
	float a[3][4];

		float p[3][4] = {	{p0[0], p1[0], p2[0], p3[0]},
							{p0[1], p1[1], p2[1], p3[1]},
							{p0[2], p1[2], p2[2], p3[2]}};

	for (int i = 0; i < 4; i++){
        multMatrixVector(&m[0][0], p[i], a[i]);
    }
	
	// Compute pos = T * A
	float tVector1[4] = {powf(t, 3), powf(t, 2), t, 1};
	for (int i = 0; i < 3; i++) {
		pos[i] = a[i][0] * tVector1[0] + a[i][1] * tVector1[1] + a[i][2] * tVector1[2] + a[i][3] * tVector1[3];
	}
	
	// compute deriv = T' * A
	float tVector2[4] = {3 * powf(t, 2), 2 * t, 1, 0};
	for (int i = 0; i < 3; i++) {
        deriv[i] = a[i][0] * tVector2[0] + a[i][1] * tVector2[1] + a[i][2] * tVector2[2] + a[i][3] * tVector2[3];
	}

	// ...
}


// given  global t, returns the point in the curve
void getGlobalCatmullRomPoint(float gt, float *pos, float *deriv, vector<Point> p) {

    int point_count = p.size();
	float t = gt * point_count; // this is the real global t
	int index = floor(t);  // which segment
	t = t - index; // where within  the segment

	// indices store the points
	int indices[4]; 
	indices[0] = (index + point_count-1)%point_count;	
	indices[1] = (indices[0]+1)%point_count;
	indices[2] = (indices[1]+1)%point_count; 
	indices[3] = (indices[2]+1)%point_count;

	float p0[3] = {p[indices[0]].x, p[indices[0]].y, p[indices[0]].z};
    float p1[3] = {p[indices[1]].x, p[indices[1]].y, p[indices[1]].z};
    float p2[3] = {p[indices[2]].x, p[indices[2]].y, p[indices[2]].z};
    float p3[3] = {p[indices[3]].x, p[indices[3]].y, p[indices[3]].z};
    getCatmullRomPoint(t, p0, p1, p2, p3, pos, deriv);
}

void renderCatmullRomCurve(vector<Point> p) {

// draw curve using line segments with GL_LINE_LOOP
	float pos[4];
    float deriv[4];

    glBegin(GL_LINE_LOOP);
    for(int i = 0; i < 1000; i++){
        getGlobalCatmullRomPoint(i/1000.0, pos, deriv, p);
        glVertex3f(pos[0], pos[1], pos[2]);
    }
    glEnd();
}