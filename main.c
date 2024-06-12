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

    double initial_score = calculate_score(ff_blocks, gate_blocks, instances, timing_slack, gate_power, qpin_delay, displacement_delay, alpha, beta, gamma, lambda, bin);
    printf("Score: %lf\n", initial_score);

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

    for (int i = 0; i < 2; i++) {
        bank_flip_flops(instances, nets, ff_blocks);
        if (check_placement(instances, die) == 1) {
            printf("Placement is valid\n");
        } else {
            printf("Placement is invalid\n");
        }

        if (check_overlap(instances, new_insts, placements_rows_set) == 1) {
            printf("Overlap is valid\n");
        } else {
            printf("Overlap is invalid\n");
        }
        double score = calculate_score(ff_blocks, gate_blocks, instances, timing_slack, gate_power, qpin_delay, displacement_delay, alpha, beta, gamma, lambda, bin);
        printf("In iteration %d, score: %lf, reduced percentage: %lf\n", i, score, (initial_score - score) / initial_score * 100);

        Inst* tmp;
        HASH_ITER(hh, instances->map, tmp, instances->map) {
            printf("Instance name: %s, x: %d, y: %d\n", tmp->inst_name, tmp->x, tmp->y);
        }

        Net* tmp_net;
        HASH_ITER(hh, nets->map, tmp_net, nets->map) {
            printf("Net name: %s\n", tmp_net->name);
        }


    }

    double final_score = calculate_score(ff_blocks, gate_blocks, instances, timing_slack, gate_power, qpin_delay, displacement_delay, alpha, beta, gamma, lambda, bin);
    printf("Score: %lf\n", final_score);
    printf("Reduced Percentage: %lf\n", (initial_score - final_score) / initial_score * 100);
    // place_main(*ff_blocks, gate_blocks, *inputs, *outputs, *instances, *nets, *placements_rows_set, *displacement_delay, *bin, alpha, lambda, die);


    // Free allocated memory
    return 0;
}