//
// Created by Ming-Hao Hsu on 2024/6/15.
//

#include "utils.h"


using namespace std;

double manhattan_distance(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2) {
    return static_cast<double>(abs(static_cast<int>(x1) - static_cast<int>(x2)) +
                               abs(static_cast<int>(y1) - static_cast<int>(y2)));
}

double calculate_score(
        const shared_ptr<FFs> &ffs, const shared_ptr<Gates> &gates, const shared_ptr<Insts> &insts,
        const shared_ptr<TimingSlacks> &timing_slacks,
        const shared_ptr<GatePowers> &gate_powers, const shared_ptr<QpinDelays> &qpin_delays,
        const shared_ptr<DisplacementDelay> &displacement_delay,
        double alpha, double beta, double gamma, double lambda, const shared_ptr<Bin> &bin,
        const shared_ptr<Nets> &nets, const shared_ptr<Inputs> &inputs, const shared_ptr<Outputs> &outputs,
        const shared_ptr<Die> &die, const string &output_filename
) {
    ofstream output_file(output_filename);
    output_file << fixed;

    double total_tns = 0.0;
    double total_power = 0.0;
    double total_area = 0.0;
    double density_penalty = 0.0;

    // Calculate TNS: QpinDelay + TimingSlack
    double total_qpin_delay = 0.0, total_timing_slack = 0.0;
    for (auto &[key, inst]: insts->map) {
        if (inst.libCellName.find("FF") == string::npos) continue;

        if (auto slack = timing_slacks->map.find(inst.instName); slack != timing_slacks->map.end()) {
            total_timing_slack += slack->second.slack;
        }

        if (auto qpin_delay = qpin_delays->map.find(inst.libCellName); qpin_delay != qpin_delays->map.end()) {
            total_qpin_delay += qpin_delay->second.delay;
        }
    }

    output_file << "Total QpinDelay: " << total_qpin_delay << endl;
    output_file << "Total TimingSlack: " << total_timing_slack << endl;



    // Calculate TNS: DisplacementDelay
    double all_net_displacement_delay = 0.0;
    for (auto &[key, net]: nets->map) {
        vector<NetPin *> pins_in_net;
        pins_in_net.reserve(net.map.size());
        for (auto &[pin_key, pin]: net.map) {
            pins_in_net.push_back(&pin);
        }

        for (size_t i = 0; i < pins_in_net.size(); i++) {
            if ((pins_in_net[i]->libPinName.find("INPUT") != string::npos ||
                 pins_in_net[i]->libPinName.find("Q") != string::npos ||
                 pins_in_net[i]->libPinName.find("OUT") != string::npos) &&
                pins_in_net[i]->instName.find("OUTPUT") == string::npos) {
                auto inst_in = insts->map.find(pins_in_net[i]->instName);
                for (size_t j = 0; j < pins_in_net.size(); j++) {
                    if (j == i) continue;
                    auto inst_out_tmp = insts->map.find(pins_in_net[j]->instName);
                    all_net_displacement_delay += displacement_delay->coefficient *
                                                  static_cast<double>(manhattan_distance(inst_in->second.x,
                                                                                         inst_in->second.y,
                                                                                         inst_out_tmp->second.x,
                                                                                         inst_out_tmp->second.y));
                }
            }
        }
    }
    output_file << "Total DisplacementDelay: " << all_net_displacement_delay << endl;
    total_tns = total_qpin_delay + total_timing_slack + all_net_displacement_delay;
    output_file << "Total TNS: " << total_tns << endl;

    // Calculate Power and Area
    for (auto &[key, inst]: insts->map) {
        if (inst.isUsed) continue;

        if (auto power = gate_powers->map.find(inst.libCellName); power != gate_powers->map.end() &&
                                                                    power->second.libCellName.find("FF") !=
                                                                    string::npos) {
            total_power += power->second.powerConsumption;
        }

        if (auto ff = ffs->map.find(inst.libCellName); ff != ffs->map.end()) {
            total_area += (ff->second.width * ff->second.height);
        }
    }

    output_file << "Total Power: " << total_power << endl;
    output_file << "Total Area: " << total_area << endl;

    double score = alpha * total_tns + beta * total_power + gamma * total_area + density_penalty;
    output_file << "Score: " << score << endl;

    return score;
}