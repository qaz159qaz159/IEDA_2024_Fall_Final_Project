//
// Created by Ming-Hao Hsu on 2024/6/14.
//

#include <iostream>
#include <memory>
#include "read_input.h"
#include "constraints.h"
#include "quadtree.h"

#define MAX_DISPLAY_LEN 10

using namespace std;

int main(int argc, char* argv[]) {
    // if (argc != 2) {
    //     cerr << "Usage: " << argv[0] << " <input_filename>" << endl;
    //     return 1;
    // }

    // const char* filename = argv[1];
    const string filename = "../testcase1.txt";

    double alpha, beta, gamma, lambda;
    auto die = make_shared<Die>();
    // Input and Output
    int input_count, output_count;
    auto inputs = make_shared<Inputs>();
    auto outputs = make_shared<Outputs>();
    // FF and Gate blocks
    auto ff_blocks = make_shared<FFs>();
    auto gate_blocks = make_shared<Gates>();
    // Instances
    auto instances = make_shared<Insts>();
    // Nets
    auto nets = make_shared<Nets>();
    // Bin
    auto bin = make_shared<Bin>();
    // Placements
    auto placements_rows_set = make_shared<PlacementsRowsSet>();
    // DisplacementDelay
    auto displacement_delay = make_shared<DisplacementDelay>();
    // QpinDelay
    auto qpin_delay = make_shared<QpinDelays>();
    // TimingSlack
    auto timing_slack = make_shared<TimingSlacks>();
    // GatePower
    auto gate_power = make_shared<GatePowers>();

    auto grid = make_shared<Grid>();

    cout << "Reading input..." << endl;
    // Assuming read_input is defined and works correctly
    read_input(filename, alpha, beta, gamma, lambda, *die, input_count, *inputs, output_count, *outputs, *ff_blocks, *gate_blocks, *instances, *nets, *bin, *placements_rows_set, *displacement_delay, *qpin_delay, *timing_slack, *gate_power, *grid);
    sort_ff_by_size(*ff_blocks);
    FF* ff;
    FF* tmp;
    HASH_ITER(hh, ff_blocks->map, ff, tmp) {
        cout << ff->name << ": " << ff->bits << ", " << ff->height << ", " << ff->width << ", " << ff->height * ff->width << endl;
    }

//    double initial_score = calculate_score(*ff_blocks, *gate_blocks, *instances, *timing_slack, *gate_power, *qpin_delay, *displacement_delay, alpha, beta, gamma, lambda, *bin, *nets, *inputs, *outputs, *die);
//    cout << "Score: " << initial_score << endl;

//    for (int i = 0; i < 50; i++) {
//        cout << "Iteration " << i+1 << endl;
//        bank_flip_flops(*instances, *nets, *ff_blocks, *grid);
//        // cout << "Banked flip flops" << endl;
//        // if (check_placement(*instances, *die) == 1) {
//        //     cout << "Placement is valid" << endl;
//        // } else {
//        //     cout << "Placement is invalid" << endl;
//        // }
//
//        // if (check_overlap(*instances, *placements_rows_set) == 1) {
//        //     cout << "Overlap is valid" << endl;
//        // } else {
//        //     cout << "Overlap is invalid" << endl;
//        // }
//        double score = calculate_score(*ff_blocks, *gate_blocks, *instances, *timing_slack, *gate_power, *qpin_delay, *displacement_delay, alpha, beta, gamma, lambda, *bin, *nets, *inputs, *outputs, *die);
//        cout << "In iteration " << i+1 << ", score: " << score << ", reduced percentage: " << (initial_score - score) / initial_score * 100 << "%" << endl;
//    }

    // No need to explicitly free allocated memory since we are using smart pointers
    return 0;
}

