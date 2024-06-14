//
// Created by Ming-Hao Hsu on 2024/6/15.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <memory>
#include <cstring>
#include "grid.h"
#include "structure.h"

using namespace std;

// Function to read input data
int read_input(
        // Input file name
        const string& filename,
        // Parameters
        double& alpha,
        double& beta,
        double& gamma,
        double& lambda,
        Die& die,
        // Input and Output
        int& num_inputs,
        Inputs& inputs,
        int& num_outputs,
        Outputs& outputs,
        // FF and Gate blocks
        FFs& ff_blocks,
        Gates& gate_blocks,
        // Instances
        Insts& instances,
        // Nets
        Nets& nets,
        // Bin
        Bin& bin,
        // Placements
        PlacementsRowsSet& placements_rows_set,
        // DisplacementDelay
        DisplacementDelay& displacement_delay,
        // QpinDelay
        QpinDelays& qpin_delay,
        // TimingSlack
        TimingSlacks& timing_slack,
        // GatePower
        GatePowers& gate_power,
        // Grid
        Grid& grid
) {

    cout << "Reading input from file " << filename << endl;
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file " << filename << endl;
        return -1;
    }

    file >> alpha >> beta >> gamma >> lambda;
    file >> die.lower_left_x >> die.lower_left_y >> die.upper_right_x >> die.upper_right_y;
    die.height = die.upper_right_y - die.lower_left_y;
    die.width = die.upper_right_x - die.lower_left_x;

    file >> num_inputs;
    inputs.count = num_inputs;
    for (int i = 0; i < num_inputs; i++) {
        auto input = make_shared<Input>();
        file >> input->name >> input->x >> input->y;
        inputs.map[input->name] = *input;
    }

    file >> num_outputs;
    outputs.count = num_outputs;
    for (int i = 0; i < num_outputs; i++) {
        auto output = make_shared<Output>();
        file >> output->name >> output->x >> output->y;
        outputs.map[output->name] = *output;
    }

    string line;
    getline(file, line);
    ff_blocks.count = 0;
    gate_blocks.count = 0;
    while (getline(file, line)) {
        if (line.find("FlipFlop") == 0) {
            // Read FlipFlop block
            ff_blocks.count++;
            auto ff = make_shared<FF>();
            stringstream ss(line);
            ss >> ff->bits >> ff->name >> ff->width >> ff->height >> ff->pin_count;
            ff_blocks.map[ff->name] = *ff;
            for (uint16_t i = 0; i < ff->pin_count; i++) {
                getline(file, line);
                auto pin = make_shared<Pin>();
                stringstream pin_ss(line);
                pin_ss >> pin->name >> pin->x >> pin->y;
                ff->map[pin->name] = *pin;
            }
        } else if (line.find("Gate") == 0) {
            // Read Gate block
            gate_blocks.count++;
            auto gate = make_shared<Gate>();
            stringstream ss(line);
            ss >> gate->name >> gate->width >> gate->height >> gate->pin_count;
            gate_blocks.map[gate->name] = *gate;
            for (uint16_t i = 0; i < gate->pin_count; i++) {
                getline(file, line);
                auto pin = make_shared<Pin>();
                stringstream pin_ss(line);
                pin_ss >> pin->name >> pin->x >> pin->y;
                gate->map[pin->name] = *pin;
            }
        } else {
            break;
        }
    }

    getline(file, line);
    stringstream ss(line);
    ss >> instances.count;
    for (uint64_t i = 0; i < instances.count; i++) {
        auto instance = make_shared<Inst>();
        file >> instance->inst_name >> instance->lib_cell_name >> instance->x >> instance->y;
        instance->isUsed = NOT_USED;
        auto ff_it = ff_blocks.map.find(instance->lib_cell_name);
        if (ff_it != ff_blocks.map.end()) {
            instance->width = ff_it->second.width;
            instance->height = ff_it->second.height;
        }
        instances.map[instance->inst_name] = *instance;
    }

    file >> nets.count;
    for (uint64_t i = 0; i < nets.count; i++) {
        getline(file, line);
        auto net = make_shared<Net>();
        stringstream net_ss(line);
        net_ss >> net->name >> net->pinCount;
        nets.map[net->name] = *net;
        for (uint64_t j = 0; j < net->pinCount; j++) {
            getline(file, line);
            auto pin = make_shared<NetPin>();
            stringstream pin_ss(line);
            pin_ss >> pin->key;
            if (line.find('/') != string::npos) {
                strcpy(pin->instName, strtok(pin->key, "/"));
                strcpy(pin->libPinName, strtok(nullptr, "/"));
            } else {
                strcpy(pin->instName, pin->key);
                strcpy(pin->libPinName, pin->key);
            }
            net->map[pin->instName] = *pin;
        }
    }

    file >> bin.width >> bin.height >> bin.maxUtil;

    placements_rows_set.count = 0;
    auto row_start = make_shared<PlacementsRows>();
    auto row_end = make_shared<PlacementsRows>();
    while (getline(file, line)) {
        if (line.find("PlacementRows") == 0) {
            if (placements_rows_set.count == 0) {
                ss >> row_start->start_x >> row_start->start_y >> row_start->width >> row_start->height >> row_start->totalNumOfSites;
            } else {
                ss >> row_end->start_x >> row_end->start_y >> row_end->width >> row_end->height >> row_end->totalNumOfSites;
            }
            placements_rows_set.count++;
        } else {
            break;
        }
    }

    create_grid(grid, row_start->width * row_start->totalNumOfSites, row_end->start_y + row_end->height - row_start->start_y, row_start->width, row_start->height, row_start->start_x, row_start->start_y);

    // Insert instances to the grid
    for (auto& instance : instances.map) {
        insert_to_grid(grid, &instance.second);
    }

    ss >> displacement_delay.coefficient;

    qpin_delay.count = 0;
    timing_slack.count = 0;
    gate_power.count = 0;
    while (getline(file, line)) {
        if (line.find("QpinDelay") == 0) {
            qpin_delay.count++;
            auto qpin = make_shared<QpinDelay>();
            stringstream qpin_ss(line);
            qpin_ss >> qpin->libCellName >> qpin->delay;
            qpin_delay.map[qpin->libCellName] = *qpin;
        } else if (line.find("TimingSlack") == 0) {
            timing_slack.count++;
            auto slack = make_shared<TimingSlack>();
            stringstream slack_ss(line);
            slack_ss >> slack->instanceCellName >> slack->pinName >> slack->slack;
            timing_slack.map[slack->instanceCellName] = *slack;
        } else if (line.find("GatePower") == 0) {
            gate_power.count++;
            auto power = make_shared<GatePower>();
            stringstream power_ss(line);
            power_ss >> power->libCellName >> power->powerConsumption;
            gate_power.map[power->libCellName] = *power;
        } else {
            break;
        }
    }

    cout << "Read input successfully" << endl;
    return 0;
}
