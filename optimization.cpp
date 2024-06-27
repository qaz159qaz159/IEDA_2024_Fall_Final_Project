//
// Created by Ming-Hao Hsu on 2024/6/15.
//

#include <cstdint>
#include <iostream>
#include <algorithm>
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
    for (auto& [key, net] : clk_nets->map) {
        if (net.map.empty()) {
            clk_nets->map.erase(key);
        }
    }

    return clk_nets;
}

bool place_ff(Inst *oldInst, Inst *newInst, Grid *grid) {
    // Starting search position, with a 5-cell radius around the old instance
    auto startX = oldInst->x;
    auto startY = oldInst->y;
    uint32_t cellIndexTable[10][10] = {0};

    auto startCellX = (startX - grid->startX) / grid->gridSizeX;
    auto startCellY = (startY - grid->startY) / grid->gridSizeY;

    // Determine how many cells the new instance requires
    // TODO: do this step when reading the input
    auto newInstCellWidth   = (newInst->width + grid->gridSizeX - 1) / grid->gridSizeX;
    auto newInstCellHeight  = (newInst->height + grid->gridSizeY - 1) / grid->gridSizeY;

    // Create a table to store the index of each cell
    for (auto i = 0; i < 10; ++i) {
        for (auto j = 0; j < 10; ++j) {
            if (startCellX + i >= grid->cols || startCellY + j >= grid->rows) {
                continue;
            }
            cellIndexTable[i][j] = grid->get_cell_index(startX + i * grid->gridSizeX, startY + j * grid->gridSizeY);
        }
    }

    // Search for a suitable position in the 10x10 grid
    for (auto i = 0; i < 10; ++i) {
        for (auto j = 0; j < 10; ++j) {
            if (cellIndexTable[i][j] == 0) {
                continue;
            }

            // Check if the new instance overlaps with any existing instance
            auto isOverlap = false;
            for (auto k = 0; k < newInstCellWidth; ++k) {
                for (auto l = 0; l < newInstCellHeight; ++l) {
                    if (i + k >= 10 || j + l >= 10) {
                        isOverlap = true;
                        break;
                    }
                    auto cellIndex = cellIndexTable[i + k][j + l];
                    auto cell = grid->cells[cellIndex];
                    if (cell) {
                        isOverlap = true;
                        break;
                    }
                }
                if (isOverlap) {
                    break;
                }
            }
            if (isOverlap) {
                continue;
            }

            newInst->x = grid->startX + i * grid->gridSizeX;
            newInst->y = grid->startY + j * grid->gridSizeY;

            grid->insert_to_grid(newInst);
            return true;
        }
    }
    return false;
}


void bank_flip_flops(const shared_ptr<Insts>& instances, const shared_ptr<Nets>& nets, const shared_ptr<FFs>& ff_blocks, shared_ptr<Grid>& grid) {
    auto clk_nets = find_clk_nets(nets);
    // Combine 2 2-bit FFs to 1 4-bit FF
    vector<Inst*> chosen_instances(4, nullptr);
    for (auto item = clk_nets->map.begin(); item != clk_nets->map.end(); item++) {
        auto& clkNet = item->second;
        auto clkNetKey = item->first;
//        cout << "Found a clock net: " << clkNetKey << endl;
        if (clkNetKey == "net104127") {
            cout << "Found net104127" << endl;
            for (auto& [clkNetPinKey, clkNetPin] : clkNet.map) {
                cout << clkNetPin.instName << " " << clkNetPin.libPinName << endl;
            }
        fscanf(stdin, "%*c");
        }
//
//        printf("Found a clock net: %s\n", clkNetKey.c_str());
        uint8_t count = 0;
        for (auto& [clkNetPinKey, clkNetPin] : clkNet.map) {
            auto instance = instances->map.find(clkNetPin.instName);
            if (instance == instances->map.end()) continue;
            if (instance->second.isUsed) continue;
            if (instance->second.libCellName.find("FF") == string::npos) continue;
            if (auto ff = ff_blocks->map.find(instance->second.libCellName); ff == ff_blocks->map.end() || ff->second.bits != 1) continue;

            if (count < chosen_instances.size()) {
                chosen_instances[count++] = &instance->second;
            } else {
                break;
            }
        }

        if (count < chosen_instances.size()) continue;

        string new_ff_name = "FF40_" + string(chosen_instances[0]->instName) + "_" + string(chosen_instances[1]->instName) + "_" + string(chosen_instances[2]->instName) + "_" + string(chosen_instances[3]->instName);
        auto new_instance = make_shared<Inst>();
        new_instance->instName      = new_ff_name;
        new_instance->libCellName   = "FF40";
        new_instance->width         = 867;
        new_instance->height        = 84;
        new_instance->isUsed        = false;

        // TODO: should place at the final position
        bool canBePlaced = place_ff(chosen_instances[0], &*new_instance, &*grid);

        if (canBePlaced) {
            for (auto& inst : chosen_instances) {
                inst->isUsed = true;
//                inst = nullptr;
            }

        } else {
            for (auto& inst : chosen_instances) {
                inst->isUsed = false;
            }
            continue;
        }


        for (auto& [netKey, net] : nets->map) {
            for (auto it = net.map.begin(); it != net.map.end(); ) {
            auto& pin = it->second;
            auto pos = std::find_if(chosen_instances.begin(), chosen_instances.end(),
                                    [&](const Inst* inst) { return inst->instName == pin.instName; });

                if (pos != chosen_instances.end()) {
                    // This pin belongs to one of the old flip-flops we're replacing
                    auto index = std::distance(chosen_instances.begin(), pos);
                    string new_pin_name;

                    if (pin.libPinName.find('Q') != string::npos) {
                        new_pin_name = "Q" + to_string(index);
                    } else if (pin.libPinName.find('D') != string::npos) {
                        new_pin_name = "D" + to_string(index);
                    } else if (pin.libPinName.find("CLK") != string::npos) {
                        new_pin_name = "CLK";
                    } else {
                        cerr << "Unknown pin type: " << pin.libPinName << endl;
                        ++it;
                        continue;
                    }

                    NetPin new_pin;
                    new_pin.instName = new_ff_name;
                    new_pin.libPinName = new_pin_name;
                    new_pin.key = new_ff_name + "/" + new_pin_name;

                    // Remove the old pin and add the new one
                    it = net.map.erase(it);
                    net.map[new_pin.key] = new_pin;
                } else {
                    ++it;
                }
            }
        }

        instances->map[new_instance->instName] = *new_instance;

        for (auto& inst : chosen_instances) {
            inst->isUsed = true;
            inst = nullptr;
        }

        clk_nets->map.clear();
    }
}