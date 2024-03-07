#include <stdio.h>
#include <math.h>

#define WIDTH 750 // 750 and 275
#define HEIGHT 500 // 500 and 183
#define DATA_SIZE WIDTH * HEIGHT
#define EDGE_INTENSITY 50
#define NO_EDGE 0
#define THRESHOLD 30 // This value should be set manually through trial and error

unsigned char clampPixelValue(int value);
void computeGradients(unsigned char image_input[HEIGHT][WIDTH], 
                      unsigned char horizontalGradient[HEIGHT][WIDTH], 
                      unsigned char verticalGradient[HEIGHT][WIDTH]);

void detectEdges(unsigned char horizontalGradient[HEIGHT][WIDTH],
                 unsigned char verticalGradient[HEIGHT][WIDTH],
                 unsigned char horizontalEdges[HEIGHT][WIDTH],
                 unsigned char verticalEdges[HEIGHT][WIDTH]);

int main() {
    FILE *inputFile;
    FILE *outputFileHGradient, *outputFileVGradient;
    unsigned char inputData[DATA_SIZE];
    unsigned char edges[HEIGHT][WIDTH];
    unsigned char image_input[HEIGHT][WIDTH];
    unsigned char horizontalEdges[HEIGHT][WIDTH], verticalEdges[HEIGHT][WIDTH];
    unsigned char horizontalGradient[HEIGHT][WIDTH], verticalGradient[HEIGHT][WIDTH];

    const char *fileName = "unesco750-1.raw";
    const char *horizontalOutputFileName = "verticalGradientImage.raw";
    const char *verticalOutputFileName = "horizontalGradientImage.raw";

    // Open input file and read image data
    inputFile = fopen(fileName, "rb");
    fread(inputData, 1, DATA_SIZE, inputFile);
    fclose(inputFile);

    // Convert linear array to 2D image array
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            image_input[i][j] = inputData[i * WIDTH + j];
        }
    }

    computeGradients(image_input, horizontalGradient, verticalGradient);
    detectEdges(horizontalGradient, verticalGradient, horizontalEdges, verticalEdges);

    // save vertical edge detected image
    FILE *outputFileHorizontalEdges = fopen("verticalEdges.raw", "wb");
    fwrite(horizontalEdges, 1, DATA_SIZE, outputFileHorizontalEdges);
    fclose(outputFileHorizontalEdges);

    // save horizontal edge detected image
    FILE *outputFileVerticalEdges = fopen("horizontalEdges.raw", "wb");
    fwrite(verticalEdges, 1, DATA_SIZE, outputFileVerticalEdges);
    fclose(outputFileVerticalEdges);

    // Save horizontal gradient image
    outputFileHGradient = fopen(horizontalOutputFileName, "wb");
    fwrite(horizontalGradient, 1, DATA_SIZE, outputFileHGradient);
    fclose(outputFileHGradient);

    // Save vertical gradient image
    outputFileVGradient = fopen(verticalOutputFileName, "wb");
    fwrite(verticalGradient, 1, DATA_SIZE, outputFileVGradient);
    fclose(outputFileVGradient);

    return 0;
}

unsigned char clampPixelValue(int value) {
    if (value < 0) {
        return 0;
    } else if (value > 255) {
        return 255;
    }
    return (unsigned char)value;
}

void computeGradients(unsigned char image_input[HEIGHT][WIDTH],
                      unsigned char horizontalGradient[HEIGHT][WIDTH],
                      unsigned char verticalGradient[HEIGHT][WIDTH]) {
    int gx, gy;

    for (int i = 1; i < HEIGHT - 1; ++i) {
        for (int j = 1; j < WIDTH - 1; ++j) {
            // Compute gradients for horizontal and vertical directions
            gx = (image_input[i-1][j-1] + 2*image_input[i][j-1] + image_input[i+1][j-1]) -
                 (image_input[i-1][j+1] + 2*image_input[i][j+1] + image_input[i+1][j+1]);

            gy = (image_input[i-1][j-1] + 2*image_input[i-1][j] + image_input[i-1][j+1]) -
                 (image_input[i+1][j-1] + 2*image_input[i+1][j] + image_input[i+1][j+1]);

            // Convert gradients to positive values
            unsigned char abs_gx = clampPixelValue(abs(gx));
            unsigned char abs_gy = clampPixelValue(abs(gy));

            horizontalGradient[i][j] = abs_gx;
            verticalGradient[i][j] = abs_gy;

            // Compute the gradient magnitude
            // gradientImage[i][j] = clampPixelValue((int)sqrt(gx * gx + gy * gy));
        }
    }
}

void detectEdges(unsigned char horizontalGradient[HEIGHT][WIDTH],
                 unsigned char verticalGradient[HEIGHT][WIDTH],
                 unsigned char horizontalEdges[HEIGHT][WIDTH],
                 unsigned char verticalEdges[HEIGHT][WIDTH]) {
    // Initialize edges images to zero
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            horizontalEdges[i][j] = NO_EDGE;
            verticalEdges[i][j] = NO_EDGE;
        }
    }

    for (int i = 1; i < HEIGHT - 1; ++i) {
        for (int j = 1; j < WIDTH - 1; ++j) {
            // Horizontal edge detection
            if (horizontalGradient[i][j] > THRESHOLD &&
                horizontalGradient[i][j] > horizontalGradient[i][j - 1] &&
                horizontalGradient[i][j] > horizontalGradient[i][j + 1]) {
                horizontalEdges[i][j] = EDGE_INTENSITY;
            }

            // Vertical edge detection
            if (verticalGradient[i][j] > THRESHOLD &&
                verticalGradient[i][j] > verticalGradient[i - 1][j] &&
                verticalGradient[i][j] > verticalGradient[i + 1][j]) {
                verticalEdges[i][j] = EDGE_INTENSITY;
            }
        }
    }
}
