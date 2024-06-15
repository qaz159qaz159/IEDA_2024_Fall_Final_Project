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

bool place_ff(const shared_ptr<Inst>& oldInst, const shared_ptr<Inst>& newInst, const shared_ptr<Grid>& grid) {
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
            cellIndexTable[i][j] = grid->get_cell_index(startCellX + i, startCellY + j);
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
            newInst->x = grid->startX + (cellIndexTable[i][j] % grid->cols) * grid->gridSizeX;
            newInst->y = grid->startY + (cellIndexTable[i][j] / grid->cols) * grid->gridSizeY;
            grid->insert_to_grid(newInst);
            return true;
        }
    }
    return false;
}


void bank_flip_flops(const shared_ptr<Insts>& instances, const shared_ptr<Nets>& nets, const shared_ptr<FFs>& ff_blocks, const shared_ptr<Grid>& grid) {
    auto clk_nets = find_clk_nets(nets);

    // Combine 2 2-bit FFs to 1 4-bit FF
    vector<Inst*> chosen_instances(4, nullptr);
    for (auto& [clkNetKey, clkNet] : clk_nets->map) {
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
        new_instance->instName     = new_ff_name;
        new_instance->libCellName = "FF40";
        new_instance->width         = 867;
        new_instance->height        = 84;
        new_instance->isUsed        = false;

        // TODO: should place at the final position
        if (place_ff(static_cast<const shared_ptr<Inst>>(chosen_instances[0]), new_instance, grid)) {
            for (auto& inst : chosen_instances) {
                inst->isUsed = true;
//                inst = nullptr;
            }
            clk_nets->map.clear();
        } else {
            for (auto& inst : chosen_instances) {
                inst->isUsed = false;
            }
            continue;
        }

        for (auto& [netKey, net] : nets->map) {
            for (auto& [pinKey, pin] : net.map) {
                if (auto pos = find_if(begin(chosen_instances), end(chosen_instances), [&](Inst* inst) { return strcmp(inst->instName, pin.instName) == 0; }); pos != end(chosen_instances)) {
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

        instances->map[new_instance->instName] = *new_instance;

        for (auto& inst : chosen_instances) {
            inst->isUsed = true;
            inst = nullptr;
        }

        clk_nets->map.clear();
    }
}