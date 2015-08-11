#ifndef _VECTOR_H
#define _VECTOR_H

typedef struct {
	double x, y, z;
} Vector;

typedef struct {
	Vector v;
	double t;
} Vector4;

typedef struct {
	double r, theta, phi;
} PolarVector;

extern const Vector VECTOR_UNIT_X;
extern const Vector VECTOR_UNIT_Y;
extern const Vector VECTOR_UNIT_Z;

int vector_equals(Vector a, Vector b);
void vector_print(Vector v);
Vector vector_add(Vector u, Vector v);
Vector vector_sum(Vector* data, int len);
Vector vector_scale(Vector v, double k);
Vector vector_average(Vector* data, int len);
double vector_squareMag(Vector v);
double vector_mag(Vector v);
Vector vector_unit(Vector v);

double vector_dot(Vector u, Vector v);
Vector vector_cross(Vector u, Vector v);

PolarVector vector_toPolar(Vector v);
Vector vector_toCartesian(PolarVector v);
#endif
