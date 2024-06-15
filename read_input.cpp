//
// Created by Ming-Hao Hsu on 2024/6/15.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <memory>
#include <cstring>
#include "read_input.h"

using namespace std;

// Function to read input data
int read_input(
        // Input file name
        const string &filename,
        // Parameters
        double &alpha,
        double &beta,
        double &gamma,
        double &lambda,
        Die &die,
        // Input and Output
        int &num_inputs,
        Inputs &inputs,
        int &num_outputs,
        Outputs &outputs,
        // FF and Gate blocks
        FFs &ff_blocks,
        Gates &gate_blocks,
        // Instances
        Insts &instances,
        // Nets
        Nets &nets,
        // Bin
        Bin &bin,
        // Placements
        PlacementsRowsSet &placements_rows_set,
        // DisplacementDelay
        DisplacementDelay &displacement_delay,
        // QpinDelay
        QpinDelays &qpin_delay,
        // TimingSlack
        TimingSlacks &timing_slack,
        // GatePower
        GatePowers &gate_power,
        // Grid
        Grid &grid
) {

    cout << "Reading input from file " << filename << endl;
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file " << filename << endl;
        return -1;
    }

    string prefix;
    file >> prefix >> alpha >> prefix >> beta >> prefix >> gamma >> prefix
         >> lambda; // Error: "Alpha" is a string, should be alpha
    file >> prefix >> die.lower_left_x >> die.lower_left_y >> die.upper_right_x >> die.upper_right_y;
    die.height = die.upper_right_y - die.lower_left_y;
    die.width = die.upper_right_x - die.lower_left_x;

    file >> prefix >> num_inputs;
    inputs.count = num_inputs;
    for (int i = 0; i < num_inputs; i++) {
        auto input = make_shared<Input>();
        file >> prefix >> input->name >> input->x >> input->y;
        inputs.map[input->name] = *input;
        auto input_instance = make_shared<Inst>();
        input_instance->instName = input->name;
        input_instance->libCellName = "INPUT";
        input_instance->x = input->x;
        input_instance->y = input->y;
        instances.map[input->name] = *input_instance;
    }

    file >> prefix >> num_outputs;
    outputs.count = num_outputs;
    for (int i = 0; i < num_outputs; i++) {
        auto output = make_shared<Output>();
        file >> prefix >> output->name >> output->x >> output->y;
        outputs.map[output->name] = *output;
        auto output_instance = make_shared<Inst>();
        output_instance->instName = output->name;
        output_instance->libCellName = "OUTPUT";
        output_instance->x = output->x;
        output_instance->y = output->y;
        instances.map[output->name] = *output_instance;
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
            ss >> prefix >> ff->bits >> ff->name >> ff->width >> ff->height >> ff->pin_count;
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
            ss >> prefix >> gate->name >> gate->width >> gate->height >> gate->pin_count;
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

    stringstream ss(line);
    ss >> prefix >> instances.count;
    for (uint64_t i = 0; i < instances.count; i++) {
        auto instance = make_shared<Inst>();
        file >> prefix >> instance->instName >> instance->libCellName >> instance->x >> instance->y;
        instance->isUsed = false;
        auto ff_it = ff_blocks.map.find(instance->libCellName);
        if (ff_it != ff_blocks.map.end()) {
            instance->width = ff_it->second.width;
            instance->height = ff_it->second.height;
        }
        instances.map[instance->instName] = *instance;
    }

    instances.count = instances.map.size();

    file >> prefix >> nets.count;
    for (uint64_t i = 0; i < nets.count; i++) {
        auto net = make_shared<Net>();
        file >> prefix >> net->name >> net->pinCount;
        nets.map[net->name] = *net;
        for (uint64_t j = 0; j < net->pinCount; j++) {
            auto pin = make_shared<NetPin>();
            file >> prefix >> pin->key;
            if (pin->key.find('/') != string::npos) {
                // Split the key into instance name and lib pin name by '/'
                size_t pos = pin->key.find('/');
                pin->instName = pin->key.substr(0, pos);
                pin->libPinName = pin->key.substr(pos + 1);
            } else {
                // pin->instName, pin->libPinName are same as pin->key
                pin->instName = pin->key;
                pin->libPinName = pin->key;
            }
            nets.map[net->name].map[pin->key] = *pin;
        }
    }

    file >> prefix >> bin.width >> prefix >> bin.height >> prefix >> bin.maxUtil;
    placements_rows_set.count = 0;
    auto row_start = make_shared<PlacementsRows>();
    auto row_end = make_shared<PlacementsRows>();
    getline(file, line); // I don't know why this is needed
    while (getline(file, line)) {
        if (line.find("PlacementRows") == 0) {
            stringstream ss(line);
            if (placements_rows_set.count == 0) {
                ss >> prefix >> row_start->startX >> row_start->startY >> row_start->width >> row_start->height
                   >> row_start->totalNumOfSites;
            } else {
                ss >> prefix >> row_end->startX >> row_end->startY >> row_end->width >> row_end->height
                   >> row_end->totalNumOfSites;
            }
            placements_rows_set.count++;
        } else {
            break;
        }
    }

    // Create grid
    grid.create_grid(row_start->width * row_start->totalNumOfSites,
                     row_end->startY + row_end->height - row_start->startY, row_start->width, row_start->height,
                     row_start->startX, row_start->startY);

    // Insert instances to the grid
    for (auto &instance: instances.map) {
        grid.insert_to_grid(make_shared<Inst>(instance.second));
    }

    stringstream displacement_ss(line);
    displacement_ss >> prefix >> displacement_delay.coefficient;

    qpin_delay.count = 0;
    timing_slack.count = 0;
    gate_power.count = 0;
    while (getline(file, line)) {
        if (line.find("QpinDelay") == 0) {
            qpin_delay.count++;
            auto qpin = make_shared<QpinDelay>();
            stringstream qpin_ss(line);
            qpin_ss >> prefix >> qpin->libCellName >> qpin->delay;
            qpin_delay.map[qpin->libCellName] = *qpin;
        } else if (line.find("TimingSlack") == 0) {
            timing_slack.count++;
            auto slack = make_shared<TimingSlack>();
            stringstream slack_ss(line);
            slack_ss >> prefix >> slack->instanceCellName >> slack->pinName >> slack->slack;
            timing_slack.map[slack->instanceCellName] = *slack;
        } else if (line.find("GatePower") == 0) {
            gate_power.count++;
            auto power = make_shared<GatePower>();
            stringstream power_ss(line);
            power_ss >> prefix >> power->libCellName >> power->powerConsumption;
            gate_power.map[power->libCellName] = *power;
        } else {
            break;
        }
    }

    file.close();
    cout << "Read input successfully" << endl;
    return 0;
}