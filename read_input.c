#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define maximum lengths for strings
#define MAX_NAME_LEN 100
#define MAX_LINE_LEN 1024

// Define structures to hold the input data
typedef struct {
    float lowerLeftX, lowerLeftY;
    float upperRightX, upperRightY;
} DieSize;

typedef struct {
    char name[MAX_NAME_LEN];
    float x, y;
} Pin;

typedef struct {
    char name[MAX_NAME_LEN];
    int bits;
    int width, height, pinCount;
    Pin *pins;
} FlipFlop;

typedef struct {
    char name[MAX_NAME_LEN];
    int width, height, pinCount;
    Pin *pins;
} Gate;

typedef struct {
    char name[MAX_NAME_LEN];
    int instanceCount;
    struct {
        char name[MAX_NAME_LEN];
        char libCellName[MAX_NAME_LEN];
        float x, y;
    } *instances;
} PlacementResult;

typedef struct {
    char name[MAX_NAME_LEN];
    int pinCount;
    struct {
        char instName[MAX_NAME_LEN];
        char libPinName[MAX_NAME_LEN];
    } *pins;
} Net;

typedef struct {
    float width, height, maxUtil;
} Bin;

typedef struct {
    float startX, startY;
    float siteWidth, siteHeight;
    int totalNumOfSites;
} PlacementRow;

typedef struct {
    float coefficient;
} DisplacementDelay;

typedef struct {
    char libCellName[MAX_NAME_LEN];
    float delay;
} QpinDelay;

typedef struct {
    char instanceCellName[MAX_NAME_LEN];
    char pinName[MAX_NAME_LEN];
    float slack;
} TimingSlack;

typedef struct {
    char libCellName[MAX_NAME_LEN];
    float powerConsumption;
} GatePower;

// Assuming previous structure definitions and readInputFile function are already included

void displayInputData(float alpha, float beta, float gamma, float lambda, 
                      DieSize dieSize, int numInputs, Pin *inputs, 
                      int numOutputs, Pin *outputs, int numFlipFlops, FlipFlop *flipFlops) {
    // Display weight factors
    printf("Alpha: %f, Beta: %f, Gamma: %f, Lambda: %f\n", alpha, beta, gamma, lambda);
    
    // Display DieSize
    printf("DieSize: %f %f %f %f\n", dieSize.lowerLeftX, dieSize.lowerLeftY, dieSize.upperRightX, dieSize.upperRightY);

    // Display input pins
    printf("Number of Inputs: %d\n", numInputs);
    for (int i = 0; i < numInputs; i++) {
        printf("Input %s: %f %f\n", inputs[i].name, inputs[i].x, inputs[i].y);
    }

    // Display output pins
    printf("Number of Outputs: %d\n", numOutputs);
    for (int i = 0; i < numOutputs; i++) {
        printf("Output %s: %f %f\n", outputs[i].name, outputs[i].x, outputs[i].y);
    }

    // Display FlipFlops
    printf("Number of FlipFlops: %d\n", numFlipFlops);
    for (int i = 0; i < numFlipFlops; i++) {
        printf("FlipFlop %s: %d bits, %d x %d, %d pins\n", flipFlops[i].name, flipFlops[i].bits, flipFlops[i].width, flipFlops[i].height, flipFlops[i].pinCount);
        for (int j = 0; j < flipFlops[i].pinCount; j++) {
            printf("  Pin %s: %f %f\n", flipFlops[i].pins[j].name, flipFlops[i].pins[j].x, flipFlops[i].pins[j].y);
        }
    }
}


void readInputFile(const char *filename, float *alpha, float *beta, float *gamma, float *lambda, 
                   DieSize *dieSize, int *numInputs, Pin **inputs, 
                   int *numOutputs, Pin **outputs, int *numFlipFlops, FlipFlop **flipFlops) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    // Read weight factors
    fscanf(file, "Alpha %f\n", alpha);
    fscanf(file, "Beta %f\n", beta);
    fscanf(file, "Gamma %f\n", gamma);
    fscanf(file, "Lambda %f\n", lambda);

    // Read DieSize
    fscanf(file, "DieSize %f %f %f %f\n", &dieSize->lowerLeftX, &dieSize->lowerLeftY, &dieSize->upperRightX, &dieSize->upperRightY);

    // Read input pins
    fscanf(file, "NumInput %d\n", numInputs);
    *inputs = (Pin *)malloc((*numInputs) * sizeof(Pin));
    for (int i = 0; i < *numInputs; i++) {
        fscanf(file, "Input %s %f %f\n", (*inputs)[i].name, &(*inputs)[i].x, &(*inputs)[i].y);
    }

    // Read output pins
    fscanf(file, "NumOutput %d\n", numOutputs);
    *outputs = (Pin *)malloc((*numOutputs) * sizeof(Pin));
    for (int i = 0; i < *numOutputs; i++) {
        fscanf(file, "Output %s %f %f\n", (*outputs)[i].name, &(*outputs)[i].x, &(*outputs)[i].y);
    }

    // Read FlipFlops (example for one flip-flop)
    *numFlipFlops = 1;  // Assuming only one flip-flop for demonstration
    *flipFlops = (FlipFlop *)malloc((*numFlipFlops) * sizeof(FlipFlop));
    fscanf(file, "FlipFlop %d %s %d %d %d\n", &(*flipFlops)[0].bits, (*flipFlops)[0].name, &(*flipFlops)[0].width, &(*flipFlops)[0].height, &(*flipFlops)[0].pinCount);
    (*flipFlops)[0].pins = (Pin *)malloc((*flipFlops)[0].pinCount * sizeof(Pin));
    for (int i = 0; i < (*flipFlops)[0].pinCount; i++) {
        fscanf(file, "Pin %s %f %f\n", (*flipFlops)[0].pins[i].name, &(*flipFlops)[0].pins[i].x, &(*flipFlops)[0].pins[i].y);
    }

    fclose(file);
}


int main() {
    const char *inputFileName = "sampleCase"; // replace with the actual input file name

    // Variables to hold the read data
    float alpha, beta, gamma, lambda;
    DieSize dieSize;
    int numInputs, numOutputs, numFlipFlops;
    Pin *inputs = NULL;
    Pin *outputs = NULL;
    FlipFlop *flipFlops = NULL;

    // Read the input file
    readInputFile(inputFileName, &alpha, &beta, &gamma, &lambda, &dieSize, &numInputs, &inputs, &numOutputs, &outputs, &numFlipFlops, &flipFlops);

    // Display the read data
    displayInputData(alpha, beta, gamma, lambda, dieSize, numInputs, inputs, numOutputs, outputs, numFlipFlops, flipFlops);

    // Free allocated memory
    free(inputs);
    free(outputs);
    for (int i = 0; i < numFlipFlops; i++) {
        free(flipFlops[i].pins);
    }
    free(flipFlops);

    return 0;
}
