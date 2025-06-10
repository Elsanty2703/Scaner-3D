#ifndef RAYTRACING_RAYTRACING_H
#define RAYTRACING_RAYTRACING_H

typedef struct {
    float x,y,z;
} VEC3_T;
typedef struct {
    VEC3_T o, d;
} RAY_T;
typedef struct {
    VEC3_T n;
    VEC3_T p1;
    VEC3_T p2;
    VEC3_T p3;
} TRI_T;

VEC3_T VEC3_add(VEC3_T a, VEC3_T b);
VEC3_T VEC3_sub(VEC3_T a, VEC3_T b);
float VEC3_dot(VEC3_T a, VEC3_T b);
VEC3_T VEC3_cross(VEC3_T a, VEC3_T b);
VEC3_T VEC3_normalize(VEC3_T a);
VEC3_T VEC3_scale(VEC3_T a, float b);
VEC3_T VEC3_zero();
float VEC3_lenght(VEC3_T a);
VEC3_T VEC3_set(float x, float y, float z);
VEC3_T VEC3_rotateX(VEC3_T a, float b, VEC3_T c);
VEC3_T VEC3_rotateY(VEC3_T a, float b, VEC3_T c);
VEC3_T VEC3_rotateZ(VEC3_T a, float b, VEC3_T c);
float IntersectionTriangle(RAY_T a, TRI_T b);
TRI_T* LlenarTriangles(const char* a, int* b);
TRI_T* LlenarTrianglesBinario(const char* a, int* b);
TRI_T* LlenarTrianglesASCII(const char* a, int* b);
float ShadingTriangle(float a, TRI_T d, VEC3_T e);
RAY_T CrearRayo(VEC3_T a, VEC3_T b, VEC3_T c, RAY_T d, int e, int f, int x, int y);

int RayTracing(const char* a, int b, int c, int d, int e);

#endif //RAYTRACING_RAYTRACING_H
