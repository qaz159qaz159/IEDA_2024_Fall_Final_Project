#include <stdio.h>
#include <stdlib.h>
#include "place.h"

#define MAX_DISPLAY_LEN 10

// Function to print the data
void print_data(
    // Parameters
    double*             alpha, 
    double*             beta, 
    double*             gamma, 
    double*             lambda, 
    double*             die_lower_left_x, 
    double*             die_lower_left_y, 
    double*             die_upper_right_x, 
    double*             die_upper_right_y,
    // Input and Output
    int*                num_inputs,
    Inputs*             inputs,
    int*                num_outputs,
    Outputs*            outputs,
    // FF and Gate blocks
    FFs*                ff_blocks, 
    Gates*              gate_blocks,
    // Instances
    Insts*              instances, 
    // Nets
    Nets*               nets,
    // Bin
    Bin*                bin,
    // Placements
    PlacementsRowsSet*  placements_rows_set,
    // DisplacementDelay
    DisplacementDelay*  displacement_delay,
    // QpinDelay
    QpinDelays*         qpin_delay,
    // TimingSlack
    TimingSlacks*       timing_slack,
    // GatePower
    GatePowers*         gate_power
    ) {
    printf("Alpha %lf\n", *alpha);
    printf("Beta %lf\n", *beta);
    printf("Gamma %lf\n", *gamma);
    printf("Lambda %lf\n", *lambda);
    printf("DieSize %lf %lf %lf %lf\n", *die_lower_left_x, *die_lower_left_y, *die_upper_right_x, *die_upper_right_y);
    printf("NumInput %d\n", *num_inputs);
    for (int i = 0; i < MAX_DISPLAY_LEN; i++) {
        printf("Input %d: %s, x = %lu, y = %lu\n", i, *inputs->items[i].name, inputs->items[i].x, inputs->items[i].y);
    }
    printf("Num Outputs: %d\n", *num_outputs);
    for (int i = 0; i < MAX_DISPLAY_LEN; i++) {
        printf("Output %d: %s, x = %lu, y = %lu\n", i, *outputs->items[i].name, outputs->items[i].x, outputs->items[i].y);
    }
    printf("\nFlipFlop Blocks:\n");
    for (uint64_t i = 0; i < ff_blocks->count; i++) {
        Block* block = &ff_blocks->items[i];
        printf("Bits: %lu, name = %s, width = %lf, height = %lf, pin_count = %lu\n", block->bits, *block->name, block->width, block->height, block->pin_count);
    }
    printf("\nGate Blocks:\n");
    for (uint64_t i = 0; i < gate_blocks->count; i++) {
        Block* block = &gate_blocks->items[i];
        printf("Name = %s, width = %lf, height = %lf, pin_count = %lu\n", *block->name, block->width, block->height, block->pin_count);
    }
}

int main() {
    double alpha, beta, gamma, lambda, die_start_x, die_start_y, die_width, die_height;
    // Input and Output
    int input_count, output_count;
    Inputs* inputs = malloc(sizeof(Inputs));
    Outputs* outputs = malloc(sizeof(Outputs));
    // FF and Gate blocks
    FFs* ff_blocks = malloc(sizeof(FFs));
    Gates* gate_blocks = malloc(sizeof(Gates));
    // Instances
    Insts* instances = malloc(sizeof(Insts));   
    // Nets
    Nets* nets = malloc(sizeof(Nets));
    // Bin
    Bin* bin = malloc(sizeof(Bin));
    // Placements
    PlacementsRowsSet* placements_rows_set = malloc(sizeof(PlacementsRowsSet));
    // DisplacementDelay
    DisplacementDelay* displacement_delay = malloc(sizeof(DisplacementDelay));
    // QpinDelay
    QpinDelays* qpin_delay = malloc(sizeof(QpinDelays));
    // TimingSlack
    TimingSlacks* timing_slack = malloc(sizeof(TimingSlacks));
    // GatePower
    GatePowers* gate_power = malloc(sizeof(GatePowers));

    printf("Reading input...\n");
    // Assuming read_input is defined and works correctly
    read_input("sampleCase", 
    &alpha, &beta, &gamma, &lambda, 
    &die_start_x, &die_start_y, &die_width, &die_height,
    &input_count, inputs, &output_count, outputs,
    ff_blocks, gate_blocks, instances, nets, bin, placements_rows_set, displacement_delay, qpin_delay, timing_slack, gate_power);

    // Free allocated memory
    return 0;
}
