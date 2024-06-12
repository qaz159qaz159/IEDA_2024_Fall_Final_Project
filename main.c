#include <stdio.h>
#include <stdlib.h>
#include "place.h"
#include "utils.h"
#include "optimization.h"
#include "read_input.h"

#define MAX_DISPLAY_LEN 10

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_filename>\n", argv[0]);
        return 1;
    }

    const char* filename = argv[1];

    double alpha, beta, gamma, lambda;
    Die* die = malloc(sizeof(Die));
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


    // Used Inst
    Insts* used_insts = malloc(sizeof(Insts));
    // New Inst
    Insts* new_insts = malloc(sizeof(Insts));

    printf("Reading input...\n");
    // Assuming read_input is defined and works correctly
    read_input(filename, &alpha, &beta, &gamma, &lambda, die, &input_count, inputs, &output_count, outputs, ff_blocks, gate_blocks, instances, nets, bin, placements_rows_set, displacement_delay, qpin_delay, timing_slack, gate_power);

    InstNetMapping* instNetMappings = malloc(sizeof(InstNetMapping));
    size_t count = 0;
    instNetMappings->net = NULL; 
    instNetMappings = populate_inst_net_mapping(nets, &count);

    // Example usage of find_net_by_inst_name
    // Net* netFound = malloc(sizeof(Net));
    // netFound = find_net_by_inst_name(instNetMappings, count, "C90606/IN4");
    // printf("Net found for instance %s: %s\n", "C90606/IN4", netFound->name);

    // Example usage of sort_ff_by_size
    // printf("Before sorting:\n");
    // FF* current_ff;
    // FF* tmp;
    // HASH_ITER(hh, ff_blocks->map, current_ff, tmp) {
    //     printf("FF name: %s, size: %d\n", current_ff->name, current_ff->width * current_ff->height);
    // }

    sort_ff_by_size(ff_blocks);

    // printf("After sorting:\n");
    // HASH_ITER(hh, ff_blocks->map, current_ff, tmp) {
    //     printf("FF name: %s, size: %d\n", current_ff->name, current_ff->width * current_ff->height);
    // }

    bank_flip_flops(instances, nets, ff_blocks, used_insts, new_insts);

    // place_main(*ff_blocks, gate_blocks, *inputs, *outputs, *instances, *nets, *placements_rows_set, *displacement_delay, *bin, alpha, lambda, die);


    // Free allocated memory
    return 0;
}