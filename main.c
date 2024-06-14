#include <stdio.h>
#include <stdlib.h>
#include "place.h"
#include "utils.h"
#include "optimization.h"
#include "read_input.h"
#include "constraints.h"
#include "quadtree.h"

#define MAX_DISPLAY_LEN 10

int main(int argc, char* argv[]) {
    // if (argc != 2) {
    //     fprintf(stderr, "Usage: %s <input_filename>\n", argv[0]);
    //     return 1;
    // }

    // const char* filename = argv[1];
    const char* filename = "../testcase1.txt";

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


    Grid* grid = malloc(sizeof(Grid));

    printf("Reading input...\n");
    // Assuming read_input is defined and works correctly
    read_input(filename, &alpha, &beta, &gamma, &lambda, die, &input_count, inputs, &output_count, outputs, ff_blocks, gate_blocks, instances, nets, bin, placements_rows_set, displacement_delay, qpin_delay, timing_slack, gate_power, grid);
    sort_ff_by_size(ff_blocks);
    FF* ff, *tmp;
    HASH_ITER(hh, ff_blocks->map, ff, tmp) {
        printf("%s: %u, %u, %u, %u\n", ff->name, ff->bits, ff->height, ff->width, ff->height * ff->width);
    }

    double initial_score = calculate_score(ff_blocks, gate_blocks, instances, timing_slack, gate_power, qpin_delay, displacement_delay, alpha, beta, gamma, lambda, bin, nets, inputs, outputs, die);
    printf("Score: %lf\n", initial_score);


    for (int i = 0; i < 50; i++) {
        printf("Iteration %d\n", i+1);
        bank_flip_flops(instances, nets, ff_blocks, grid);
        // printf("Banked flip flops\n");
        // if (check_placement(instances, die) == 1) {
        //     printf("Placement is valid\n");
        // } else {
        //     printf("Placement is invalid\n");
        // }

        // if (check_overlap(instances, placements_rows_set) == 1) {
        //     printf("Overlap is valid\n");
        // } else {
        //     printf("Overlap is invalid\n");
        // }
        double score = calculate_score(ff_blocks, gate_blocks, instances, timing_slack, gate_power, qpin_delay, displacement_delay, alpha, beta, gamma, lambda, bin, nets, inputs, outputs, die);
            printf("In iteration %d, score: %lf, reduced percentage: %lf\n", i+1, score, (initial_score - score) / initial_score * 100);

    // double final_score = calculate_score(ff_blocks, gate_blocks, instances, timing_slack, gate_power, qpin_delay, displacement_delay, alpha, beta, gamma, lambda, bin);
    // printf("Score: %lf\n", final_score);
    // printf("Reduced Percentage: %lf\n", (initial_score - final_score) / initial_score * 100);

    }
    // Free allocated memory
    return 0;
}