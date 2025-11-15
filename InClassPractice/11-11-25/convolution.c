#include <stdio.h>
#include <string.h>
#include <math.h>

#define SIZE 1024;

double image[SIZE][SIZE];
double output[SIZE][SIZE];

void load_pgm(){
    FILE *f;

    char s[10];
    int i, j, v;

    //Change to use an actual pgm file that you have
    f = fopen("bigger_digger.pgm", "r");

    // Eating the header
    fscanf(f, " %s ", s);
    fscanf(f, " %s ", s);
    fscanf(f, " %s ", s);
    fscanf(f, " %s ", s);

    for(j = 0; j < 1024; j++){
        for(i = 0; i < 1024; i++){
            fscanf(f, " %d ", &v);

            image[j][i] = v;
        }
    }
    fclose(f);

}

void write_pgm(double out[SIZE][SIZE]){
    int i, j;
    FILE *f;

    f = fopen("convolution.pgm", "w");

    fprintf(f, "P2\n1024 1024\n255\n");

    for(j = 0; j < 1024; j++){
        for(i = 0; i < 1024; i++){
            fprintf(f, "%d ", (int) (fabs(out[j][i] > 255 ? 255: fabs(out[j][i]))));
        }
        if(i % 8 == 0) {
            printf(f, "\n");
        }
    }
    fclose(f);

}

double sobel_x[3][3] = {{1, 0, -1}, 
                        {2, 0, -2}, 
                        {1, 0, -1}};

double sobel_y[3][3] = {{ 1,  2,  1},
                        { 0,  0,  0},
                        {-1, -2, -1}};

double gaussian[3][3] = {{1, 2, 1}, 
                         {2, 4, 2}, 
                         {1, 2, 1}};

void convolve(double k[3][3]){
    int i, j;

    int weight ;

    weight = k[0][0] + 
            k[0][1] +
            k[0][2] +
            k[1][0] +
            k[1][1] +
            k[1][2] +
            k[2][0] +
            k[2][1] +
            k[2][2];


    for(i = 0; i < SIZE; i++){
        output[i][0] = output[0][i] = output[i][SIZE - 1] = output[SIZE - 1][i] = 0;
    }

    for(j = 1; j < SIZE - 1; j++){
        for(i = 1; i < SIZE -1; i++){
            output[j][i] = (image[j - 1][i - 1] * k[0][0] +
                        image[j - 1][i ] * k[0][1] +
                        image[j - 1][i + 1] * k[0][2] +
                        image[j ][i - 1] * k[1][0] +
                        image[j ][i  ] * k[1][1] +
                        image[j ][i + 1] * k[1][2] +
                        image[j + 1][i - 1] * k[2][0] +
                        image[j + 1][i  ] * k[2][1] +
                        image[j + 1][i + 1] * k[2][2]); 
        }
    }

    if(weight != 0){
        output[j][i] /= weight;
    }
}

double x[SIZE][SIZE], y[SIZE][SIZE];

int main(int argc, char *argv[]){

    int i, j;

    load_pgm();

    convolve(sobel_x);
    memcpy(x, output, sizeof (x));

    convolve(sobel_y);
    memcpy(y, output, sizeof (y));

    for(j = 0; j < SIZE; j++){
        for(i = 0; i < SIZE; i++){
            output[j][i] = sqrt(x[j][i] * x[j][i] + y[j][i] * y[j][i]);
        }
    }

    convolve(gaussian);

    write_pgm(output);



    return 0;
}