#include <stdio.h>

//Index 0 is x dim, 1 is y dim, and 2 is z din
typedef double vec3[3];

struct triangle {
    vec3 v1, v2, v3;
    vec3 n;
};

double vec3_dot( vec3 v, vec3 w){
    return v[0] * w[0] + v[1] * w[1] + v[2] * w[2];
}

void vec3_cross(vec3 out, vec3 v, vec3 w){
    out[0] = v[1] * w[2] - v[2] * w[1];
    out[1] = -(v[0] * w[2] - v[2] * w[0]);
    out[2] = v[0] * w[1] - v[1] * w[0];
}

void vec3_sub(vec3 out, vec3 v, vec3 w){
    out[0] = v[0] - w[0];
    out[1] = v[1] - w[1];
    out[2] = v[2] - w[2];

}

double image[Y][X][3];

int main(int argc, char *argv[]){

    int y, x;
    double dy, dx;
    FILE *f;
    vec3 p;
    vec3 v1, v2;
    vec3 alpha, beta, gamma;

    //Make a triangle
    struct triangle t;
    t.v1[0] = -0.5;
    t.v1[1] = -0.5;
    t.v1[2] = -1.0;

    t.v2[0] = 0.0;
    t.v2[1] = 0.0;
    t.v2[2] = -1.0;

    t.v3[0] = -0.25;
    t.v3[1] = 0.5;
    t.v3[2] = -1.0;

    //Produce Image
    for(y = 0; y < Y; y++){
        for(x = 0; x < X; x++){
            dx = ((x - (X / 2.0)) / (X / 2.0)) + (1.0 / X);
            //printf("%d" ,"%lf\n", x, dx);
            dy = ((y - (Y / 2.0)) / (Y / 2.0)) + (1.0 / Y);

            //na (c - b) x (p - b);
            p[0] = dx;
            p[1] = dy;
            p[2] = -1.0; //Triangle plane is at -1

            vec3_sub(v1, t.v3, t.v2);
            vec3_sub(v2, p, t.v2);
            vec3_cross(alpha, v1, v2);

            //nb = (a - c) x (p - c)
            //nc = (b - a) x (c - a)

        }
    }


    //Write the Image
    fopen("barycentric.ppm", "w");
    fprintf(f, "P3\n%d\n%d\n255\n", X, Y);
    for (y = 0; y < Y; y++){
        for(x = 0; x < X; x++){
            fprintf(f, "%d %d %d\n", 
                (unsigned char) (image[y][x][0] * 255.99),
                (unsigned char) (image[y][x][1] * 255.99),
                (unsigned char) (image[y][x][2] * 255.99));
        }
    }

    return 0;
}