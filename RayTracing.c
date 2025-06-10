#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "RayTracing.h"

#define SKY 1e+30

int RayTracing( const char* nArchivo, int WIDTH, int HEIGHT, int FRAMES, int SCALE ){
    int NUM_TRIANGLES;

    // Se cargan los triángulos con los puntos
    TRI_T* triangles = LlenarTriangles(nArchivo, &NUM_TRIANGLES);
    printf("Cantidad de triangulos: %d\n", NUM_TRIANGLES);

    // Se cargan las luces desde archivo
    VEC3_T lights;

    // Se define el origen de la cámara y el punto donde quiero que mire
    VEC3_T origen = VEC3_set(0.0, 0, 0.6);
    VEC3_T foco = VEC3_set(0, 0, -0.1);
    VEC3_T techo = VEC3_set(1, 0, 0);
    // Se define el rayo que se lanzará desde el origen de la cámara
    RAY_T ray = { origen, VEC3_zero() };
    ray.o = VEC3_rotateX(ray.o, M_PI*2/FRAMES, foco);

    // Bucle de animación por cada frame
    for(int time = 0; time < FRAMES ; time++){
        // Se crea el nombre del archivo para el frame actual
        char filename[50];
        snprintf(filename, sizeof(filename), "Animacion/RAY_TRACING_%d.pgm", time);

        // Se abre el archivo para escritura
        FILE *file = fopen(filename, "w");
        if (!file) {
            perror("Error al abrir el archivo5");
            return 1;
        }

        // Se escribe cabecera del archivo PGM
        fprintf(file, "P2\n%d %d\n%d\n", WIDTH, HEIGHT, SCALE);

        if(time!=0) {
            ray.o = VEC3_rotateX(ray.o, M_PI*2/FRAMES, foco);
        }

        lights = VEC3_normalize(VEC3_sub(foco, ray.o));
        VEC3_T right = VEC3_normalize(VEC3_cross(lights, techo));
        VEC3_T up = VEC3_cross(right, lights);

        for (int y = 0; y < HEIGHT; y++) {
            for (int x = 0; x < WIDTH; x++) {
                // Dirección del rayo hacia el píxel
                RAY_T pixel_ray = CrearRayo(lights, right, up, ray, WIDTH, HEIGHT, x, y);

                // Ahora usamos pixel_ray en lugar de ray para todo lo que sigue
                float dst_triangle = SKY;

                int closest_triangle_idx = 0;
                if(NUM_TRIANGLES!=0){
                    for (int k = 0; k < NUM_TRIANGLES; k++) {
                        float d = IntersectionTriangle(pixel_ray, triangles[k]);
                        if (d < dst_triangle) {
                            dst_triangle = d;
                            closest_triangle_idx = k;
                        }
                    }
                }

                fprintf(file, "%d ", (int)(ShadingTriangle(dst_triangle, triangles[closest_triangle_idx], lights) * SCALE));
            }

            fprintf(file, "\n");
            printf("Porcentaje completado: %d%%\r", (int)((y + 1) * 100.0 / HEIGHT));
        }
        // Se cierra el archivo
        fclose(file);
    }
    // Se libera la memoria de las esferas y luces
    free(triangles);
    return 0;
}

float IntersectionTriangle(RAY_T ray, TRI_T tri) {
    // Vectores que definen el plano del triángulo
    VEC3_T v0 = VEC3_sub(tri.p1, tri.p2);
    VEC3_T v1 = VEC3_sub(tri.p1, tri.p3);

    // Calcula la normal al triángulo
    VEC3_T normal = VEC3_cross(v0, v1);

    // Calcula el denominador para la distancia usando producto punto
    float denom = VEC3_dot(ray.d, normal);

    // Calcula la distancia desde el origen del rayo al plano del triángulo
    float dst = -1 * VEC3_dot(VEC3_sub(ray.o, tri.p1), normal) / denom;

    // Calcula el punto de intersección
    VEC3_T intersection = VEC3_add(ray.o, VEC3_scale(ray.d, dst));

    // Cálculos para coordenadas baricéntricas
    VEC3_T v2 = VEC3_sub(tri.p1, intersection);
    float d00 = VEC3_dot(v0, v0);
    float d01 = VEC3_dot(v0, v1);
    float d11 = VEC3_dot(v1, v1);
    float d20 = VEC3_dot(v2, v0);
    float d21 = VEC3_dot(v2, v1);
    float denomBary = d00 * d11 - d01 * d01;

    // Calcula coordenadas baricéntricas u, v, w
    float v = (d11 * d20 - d01 * d21) / denomBary;
    float w = (d00 * d21 - d01 * d20) / denomBary;
    float u = 1.0f - v - w;

    // Verifica si el punto está dentro del triángulo y en la dirección del rayo
    return (u >= 0.0f && v >= 0.0f && w >= 0.0f) ? (dst > 0) ? dst : SKY : SKY;
}

float ShadingTriangle(float dst, TRI_T triangle, VEC3_T lights) {
    // Si no hay intersección, no hay luz
    if (dst == SKY)
        return 0.0;

    float diffuse = VEC3_dot(triangle.n, lights);

    return (diffuse < 0.0) ? -diffuse : diffuse;
}

RAY_T CrearRayo(VEC3_T dir, VEC3_T right, VEC3_T up, RAY_T ray, int WIDTH, int HEIGHT, int x, int y) {
VEC3_T pixel_dir = VEC3_add(
        VEC3_add(
                VEC3_scale(right, (x - WIDTH / 2.0f) / WIDTH),
                VEC3_scale(up, (y - HEIGHT / 2.0f) / HEIGHT)
        ),
        dir
);
pixel_dir = VEC3_normalize(pixel_dir);

// Creamos un rayo que parte del origen de la cámara con esa dirección
return (RAY_T){ ray.o, pixel_dir };
}

TRI_T* LlenarTriangles(const char* nombreArchivo, int* num_triangles) {
    FILE* archivo = fopen(nombreArchivo, "rb");
    if (!archivo) {
        perror("No se pudo abrir el archivo STL");
        *num_triangles = 0;
        return NULL;
    }

    char buffer[6] = {0};
    fread(buffer, 1, 5, archivo); // Leer las primeras 5 letras
    fclose(archivo);

    // Verifica si comienza con "solid" (ASCII)
    if (strncmp(buffer, "solid", 5) == 0) {
        return LlenarTrianglesASCII(nombreArchivo, num_triangles);
    } else {
        return LlenarTrianglesBinario(nombreArchivo, num_triangles);
    }
}

TRI_T* LlenarTrianglesBinario(const char* nombreArchivo, int* num_triangles) {
    FILE* archivo = fopen(nombreArchivo, "rb");
    if (!archivo) {
        perror("Error al abrir archivo STL binario");
        *num_triangles = 0;
        return NULL;
    }

    fseek(archivo, 80, SEEK_SET);
    unsigned int cantidad;
    fread(&cantidad, sizeof(unsigned int), 1, archivo);
    *num_triangles = (int)cantidad;

    TRI_T* triangles = (TRI_T*)malloc(cantidad * sizeof(TRI_T));
    if (!triangles) {
        perror("Error al asignar memoria binaria");
        fclose(archivo);
        *num_triangles = 0;
        return NULL;
    }

    for (int i = 0; i < *num_triangles; i++) {
        float normal[3], v1[3], v2[3], v3[3];
        unsigned short attr;

        fread(normal, sizeof(float), 3, archivo);
        fread(v1, sizeof(float), 3, archivo);
        fread(v2, sizeof(float), 3, archivo);
        fread(v3, sizeof(float), 3, archivo);
        fread(&attr, sizeof(unsigned short), 1, archivo); // Atributo ignorado

        triangles[i].n = VEC3_set(normal[0], normal[1], normal[2]);
        triangles[i].p1 = VEC3_set(v1[0], v1[1], v1[2]);
        triangles[i].p2 = VEC3_set(v2[0], v2[1], v2[2]);
        triangles[i].p3 = VEC3_set(v3[0], v3[1], v3[2]);
    }

    fclose(archivo);
    return triangles;
}

TRI_T* LlenarTrianglesASCII(const char* nombreArchivo, int* num_triangles) {
    FILE* archivo = fopen(nombreArchivo, "r");
    if (!archivo) {
        perror("Error al abrir archivo STL ASCII");
        *num_triangles = 0;
        return NULL;
    }

    char linea[256];
    int total_lineas = 0;

    while (fgets(linea, sizeof(linea), archivo)) {
        total_lineas++;
    }

    *num_triangles = (total_lineas - 2) / 7;

    TRI_T* triangles = (TRI_T*)malloc(*num_triangles * sizeof(TRI_T));
    if (!triangles) {
        perror("Error al asignar memoria");
        fclose(archivo);
        *num_triangles = 0;
        return NULL;
    }

    fclose(archivo);
    archivo = fopen(nombreArchivo, "r");
    if (!archivo) {
        perror("Error al reabrir archivo STL ASCII");
        free(triangles);
        *num_triangles = 0;
        return NULL;
    }

    int idx = 0;
    VEC3_T normal, p1, p2, p3;

    while (fgets(linea, sizeof(linea), archivo)) {
        if (sscanf(linea, " facet normal %f %f %f", &normal.x, &normal.y, &normal.z) == 3) {
            fgets(linea, sizeof(linea), archivo); // outer loop
            sscanf(fgets(linea, sizeof(linea), archivo), "  vertex %f %f %f", &p1.x, &p1.y, &p1.z);
            sscanf(fgets(linea, sizeof(linea), archivo), "  vertex %f %f %f", &p2.x, &p2.y, &p2.z);
            sscanf(fgets(linea, sizeof(linea), archivo), "  vertex %f %f %f", &p3.x, &p3.y, &p3.z);
            fgets(linea, sizeof(linea), archivo); // endloop
            fgets(linea, sizeof(linea), archivo); // endfacet

            triangles[idx].n = normal;
            triangles[idx].p1 = p1;
            triangles[idx].p2 = p2;
            triangles[idx].p3 = p3;
            idx++;
        }
    }

    fclose(archivo);
    return triangles;
}

VEC3_T VEC3_add(VEC3_T a, VEC3_T b){
    return (VEC3_T){a.x + b.x, a.y + b.y, a.z + b.z};
}

VEC3_T VEC3_sub(VEC3_T a, VEC3_T b){
    return (VEC3_T){a.x - b.x, a.y - b.y, a.z - b.z};
}

float VEC3_dot(VEC3_T a, VEC3_T b){
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

VEC3_T VEC3_cross(VEC3_T a, VEC3_T b){
    return (VEC3_T){a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z,a.x*b.y - a.y*b.x};
}

float VEC3_lenght(VEC3_T a){
    return sqrt(VEC3_dot(a,a));
}

VEC3_T VEC3_normalize(VEC3_T a){
    float len = VEC3_lenght(a);
    return (len > 0) ? VEC3_scale(a, 1/len) : VEC3_zero();
}

VEC3_T VEC3_scale(VEC3_T a, float b){
    return (VEC3_T){a.x*b, a.y*b, a.z*b};
}

VEC3_T VEC3_zero(){
    return (VEC3_T){0,0,0};
}

VEC3_T VEC3_set(float x, float y, float z){
    return (VEC3_T){x,y,z};
}

VEC3_T VEC3_rotateX(VEC3_T v, float angle, VEC3_T center) {
    // Paso 1: traslación al origen
    VEC3_T trasladado = {
            v.x - center.x,
            v.y - center.y,
            v.z - center.z
    };

    // Paso 2: rotación
    float cosA = cosf(angle);
    float sinA = sinf(angle);

    VEC3_T rotado = {
            trasladado.x,
            trasladado.y * cosA - trasladado.z * sinA,
            trasladado.y * sinA + trasladado.z * cosA
    };

    // Paso 3: traslación de regreso
    return (VEC3_T) {
            rotado.x + center.x,
            rotado.y + center.y,
            rotado.z + center.z
    };
}

VEC3_T VEC3_rotateY(VEC3_T v, float angle, VEC3_T center) {
    // Paso 1: traslación al origen
    VEC3_T trasladado = {
            v.x - center.x,
            v.y - center.y,
            v.z - center.z
    };

    // Paso 2: rotación alrededor del eje Y
    float cosA = cosf(angle);
    float sinA = sinf(angle);

    VEC3_T rotado = {
            trasladado.x * cosA + trasladado.z * sinA,
            trasladado.y,
            -trasladado.x * sinA + trasladado.z * cosA
    };

    // Paso 3: traslación de regreso
    return (VEC3_T) {
            rotado.x + center.x,
            rotado.y + center.y,
            rotado.z + center.z
    };
}

VEC3_T VEC3_rotateZ(VEC3_T v, float angle, VEC3_T center) {
    // Paso 1: traslación al origen
    VEC3_T trasladado = {
            v.x - center.x,
            v.y - center.y,
            v.z - center.z
    };

    // Paso 2: rotación alrededor del eje Z
    float cosA = cosf(angle);
    float sinA = sinf(angle);

    VEC3_T rotado = {
            trasladado.x * cosA - trasladado.y * sinA,
            trasladado.x * sinA + trasladado.y * cosA,
            trasladado.z
    };

    // Paso 3: traslación de regreso
    return (VEC3_T) {
            rotado.x + center.x,
            rotado.y + center.y,
            rotado.z + center.z
    };
}
