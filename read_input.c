#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "place.h"

#define MAX_LINE_LEN 1024

// Function to read input data
int read_input(
    // Input file name
    const char*         filename, 
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

    printf("Reading input from file %s\n", filename);
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error opening file %s\n", filename);
        return -1;
    }

    fscanf("Alpha %lf\n", alpha);
    fscanf("Beta %lf\n", beta);
    fscanf("Gamma %lf\n", gamma);
    fscanf("Lambda %lf\n", lambda);
    fscanf("DieSize %lf %lf %lf %lf\n", die_lower_left_x, die_lower_left_y, die_upper_right_x, die_upper_right_y);


    printf("%lf %lf %lf %lf %lf %lf %lf %lf\n", *alpha, *beta, *gamma, *lambda, *die_lower_left_x, *die_lower_left_y, *die_upper_right_x, *die_upper_right_y);

    free(alpha);
    free(beta);
    free(gamma);
    free(lambda);

    // sscanf("NumInput %d\n", num_inputs);
    // inputs->count = *num_inputs;
    // for (int i = 0; i < *num_inputs; i++) {
    //     Input* input = malloc(sizeof(Input));
    //     inputs->items[i] = *input;
    //     sscanf("Input %s %lu %lu", &input->name, &input->x, &input->y);
    // }

    // sscanf("NumOutput %d", num_outputs);
    // outputs->count = *num_outputs;
    // for (int i = 0; i < *num_outputs; i++) {
    //     Output* output = malloc(sizeof(Output));
    //     outputs->items[i] = *output;
    //     sscanf("Output %s %lu %lu", *output->name, &output->x, &output->y);
    // }

    // char line[MAX_LINE_LEN];
    // while (fgets(line, sizeof(line), file)) {
    //     if (strncmp(line, "FlipFlop", 8) == 0) {
    //         // Read FlipFlop block
    //         ff_blocks->count++;
    //         ff_blocks->items = realloc(ff_blocks->items, ff_blocks->count * sizeof(FF));
    //         FF* ff = &ff_blocks->items[ff_blocks->count - 1];
    //         sscanf(line, "FlipFlop %ld %s %lf %lf %ld", &ff->bits, *ff->name, &ff->width, &ff->height, &ff->pin_count);
    //         ff->pins = malloc(ff->pin_count * sizeof(Pin));
    //         for (uint64_t i = 0; i < ff->pin_count; i++) {
    //             fgets(line, sizeof(line), file);
    //             Pin* pin = &ff->pins[i];
    //             sscanf(line, "Pin %s %lf %lf", *pin->name, &pin->x, &pin->y);
    //         }
    //     } else if (strncmp(line, "Gate", 4) == 0) {
    //         // Read Gate block
    //         gate_blocks->count++;
    //         gate_blocks->items = realloc(gate_blocks->items, gate_blocks->count * sizeof(Gate));
    //         Gate* gate = &gate_blocks->items[gate_blocks->count - 1];
    //         sscanf(line, "Gate %s %lf %lf %ld", *gate->name, &gate->width, &gate->height, &gate->pin_count);
    //         gate->pins = malloc(gate->pin_count * sizeof(Pin));
    //         for (uint64_t i = 0; i < gate->pin_count; i++) {
    //             fgets(line, sizeof(line), file);
    //             Pin* pin = &gate->pins[i];
    //             sscanf(line, "Pin %s %lf %lf", *pin->name, &pin->x, &pin->y);
    //         }
    //     } else break;
    // }

    // sscanf("NumInstances %lu", instances->count);
    // for (uint64_t i = 0; i < instances->count; i++) {
    //     instances->items = realloc(instances->items, (i + 1) * sizeof(Inst));
    //     Inst* instance = &instances->items[i];
    //     sscanf(line, "Inst %s %s %lf %lf", *instance->inst_name, *instance->lib_cell_name, &instance->x, &instance->y);
    // }
    
    // sscanf("NumNets %lu", nets->count);
    // for (uint64_t i = 0; i < nets->count; i++) {
    //     nets->items = realloc(nets->items, (i + 1) * sizeof(Net));
    //     Net* net = &nets->items[i];
    //     sscanf(line, "Net %s %d", net->name, &net->pinCount);
    //     net->pins = malloc(net->pinCount * sizeof(*net->pins));
    //     for (uint64_t j = 0; j < net->pinCount; j++) {
    //         fgets(line, sizeof(line), file);
    //         sscanf(line, "Net %s %s", net->pins[j].instName, net->pins[j].libPinName);
    //     }
    // }

    // sscanf(line, "BinWidth %lf", &bin->width);
    // sscanf(line, "BinHeight %lf", &bin->height);
    // sscanf(line, "BinMaxUtil %lf", &bin->maxUtil);

    // while (fgets(line, sizeof(line), file)) {
    //     if (strncmp(line, "PlacementsRow", 13) == 0) {
    //         placements_rows_set->count++;
    //         placements_rows_set->items = realloc(placements_rows_set->items, placements_rows_set->count * sizeof(PlacementsRows));
    //         PlacementsRows* row = &placements_rows_set->items[placements_rows_set->count - 1];
    //         sscanf(line, "PlacementsRow %lf %lf %lf %lf %lu", &row->start_x, &row->start_y, &row->width, &row->height, &row->totalNumOfSites);
    //     } else break;
    // }
    
    // sscanf(line, "DisplacementDelay %f", &displacement_delay->coefficient);

    // while (fgets(line, sizeof(line), file)) {
    //     if (strncmp(line, "QpinDelay", 9) == 0) {
    //         qpin_delay->count++;
    //         qpin_delay->items = realloc(qpin_delay->items, qpin_delay->count * sizeof(QpinDelay));
    //         QpinDelay* qpin = &qpin_delay->items[qpin_delay->count - 1];
    //         sscanf(line, "QpinDelay %s %f", qpin->libCellName, &qpin->delay);
    //     } else break;
    // }

    // while (fgets(line, sizeof(line), file)) {
    //     if (strncmp(line, "TimingSlack", 11) == 0) {
    //         timing_slack->count++;
    //         timing_slack->items = realloc(timing_slack->items, timing_slack->count * sizeof(TimingSlack));
    //         TimingSlack* slack = &timing_slack->items[timing_slack->count - 1];
    //         sscanf(line, "TimingSlack %s %s %lf", slack->instanceCellName, slack->pinName, &slack->slack);
    //     } else break;
    // }

    // while (fgets(line, sizeof(line), file)) {
    //     if (strncmp(line, "GatePower", 9) == 0) {
    //         gate_power->count++;
    //         gate_power->items = realloc(gate_power->items, gate_power->count * sizeof(GatePower));
    //         GatePower* power = &gate_power->items[gate_power->count - 1];
    //         sscanf(line, "GatePower %s %lf", power->libCellName, &power->powerConsumption);
    //     } else break;
    // }
    // printf("Read input successfully\n");
    fclose(file);
    return 0;
}
