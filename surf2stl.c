// surf2stl.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void cross_product(float *v1, float *v2, float *result) {
    result[0] = v1[1]*v2[2] - v1[2]*v2[1];
    result[1] = v1[2]*v2[0] - v1[0]*v2[2];
    result[2] = v1[0]*v2[1] - v1[1]*v2[0];
}

void normalize(float *v) {
    float norm = sqrtf(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
    if (norm == 0) return;
    v[0] /= norm; v[1] /= norm; v[2] /= norm;
}

void write_facet(FILE *f, float *p1, float *p2, float *p3, const char *mode) {
    float v1[3], v2[3], normal[3];

    for (int i = 0; i < 3; ++i) {
        v1[i] = p2[i] - p1[i];
        v2[i] = p3[i] - p1[i];
    }

    cross_product(v1, v2, normal);
    normalize(normal);

    if (strcmp(mode, "ascii") == 0) {
        fprintf(f, "facet normal %.7E %.7E %.7E\n", normal[0], normal[1], normal[2]);
        fprintf(f, "  outer loop\n");
        fprintf(f, "    vertex %.7E %.7E %.7E\n", p1[0], p1[1], p1[2]);
        fprintf(f, "    vertex %.7E %.7E %.7E\n", p2[0], p2[1], p2[2]);
        fprintf(f, "    vertex %.7E %.7E %.7E\n", p3[0], p3[1], p3[2]);
        fprintf(f, "  endloop\nendfacet\n");
    } else {
        fwrite(normal, sizeof(float), 3, f);
        fwrite(p1, sizeof(float), 3, f);
        fwrite(p2, sizeof(float), 3, f);
        fwrite(p3, sizeof(float), 3, f);
        unsigned short attr = 0;
        fwrite(&attr, sizeof(unsigned short), 1, f);
    }
}

void surf2stl(const char *filename, float **x, float **y, float **z, int rows, int cols, const char *mode) {
    FILE *f;
    int i, j, nfacets = 0;

    if (strcmp(mode, "ascii") == 0)
        f = fopen(filename, "w");
    else
        f = fopen(filename, "wb");

    if (!f) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    if (strcmp(mode, "ascii") == 0) {
        fprintf(f, "solid ascii\n");
    } else {
        char header[80] = "Created by surf2stl.c";
        fwrite(header, 1, 80, f);
        int zero = 0;
        fwrite(&zero, sizeof(int), 1, f); // Placeholder for facet count
    }

    for (i = 0; i < rows - 1; ++i) {
        for (j = 0; j < cols - 1; ++j) {
            float p1[3] = {x[i][j], y[i][j], z[i][j]};
            float p2[3] = {x[i][j+1], y[i][j+1], z[i][j+1]};
            float p3[3] = {x[i+1][j+1], y[i+1][j+1], z[i+1][j+1]};
            write_facet(f, p1, p2, p3, mode);
            ++nfacets;

            float p4[3] = {x[i+1][j], y[i+1][j], z[i+1][j]};
            write_facet(f, p3, p4, p1, mode);
            ++nfacets;
        }
    }

    if (strcmp(mode, "ascii") == 0) {
        fprintf(f, "endsolid ascii\n");
    } else {
        fseek(f, 80, SEEK_SET);
        fwrite(&nfacets, sizeof(int), 1, f);
    }

    fclose(f);
    printf("triangulos cargados a stl: %d \n", nfacets);
}
