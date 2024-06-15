//
// Created by Ming-Hao Hsu on 2024/6/15.
//

#include <cstdint>
#include <iostream>
#include "structure.h"
#include "grid.h"

shared_ptr<Nets> find_clk_nets(const shared_ptr<Nets>& nets) {
    auto clk_nets = make_shared<Nets>();
    for (auto& [key, net] : nets->map) {
        for (auto& [pin_key, pin] : net.map) {
            if (pin.libPinName.find("CLK") != string::npos) {
                clk_nets->map[key] = net;
                break;
            }
        }
    }
    return clk_nets;
}

void bank_flip_flops(const shared_ptr<Insts>& instances, const shared_ptr<Nets>& nets, const shared_ptr<FFs>& ff_blocks, const shared_ptr<Grid>& grid) {
    auto clk_nets = find_clk_nets(nets);

    // Combine 2 2-bit FFs to 1 4-bit FF
    vector<Inst*> chosen_instances(4, nullptr);
    for (auto& [key, net] : clk_nets->map) {
        uint8_t count = 0;
        for (auto& [pin_key, pin] : net.map) {
            auto instance = instances->map.find(pin.instName);
            if (instance == instances->map.end()) continue;
            if (instance->second.isUsed) continue;
            if (instance->second.lib_cell_name.find("FF") == string::npos) continue;
            if (auto ff = ff_blocks->map.find(instance->second.lib_cell_name); ff == ff_blocks->map.end() || ff->second.bits != 1) continue;

            if (count < chosen_instances.size()) {
                chosen_instances[count++] = &instance->second;
            } else {
                break;
            }
        }

        if (count < chosen_instances.size()) continue;
        for (auto& inst : chosen_instances) {
            inst->isUsed = true;
        }

        string new_ff_name = "FF40_" + string(chosen_instances[0]->inst_name) + "_" + string(chosen_instances[1]->inst_name) + "_" + string(chosen_instances[2]->inst_name) + "_" + string(chosen_instances[3]->inst_name);
        auto new_instance = make_shared<Inst>();
        new_instance->inst_name     = new_ff_name;
        new_instance->lib_cell_name = "FF40";
        new_instance->width         = 867;
        new_instance->height        = 84;
        new_instance->isUsed        = false;

        if (place_ff(chosen_instances[0], new_instance, grid, 10000)) {
        } else {
            for (auto& inst : chosen_instances) {
                inst->isUsed = false;
            }
            continue;
        }

        for (auto& [key, net] : nets->map) {
            for (auto& [key, pin] : net.map) {
                if (auto pos = find_if(begin(chosen_instances), end(chosen_instances), [&](Inst* inst) { return strcmp(inst->inst_name, pin.instName) == 0; }); pos != end(chosen_instances)) {
                    auto new_pin = make_shared<NetPin>();
                    strcpy(new_pin->instName, new_ff_name.c_str());
                    if (strstr(pin.libPinName, "Q") != nullptr) {
                        strcpy(new_pin->libPinName, ("Q" + to_string(distance(begin(chosen_instances), pos))).c_str());
                    } else if (strstr(pin.libPinName, "D") != nullptr) {
                        strcpy(new_pin->libPinName, ("D" + to_string(distance(begin(chosen_instances), pos))).c_str());
                    } else if (strstr(pin.libPinName, "CLK") != nullptr) {
                        strcpy(new_pin->libPinName, "CLK");
                    } else {
                        cerr << "Unknown pin type" << endl;
                    }
                    string key = new_pin->instName + string("/") + new_pin->libPinName;
                    strcpy(new_pin->key, key.c_str());
                    net.map[new_pin->instName] = *new_pin;
                    net.map.erase(pin.instName);
                }
            }
        }

        instances->map[new_instance->inst_name] = *new_instance;

        for (auto& inst : chosen_instances) {
            inst->isUsed = USED;
            inst = nullptr;
        }

        clk_nets->map.clear();
    }

    cout << "After banking:" << endl;
    print_ff_blocks(instances, ff_blocks);
    cout << "Complete banking" << endl;
}