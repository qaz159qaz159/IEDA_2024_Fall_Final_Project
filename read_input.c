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
    Die*                die,
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
    fscanf(file, "DieSize %u %u %u %u\n", &die->lower_left_x, &die->lower_left_y, &die->upper_right_x, &die->upper_right_y);
    die->height = die->upper_right_y - die->lower_left_y;
    die->width = die->upper_right_x - die->lower_left_x;

    fscanf(file, "NumInput %u\n", num_inputs);
    inputs->count = *num_inputs;
    inputs->map = NULL;
    for (int i = 0; i < *num_inputs; i++) {
        Input* input = (Input*)malloc(sizeof(Input));
        fscanf(file, "Input %s %u %u\n", input->name, &input->x, &input->y);
        HASH_ADD_STR(inputs->map, name, input);
    }

    fscanf(file, "NumOutput %u\n", num_outputs);
    outputs->count = *num_outputs;
    outputs->map = NULL;
    for (int i = 0; i < *num_outputs; i++) {
        Output* output = malloc(sizeof(Output));
        fscanf(file, "Output %s %u %u\n", output->name, &output->x, &output->y);
        HASH_ADD_STR(outputs->map, name, output);
    }

    char line[MAX_LINE_LEN];
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "FlipFlop", 8) == 0) {
            // Read FlipFlop block
            ff_blocks->count++;
            FF* ff = malloc(sizeof(FF));
            ff->map = NULL;
            sscanf(line, "FlipFlop %hu %s %hu %hu %hu\n", &ff->bits, ff->name, &ff->width, &ff->height, &ff->pin_count);
            HASH_ADD_STR(ff_blocks->map, name, ff);
            for (uint64_t i = 0; i < ff->pin_count; i++) {
                fgets(line, sizeof(line), file);
                Pin* pin = malloc(sizeof(Pin));
                sscanf(line, "Pin %s %hu %hu\n", pin->name, &pin->x, &pin->y);
                HASH_ADD_STR(ff->map, name, pin);
            }
        } else if (strncmp(line, "Gate", 4) == 0) {
            // Read Gate block
            gate_blocks->count++;
            Gate* gate = malloc(sizeof(Gate));
            gate->map = NULL;
            sscanf(line, "Gate %s %hu %hu %hu\n", gate->name, &gate->width, &gate->height, &gate->pin_count);
            HASH_ADD_STR(gate_blocks->map, name, gate);
            for (uint64_t i = 0; i < gate->pin_count; i++) {
                fgets(line, sizeof(line), file);
                Pin* pin = malloc(sizeof(Pin));
                sscanf(line, "Pin %s %hu %hu\n", pin->name, &pin->x, &pin->y);
                HASH_ADD_STR(gate->map, name, pin);
            }
        } else break;
    }

    sscanf(line, "NumInstances %u\n", &instances->count);
    instances->map = NULL;
    for (uint64_t i = 0; i < instances->count; i++) {
        Inst* instance = malloc(sizeof(Inst));
        fscanf(file, "Inst %s %s %u %u\n", instance->inst_name, instance->lib_cell_name, &instance->x, &instance->y);
        HASH_ADD_STR(instances->map, inst_name, instance);
    }
    
    fscanf(file, "NumNets %u\n", &nets->count);
    nets->map = NULL;
    for (uint64_t i = 0; i < nets->count; i++) {
        fgets(line, sizeof(line), file);
        Net* net = malloc(sizeof(Net));
        net->map = NULL;
        sscanf(line, "Net %s %u\n", net->name, &net->pinCount);
        HASH_ADD_STR(nets->map, name, net);
        for (uint64_t j = 0; j < net->pinCount; j++) {
            fgets(line, sizeof(line), file);
            NetPin* pin = (NetPin*)malloc(sizeof(NetPin));
            if (strchr(line, '/') != NULL) {
                sscanf(line, "Pin %15s\n", pin->instName);
                HASH_ADD_STR(net->map, instName, pin);
            } else {
                sscanf(line, "Pin %15s\n", pin->instName);
                HASH_ADD_STR(net->map, instName, pin);
            }
        }
    }

    fscanf(file, "BinWidth %u\n", &bin->width);
    fscanf(file, "BinHeight %u\n", &bin->height);
    fscanf(file, "BinMaxUtil %lf\n", &bin->maxUtil);

    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "PlacementRows", 13) == 0) {
            placements_rows_set->count++;
            placements_rows_set->items = realloc(placements_rows_set->items, placements_rows_set->count * sizeof(PlacementsRows*));
            PlacementsRows* row = malloc(sizeof(PlacementsRows));
            placements_rows_set->items[placements_rows_set->count - 1] = row;
            sscanf(line, "PlacementsRow %u %u %u %u %u", &row->start_x, &row->start_y, &row->width, &row->height, &row->totalNumOfSites);
        } else break;
    }
    
    sscanf(line, "DisplacementDelay %lf", &displacement_delay->coefficient);

    qpin_delay->count = 0;
    qpin_delay->map = NULL;
    timing_slack->count = 0;
    timing_slack->map = NULL;
    gate_power->count = 0;
    gate_power->map = NULL;
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "QpinDelay", 9) == 0) {
            qpin_delay->count++;
            QpinDelay* qpin = malloc(sizeof(QpinDelay));
            sscanf(line, "QpinDelay %s %lf", qpin->libCellName, &qpin->delay);
            HASH_ADD_STR(qpin_delay->map, libCellName, qpin);
        } else if (strncmp(line, "TimingSlack", 11) == 0) {
            timing_slack->count++;
            TimingSlack* slack = malloc(sizeof(TimingSlack));
            sscanf(line, "TimingSlack %s %s %lf", slack->instanceCellName, slack->pinName, &slack->slack);
            HASH_ADD_STR(timing_slack->map, instanceCellName, slack);
        } else if (strncmp(line, "GatePower", 9) == 0) {
            gate_power->count++;
            GatePower* power = malloc(sizeof(GatePower));
            sscanf(line, "GatePower %s %lf", power->libCellName, &power->powerConsumption);
            HASH_ADD_STR(gate_power->map, libCellName, power);
        } else break;
    }

    printf("Read input successfully\n");
    fclose(file);
    return 0;
}
