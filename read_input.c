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

    fscanf(file, "Alpha %lf\n", alpha);
    fscanf(file, "Beta %lf\n", beta);
    fscanf(file, "Gamma %lf\n", gamma);
    fscanf(file, "Lambda %lf\n", lambda);
    fscanf(file, "DieSize %lf %lf %lf %lf\n", die_lower_left_x, die_lower_left_y, die_upper_right_x, die_upper_right_y);

    fscanf(file, "NumInput %u\n", num_inputs);
    inputs->count = *num_inputs;
    inputs->items = malloc(*num_inputs * sizeof(Input*));
    for (int i = 0; i < *num_inputs; i++) {
        Input* input = malloc(sizeof(Input));
        inputs->items[i] = input;
        fscanf(file, "Input %s %u %u\n", &input->name, &input->x, &input->y);
        // printf("Input %u: %s, x = %u, y = %u\n", i, input->name, input->x, input->y);
    }

    fscanf(file, "NumOutput %u\n", num_outputs);
    outputs->count = *num_outputs;
    outputs->items = malloc(*num_outputs * sizeof(Output*));
    for (int i = 0; i < *num_outputs; i++) {
        Output* output = malloc(sizeof(Output));
        outputs->items[i] = output;
        fscanf(file, "Output %s %u %u\n", &output->name, &output->x, &output->y);
        // printf("Output %u: %s, x = %u, y = %u\n", i, output->name, output->x, output->y);
    }

    char line[MAX_LINE_LEN];
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "FlipFlop", 8) == 0) {
            // Read FlipFlop block
            ff_blocks->count++;
            ff_blocks->items = realloc(ff_blocks->items, ff_blocks->count * sizeof(FF*));
            FF* ff = malloc(sizeof(FF));
            ff_blocks->items[ff_blocks->count - 1] = ff;
            sscanf(line, "FlipFlop %hu %s %hu %hu %hu\n", &ff->bits, &ff->name, &ff->width, &ff->height, &ff->pin_count);
            ff->pins = malloc(ff->pin_count * sizeof(Pin*));
            // printf("FlipFlop %hu %s %hu %hu %hu\n", ff->bits, ff->name, ff->width, ff->height, ff->pin_count);
            for (uint64_t i = 0; i < ff->pin_count; i++) {
                fgets(line, sizeof(line), file);
                Pin* pin = malloc(sizeof(Pin));
                ff->pins[i] = pin;
                sscanf(line, "Pin %s %hu %hu\n", &pin->name, &pin->x, &pin->y);
                // printf("Pin %s %hu %hu\n", pin->name, pin->x, pin->y);
            }
        } else if (strncmp(line, "Gate", 4) == 0) {
            // Read Gate block
            gate_blocks->count++;
            gate_blocks->items = realloc(gate_blocks->items, gate_blocks->count * sizeof(Gate*));
            Gate* gate = malloc(sizeof(Gate));
            gate_blocks->items[gate_blocks->count - 1] = gate;
            sscanf(line, "Gate %s %hu %hu %hu\n", &gate->name, &gate->width, &gate->height, &gate->pin_count);
            gate->pins = malloc(gate->pin_count * sizeof(Pin*));
            // printf("Gate %s %hu %hu %hu\n", gate->name, gate->width, gate->height, gate->pin_count);
            for (uint64_t i = 0; i < gate->pin_count; i++) {
                fgets(line, sizeof(line), file);
                Pin* pin = malloc(sizeof(Pin));
                gate->pins[i] = pin;
                sscanf(line, "Pin %s %hu %hu\n", &pin->name, &pin->x, &pin->y);
                // printf("Pin %s %hu %hu\n", pin->name, pin->x, pin->y);
            }
        } else break;
    }

    sscanf(line, "NumInstances %u\n", &instances->count);
    // printf("NumInstances %u\n", instances->count);
    for (uint64_t i = 0; i < instances->count; i++) {
        instances->items = realloc(instances->items, (i + 1) * sizeof(Inst*));
        Inst* instance = malloc(sizeof(Inst));
        instances->items[i] = instance;
        fscanf(file, "Inst %s %s %u %u\n", &instance->inst_name, &instance->lib_cell_name, &instance->x, &instance->y);
        // printf("Inst %s %s %u %u\n", instance->inst_name, instance->lib_cell_name, instance->x, instance->y);
    }
    
    fscanf(file, "NumNets %u\n", &nets->count);
    // printf("NumNets %u\n", nets->count);
    for (uint64_t i = 0; i < nets->count; i++) {
        nets->items = realloc(nets->items, (i + 1) * sizeof(Net*));
        Net* net = malloc(sizeof(Net));
        nets->items[i] = net;
        fscanf(file, "Net %s %u\n", &net->name, &net->pinCount);
        // printf("Net %s %u\n", net->name, net->pinCount);
        net->pins = malloc(net->pinCount * sizeof(NetPin*));
        for (uint64_t j = 0; j < net->pinCount; j++) {
            NetPin* pin = malloc(sizeof(NetPin));
            net->pins[j] = pin;
            fscanf(file, "Pin %9[^/]/%9s\n", &pin->instName, &pin->libPinName);
            // printf("Pin %s/%s\n", pin->instName, pin->libPinName);
        }
    }

    fscanf(file, "BinWidth %u", &bin->width);
    fscanf(file, "BinHeight %u", &bin->height);
    fscanf(file, "BinMaxUtil %lf", &bin->maxUtil);
    printf("Bin %u %u %lf\n", bin->width, bin->height, bin->maxUtil);

    // while (fgets(line, sizeof(line), file)) {
    //     if (strncmp(line, "PlacementsRow", 13) == 0) {
    //         placements_rows_set->count++;
    //         placements_rows_set->items = realloc(placements_rows_set->items, placements_rows_set->count * sizeof(PlacementsRows));
    //         PlacementsRows* row = &placements_rows_set->items[placements_rows_set->count - 1];
    //         sscanf(line, "PlacementsRow %lf %lf %lf %lf %u", &row->start_x, &row->start_y, &row->width, &row->height, &row->totalNumOfSites);
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
