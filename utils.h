//
// Created by Ming-Hao Hsu on 2024/6/15.
//

#ifndef IEDA_FINAL_PROJECT_UTILS_H
#define IEDA_FINAL_PROJECT_UTILS_H

#include <cstdint>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <memory>
#include <cmath>
#include <fstream>
#include "structure.h"

using namespace std;

double manhattan_distance(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2);

double calculate_score(
        const shared_ptr<FFs> &ffs, const shared_ptr<Gates> &gates, const shared_ptr<Insts> &insts,
        const shared_ptr<TimingSlacks> &timing_slacks,
        const shared_ptr<GatePowers> &gate_powers, const shared_ptr<QpinDelays> &qpin_delays,
        const shared_ptr<DisplacementDelay> &displacement_delay,
        double alpha, double beta, double gamma, double lambda, const shared_ptr<Bin> &bin,
        const shared_ptr<Nets> &nets, const shared_ptr<Inputs> &inputs, const shared_ptr<Outputs> &outputs,
        const shared_ptr<Die> &die, const string &output_filename
);

#endif //IEDA_FINAL_PROJECT_UTILS_H
