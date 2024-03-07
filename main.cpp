#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>
#include <stdio.h>

#define FILTER_SIZE 3
#define WIDTH 450 // 750 and 450 and 736. unesco750-1 and L and japan
#define HEIGHT 339 // 500 and 339 and 552. unesco750-1and L and japan
#define DATA_SIZE WIDTH * HEIGHT

int normalization_constant = 3;
int threshold = 50;
const char *fileName = "L.raw"; // unesco750-1.raw and L.raw and japan.raw

std::vector<int> sobelFilterX3x3{
    -1, 0, 1, 
    -2, 0, 2,
     -1, 0, 1};
std::vector<int> sobelFilterY3x3{
    -1, -2, -1, 
    0, 0, 0, 
    1, 2, 1};

std::vector<int> sobelFilterX5x5{
    -1, -2, 0, 2, 1,
    -2, -4, 0, 4, 1, 
    -4, -8, 0, 8, 4, 
    -2, -4, 0, 4, 1,
    -1, -2, 0, 2, 1};

std::vector<int> sobelFilterY5x5{
    -1, -2, -4, -2, -1,
    -2, -4, -8, -4, -2, 
     0,  0, 0,  0,  0,
     2, 4, 4, 4, 2, 
     1, 2, 8, 2, 1};

void calculateGradient(const unsigned char inputImage[HEIGHT][WIDTH], 
                       unsigned char gradientImage[HEIGHT][WIDTH], 
                       const std::vector<int>& filterX, 
                       const std::vector<int>& filterY, 
                       int filterSize);
void calculateEdges(const unsigned char gradientImage[HEIGHT][WIDTH], unsigned char edgeImage[HEIGHT][WIDTH]);

int main() {
    FILE *inputFile, *outputFile;

    unsigned char inputData[DATA_SIZE];
    unsigned char inputImage[HEIGHT][WIDTH];
    unsigned char gradientImage3x3[HEIGHT][WIDTH], gradientImage5x5[HEIGHT][WIDTH];;
    unsigned char edgeImage3x3[HEIGHT][WIDTH], edgeImage5x5[HEIGHT][WIDTH];

    inputFile = fopen(fileName, "rb");
    fread(inputData, 1, DATA_SIZE, inputFile);
    fclose(inputFile);

    // Convert linear array to 2D array for processing
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            inputImage[i][j] = inputData[i * WIDTH + j];
        }
    }

    calculateGradient(inputImage, gradientImage3x3, sobelFilterX3x3, sobelFilterY3x3, 3);
    calculateEdges(gradientImage3x3, edgeImage3x3);

    calculateGradient(inputImage, gradientImage5x5, sobelFilterX5x5, sobelFilterY5x5, 5);
    calculateEdges(gradientImage5x5, edgeImage5x5);

    // output 3x3 gradient image
    outputFile = fopen("outputGradient_3x3.raw", "wb");
    fwrite(gradientImage3x3, 1, DATA_SIZE, outputFile);
    fclose(outputFile);

    // output 3x3 edge image
    outputFile = fopen("outputEdge_3x3.raw", "wb");
    fwrite(edgeImage3x3, 1, DATA_SIZE, outputFile);
    fclose(outputFile);

    // output 5x5 gradient image
    outputFile = fopen("outputGradient_5x5.raw", "wb");
    fwrite(gradientImage5x5, 1, DATA_SIZE, outputFile);
    fclose(outputFile);

    // output 5x5 edge image
    outputFile = fopen("outputEdge_5x5.raw", "wb");
    fwrite(edgeImage5x5, 1, DATA_SIZE, outputFile);
    fclose(outputFile);
}

void calculateGradient(const unsigned char inputImage[HEIGHT][WIDTH], 
                       unsigned char gradientImage[HEIGHT][WIDTH], 
                       const std::vector<int>& filterX, 
                       const std::vector<int>& filterY, int filterSize) {
    if (filterSize == 5) {
        normalization_constant = 15;
        threshold = 80;
    }
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (y <= filterSize / 2 || y >= HEIGHT - filterSize / 2 || x <= filterSize / 2 || x >= WIDTH - filterSize / 2) {
                gradientImage[y][x] = 0;
                continue;
            }
            double gradientX = 0, gradientY = 0;
            int filterIndex = 0;
            for (int ky = -filterSize / 2; ky <= filterSize / 2; ky++) {
                for (int kx = -filterSize / 2; kx <= filterSize / 2; kx++, filterIndex++) {
                    gradientX += filterX[filterIndex] * inputImage[y + ky][x + kx];
                    gradientY += filterY[filterIndex] * inputImage[y + ky][x + kx];
                }
            }
            gradientImage[y][x] = std::sqrt(gradientX * gradientX + gradientY * gradientY) / normalization_constant;
        }
    }
}

void calculateEdges(const unsigned char gradientImage[HEIGHT][WIDTH], unsigned char edgeImage[HEIGHT][WIDTH]) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            edgeImage[y][x] = (gradientImage[y][x] > threshold) ? 255 : 0;
        }
    }
}