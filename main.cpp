#include <iostream>
#include <memory>
#include <string>
#include "read_input.h"
#include "utils.h"

using namespace std;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <input_filename>" << endl;
        return 1;
    }

    const string filename = argv[1];

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

    cout << fixed;
    cout << "Reading input..." << endl;
    // Assuming read_input is defined and works correctly
    read_input(filename, alpha, beta, gamma, lambda, *die, input_count, *inputs, output_count, *outputs, *ff_blocks,
               *gate_blocks, *instances, *nets, *bin, *placements_rows_set, *displacement_delay, *qpin_delay,
               *timing_slack, *gate_power, *grid);

    // print all nets and its pins
    for (auto &[key, net]: nets->map) {
        cout << "Net: " << key << " " << net.map.size() << endl;
        for (auto &[key, pin]: net.map) {
            cout << "  Pin: " << key << " " << pin.instName << " " << pin.libPinName << endl;
        }
    }

    const string output_filename = "../output.txt";
    cout << "Score: " << calculate_score(ff_blocks, gate_blocks, instances, timing_slack, gate_power, qpin_delay,
                                        displacement_delay, alpha, beta, gamma, lambda, bin, nets, inputs, outputs, die, output_filename)
         << endl;

    return 0;
}
