#include <stdio.h>

#define X 1024
#define Y 1024

//Will get a nubmer between 1 and 0 -> floating point
#define frand() (rand() / (RAND_MAX + 1.0))

#define SAMPLES_PER_PIXEL 100

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

double image[Y][X][3];  //Zero by placement -> black background oringally

int main(int argc, char *argv[]){

    //Need to do srand() i thinks
    srand(time(NULL));

    int y, x;
    double dy, dx;
    FILE *f;
    vec3 p;
    vec3 v1, v2;
    vec3 alpha, beta, gamma;
    vec3 na, nb, nc, n;
    vec3 bary;
    double sx, sy;
    int s;

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

    //n = (b - a) x ( c - a)
    vec3_sub(v1, t.v2, t.v1);
    vec3_sub(v2, t.v3, t.v1);
    vec3_cross(n, v1, v2);

    //Produce Image
    for(y = 0; y < Y; y++){
        for(x = 0; x < X; x++){
            //For anit-aliasing the s adds more samples per pixel
            for(s = 0; s < SAMPLES_PER_PIXEL; s++){

                //Aliased just less obvious than next approach
                //This only works at 4 samples per pixel
                // switch (s) {
                //     case 0:
                //         sx = -(1.0 / (2.0 * X));
                //         sy = -(1.0 / (2.0 * Y));
                //         break;
                //     case 1:
                //         sx = -(1.0 / (2.0 * X));
                //         sy = (1.0 / (2.0 * Y));
                //         break;
                //     case 2:
                //         sx = -(1.0 / (2.0 * X));
                //         sy = (1.0 / (2.0 * Y));
                //         break;
                //     case 3:
                //         sx = (1.0 / (2.0 * X));
                //         sy = (1.0 / (2.0 * Y));
                //         break;
                // }

                //Get a randum number. Scale it to the dimensions of the pixel. Then subtract half a pixel
                //This is better anti-aliasing
                sx = (frand() / X) - (1.0 / (2.0 * X));
                sy = (frand() / Y) - (1.0 / (2.0 * Y));

                dx = ((x - (X / 2.0)) / (X / 2.0)) + (1.0 / X) + sx;
                //printf("%d" ,"%lf\n", x, dx);
                dy = ((y - (Y / 2.0)) / (Y / 2.0)) + (1.0 / Y) + sy;

                //na (c - b) x (p - b);
                p[0] = dx;
                p[1] = dy;
                p[2] = -1.0; //Triangle plane is at -1

                vec3_sub(v1, t.v3, t.v2);
                vec3_sub(v2, p, t.v2);
                vec3_cross(na, v1, v2);

                //nb = (a - c) x (p - c)
                vec3_sub(v1, t.v1, t.v3);
                vec3_sub(v2, p, t.v2);
                vec3_cross(nb, v1, v2);

                //nc = (b - a) x (c - a)
                vec3_sub(v1, t.v2, t.v1);
                vec3_sub(v2, p, t.v1);
                vec3_cross(nc, v1, v2);

                bary[0] = vec3_dot(n, na) / vec3_dot(n,n);
                bary[1] = vec3_dot(n, nb) / vec3_dot(n,n);
                bary[2] = vec3_dot(n, nc) / vec3_dot(n,n);


                if(bary[0] >= 0 && bary[1] >= 0 && bary[2] >= 0){
                    image[y][x][0] = image[y][x][1] = image[y][x][2] += 1.0;

                    //To see pretty color all over remove if statement. Then lotsa triangles and color

                    //Pretty rainbow triangle
                    // image[y][x][0] = bary[0];
                    // image[y][x][1] = bary[1];
                    // image[y][x][2] = bary[2];
                }
            }
        }
    }


    //Write the Image
    fopen("barycentric.ppm", "w");
    fprintf(f, "P3\n%d\n%d\n255\n", X, Y);
    for (y = Y - 1; y >= 0; y--){
        for(x = 0; x < X; x++){
            fprintf(f, "%d %d %d\n", 
                (unsigned char) (image[y][x][0] * 255.99 / SAMPLES_PER_PIXEL),
                (unsigned char) (image[y][x][1] * 255.99 / SAMPLES_PER_PIXEL),
                (unsigned char) (image[y][x][2] * 255.99 / SAMPLES_PER_PIXEL));
        }
    }

    return 0;
}