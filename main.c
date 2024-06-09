#include <stdio.h>
#include <stdlib.h>
#include "place.h"

#define MAX_DISPLAY_LEN 10

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_filename>\n", argv[0]);
        return 1;
    }

    const char* filename = argv[1];

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
    read_input(filename, &alpha, &beta, &gamma, &lambda, &die_start_x, &die_start_y, &die_width, &die_height, &input_count, inputs, &output_count, outputs, ff_blocks, gate_blocks, instances, nets, bin, placements_rows_set, displacement_delay, qpin_delay, timing_slack, gate_power);

    // Free allocated memory
    return 0;
}
