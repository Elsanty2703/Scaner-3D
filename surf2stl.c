// surf2stl.c
#include "surf2stl.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


void write_facet(FILE *f, float *p1, float *p2, float *p3) {
    float v1[3], v2[3], normal[3];

    v1[0] = p2[0] - p1[0];
    v2[0] = p3[0] - p1[0];
    v1[1] = p2[1] - p1[1];
    v2[1] = p3[1] - p1[1];
    v1[2] = p2[2] - p1[2];
    v2[2] = p3[2] - p1[2];

    normal[0] = v1[1]*v2[2] - v1[2]*v2[1];
    normal[1] = v1[2]*v2[0] - v1[0]*v2[2];
    normal[2] = v1[0]*v2[1] - v1[1]*v2[0];

    float norm = sqrtf(normal[0]*normal[0] + normal[1]*normal[1] + normal[2]*normal[2]);
    if (norm != 0) {
        normal[0] /= norm;
        normal[1] /= norm;
        normal[2] /= norm; //normalizar
    }


        fwrite(normal, sizeof(float), 3, f);
        fwrite(p1, sizeof(float), 3, f);
        fwrite(p2, sizeof(float), 3, f);
        fwrite(p3, sizeof(float), 3, f);
        unsigned short attr = 0;
        fwrite(&attr, sizeof(unsigned short), 1, f);

}

void surf2stl(Surf2STLContext *ctx) {
    switch (ctx->state){
        case F_OPEN:
            ctx->f = fopen(ctx->filename, "wb");//archivo out
        if (!ctx->f) {
            printf("esperando archivo");
        }
        else {
            char header[80] = "Created by Dynamite team";
            fwrite(header, 1, 80, ctx->f);
            int zero = 0;
            fwrite(&zero, sizeof(int), 1, ctx->f); // Placeholder for facet count
            ctx->state = ROWS_STL;
        }
        break;


        case ROWS_STL:
            if (ctx->i < ctx->rows - 1) {
                ctx->j = 0;
                ctx->state = COLS_STL;
            } else {
                fseek(ctx->f, 80, SEEK_SET);
                fwrite(&(ctx->nfacets), sizeof(int), 1, ctx->f);
                fclose(ctx->f);
                printf("triangulos cargados a stl: %d \n", ctx->nfacets);
                ctx->state = F_OPEN;
            }
        break;

        case COLS_STL:
            if (ctx->j < ctx->cols) {
                int j_next = (ctx->j + 1) % ctx->cols;

                float p1[3] = {ctx->x[ctx->i][ctx->j],     ctx->y[ctx->i][ctx->j],     ctx->z[ctx->i][ctx->j]};
                float p2[3] = {ctx->x[ctx->i][j_next],    ctx->y[ctx->i][j_next],    ctx->z[ctx->i][j_next]};
                float p3[3] = {ctx->x[ctx->i+1][j_next],  ctx->y[ctx->i+1][j_next],  ctx->z[ctx->i+1][j_next]};
                write_facet(ctx->f, p1, p2, p3);
                ++ctx->nfacets;

                float p4[3] = {ctx->x[ctx->i+1][ctx->j],   ctx->y[ctx->i+1][ctx->j],   ctx->z[ctx->i+1][ctx->j]};
                write_facet(ctx->f, p3, p4, p1);
                ++ctx->nfacets;

                ++ctx->j;
            } else {
                ++ctx->i;
                ctx->state = ROWS_STL;
            }
        break;


    }
}
