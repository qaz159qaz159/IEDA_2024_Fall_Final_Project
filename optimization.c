#include "optimization.h"
#include <time.h>



double calculate_score(
    FFs* ffs, Gates* gates, Insts* insts, TimingSlacks* timing_slacks, 
    GatePowers* gate_powers, QpinDelays* qpin_delays, DisplacementDelay* displacement_delay, 
    double alpha, double beta, double gamma, double lambda, Bin* bin, Nets* nets, Inputs* inputs, Outputs* outputs, Die* die
) {
    double total_tns        = 0.0;
    double total_power      = 0.0;
    double total_area       = 0.0;
    double density_penalty  = 0.0;

    // Calculate TNS: QpinDelay + TimingSlack
    for (Inst* inst = insts->map; inst != NULL; inst = (Inst*)(inst->hh.next)) {
        TimingSlack* slack;
        if (strstr(inst->lib_cell_name, "FF") == NULL) continue;
        HASH_FIND_STR(timing_slacks->map, inst->inst_name, slack);
        if (slack) {
            total_tns += slack->slack;
        }
        QpinDelay* qpin_delay;
        HASH_FIND_STR(qpin_delays->map, inst->lib_cell_name, qpin_delay);
        if (qpin_delay) {
            total_tns += qpin_delay->delay;
        }
    }

    // Calculate TNS: DisplacementDelay
    Net* net;
    double all_net_md = 0.0;
    for (net = nets->map; net != NULL; net = (Net*)(net->hh.next)) {
        double total_md = 0.0;
        NetPin** pins_in_net = malloc(HASH_COUNT(net->map) * sizeof(NetPin*));
        NetPin* pin;
        int i = 0;
        for (pin = net->map; pin != NULL; pin = (NetPin*)(pin->hh.next)) {
            pins_in_net[i++] = pin;
        }
        int input_type = -1; // 0: INPUT, 1: Q, 2: OUTN
        int input_idx = -1;
        for (int j = 0; j < i; j++) {
            if (strstr(pins_in_net[j]->libPinName, "INPUT") != NULL) {
                input_idx = j;
                input_type = 0;
                break;
            } else if (strstr(pins_in_net[j]->libPinName, "Q") != NULL) {
                input_idx = j;
                input_type = 1;
                break;
            } else if (strstr(pins_in_net[j]->libPinName, "OUT") != NULL) {
                input_idx = j;
                input_type = 2;
                break;
            }
        }
        if (input_idx == -1 || input_type == -1) {
            continue;
        }
        if (input_type == 0) {
            Input* inst_in;
            for (int j = 0; j < i; j++) {
                if (j == input_idx) continue;
                Inst* inst_out_tmp;
                HASH_FIND_STR(insts->map, pins_in_net[j]->instName, inst_out_tmp);
                HASH_FIND_STR(inputs->map, pins_in_net[input_idx]->instName, inst_in);
                total_md += displacement_delay->coefficient * (double)manhattan_distance(inst_in->x, inst_in->y, inst_out_tmp->x, inst_out_tmp->y);
            }
            all_net_md += total_md;
        } else if (input_type == 1 || input_type == 2) {
            Inst* inst_in;
            for (int j = 0; j < i; j++) {
                if (j == input_idx) continue;
                if (strstr(pins_in_net[j]->libPinName, "OUTPUT") != NULL) {
                    Output* inst_out_tmp;
                    HASH_FIND_STR(outputs->map, pins_in_net[j]->instName, inst_out_tmp);
                    HASH_FIND_STR(insts->map, pins_in_net[input_idx]->instName, inst_in);
                    total_md += displacement_delay->coefficient * (double)manhattan_distance(inst_in->x, inst_in->y, inst_out_tmp->x, inst_out_tmp->y);
                } else {
                    Inst* inst_out_tmp;
                    HASH_FIND_STR(insts->map, pins_in_net[j]->instName, inst_out_tmp);
                    HASH_FIND_STR(insts->map, pins_in_net[input_idx]->instName, inst_in);
                    total_md += displacement_delay->coefficient * (double)manhattan_distance(inst_in->x, inst_in->y, inst_out_tmp->x, inst_out_tmp->y);
                }
            }
            all_net_md += total_md;
        } else {
            perror("Unknown input type");
        }
    }
    printf("Total MD: %lf\n", all_net_md);
    total_tns += all_net_md;
    printf("Total TNS: %lf\n", total_tns);
    // printf("Percentage of MD: %lf\n", all_net_md / total_tns);

    // Calculate Power and Area
    for (Inst* inst = insts->map; inst != NULL; inst = (Inst*)(inst->hh.next)) {
        if (inst->isUsed == USED) continue;
        GatePower* power;
        HASH_FIND_STR(gate_powers->map, inst->lib_cell_name, power);
        // Only FFs have power consumption
        if (power && strstr(power->libCellName, "FF") != NULL) {
            total_power += power->powerConsumption;
        }

        FF* ff;
        HASH_FIND_STR(ffs->map, inst->lib_cell_name, ff);
        if (ff) {
            total_area += (ff->width * ff->height);
        }
    }

    // Calculate Density Penalty
    // TODO: Implement density penalty calculation

    // }
    // Calculate final score
    printf("Total Power: %lf\n", total_power);
    printf("Total Area: %lf\n", total_area);

    double score = alpha * total_tns + beta * total_power + gamma * total_area + density_penalty;
    return score;
}




// Check all instances must be placed within the die region.  
int check_placement(Insts* instances, Die* die) {
    Inst* instance;
    // Need get instances' width and height
    for (instance = instances->map; instance != NULL; instance = instance->hh.next) {
        if (instance->x < die->lower_left_x || instance->x + instance->width > die->upper_right_x) {
            printf("Instance %s x: %d, width: %d, y: %d, height: %d\n", instance->inst_name, instance->x, instance->width, instance->y, instance->height);
            return 0;
        }
        if (instance->y < die->lower_left_y || instance->y + instance->height > die->upper_right_y) {
            printf("Instance %s x: %d, width: %d, y: %d, height: %d\n", instance->inst_name, instance->x, instance->width, instance->y, instance->height);
            return 0;
        }
    }
    return 1;
}



// All instances must be without overlap and placed on-site of PlacementRows.
int check_overlap(Insts* instances, PlacementsRowsSet* placements_rows_set) {
    Inst* instance;
    for (instance = instances->map; instance != NULL; instance = instance->hh.next) {
        if (instance->isUsed == USED) continue;
        // Inst* tmp;
        // for (tmp = instances->map; tmp != NULL; tmp = tmp->hh.next) {
        //     if (instance == tmp) continue;
        //     if (instance->x < tmp->x + tmp->width && instance->x + instance->width > tmp->x && instance->y < tmp->y + tmp->height && instance->y + instance->height > tmp->y) {
        //         printf("Overlap detected\n");
        //         return 0;
        //     }
        // }
        // Check if the instance is placed on-site
        uint8_t isPlaced = 0;
        PlacementsRows* row, *tmp_row;
        HASH_ITER(hh, placements_rows_set->items, row, tmp_row) {
            if (instance->y == row->start_y) {
                for (uint32_t i = 0; i < row->totalNumOfSites; i++) {
                    if (instance->x == row->start_x + i * row->width) {
                        isPlaced = 1;
                        break;
                    }
                }
            }
        }
        if (isPlaced == 0) {
            printf("Instance %s is not placed on-site\n", instance->inst_name);
            return 0;
        }
    }
    return 1;
}

int check_one_overlap(Insts* instances, Inst* instance) {
    Inst* tmp;
    for (tmp = instances->map; tmp != NULL; tmp = tmp->hh.next) {
        if (instance == tmp) continue;
        if (instance->x < tmp->x + tmp->width && instance->x + instance->width > tmp->x && instance->y < tmp->y + tmp->height && instance->y + instance->height > tmp->y) {
            printf("Overlap detected\n");
            return 0;
        }
    }
    return 1;
}

// TODO: A list of banking and debanking mapping needs to be provided.

// TODO: Nets connected to the flip-flops must remain functionally equivalent to the data input. The result should not leave any open or short net. 

void print_ff_blocks(Insts* instances, FFs* ff_blocks) {
    // For all instance, print FF inst number and the number of each bits
    int one_bit_count = 0;
    int two_bit_count = 0;
    int four_bit_count = 0;
    for (Inst* instance = instances->map; instance != NULL; instance = instance->hh.next) {
        if (strstr(instance->lib_cell_name, "FF") == NULL) continue;
        if (instance->isUsed == USED) continue;
        FF* ff = NULL;
        HASH_FIND_STR(ff_blocks->map, instance->lib_cell_name, ff);
        if (ff->bits == 1) one_bit_count++;
        else if (ff->bits == 2) two_bit_count++;
        else if (ff->bits == 4) four_bit_count++;
    }
    printf("Number of 1-bit FF: %d\n", one_bit_count);
    printf("Number of 2-bit FF: %d\n", two_bit_count);
    printf("Number of 4-bit FF: %d\n", four_bit_count);
}


void find_clk_nets(Nets* total_nets, Nets* clk_nets) {
    Net* net;
    NetPin* pin;
    clk_nets->map = NULL;
    for (net = total_nets->map; net != NULL; net = net->hh.next) {
        for (pin = net->map; pin != NULL; pin = pin->hh.next) {
            if (strstr(pin->key, "CLK") != NULL) {
                Net* new_net = malloc(sizeof(Net));
                new_net->map = NULL;
                strcpy(new_net->name, net->name);
                for (NetPin* tmp = net->map; tmp != NULL; tmp = tmp->hh.next) {
                    NetPin* new_pin = malloc(sizeof(NetPin));
                    strcpy(new_pin->instName, tmp->instName);
                    strcpy(new_pin->libPinName, tmp->libPinName);
                    strcpy(new_pin->key, tmp->key);
                    HASH_ADD_STR(new_net->map, key, new_pin);
                }
                HASH_ADD_STR(clk_nets->map, name, new_net);
                break;
            }
        }
    }
    printf("Number of CLK nets: %d\n", HASH_COUNT(clk_nets->map));
}

void bank_flip_flops(Insts* instances, Nets* nets, FFs* ff_blocks, Grid* grid) {
    Nets* clk_nets = malloc(sizeof(Nets)); //  Get Nets of the same CLK (Only FFs have CLK pin)
    printf("Start banking\n");
    find_clk_nets(nets, clk_nets);

    printf("Before banking:\n");
    print_ff_blocks(instances, ff_blocks);

    // Combine 2 2-bit FFs to 1 4-bit FF
    Inst** choosed_instances = malloc(4 * sizeof(Inst*));
    choosed_instances[0] = NULL;
    choosed_instances[1] = NULL;
    choosed_instances[2] = NULL;
    choosed_instances[3] = NULL;
    Net* net = NULL;
    for (net = clk_nets->map; net != NULL; net = net->hh.next) {
        NetPin* pin = NULL;
        uint8_t count = 0;
        // printf("1\n");
        // This for loop is to find 2 instances with the same CLK net
        for (pin = net->map; pin != NULL; pin = pin->hh.next) {
            Inst* instance = NULL;
            HASH_FIND_STR(instances->map, pin->instName, instance);
            if (instance == NULL) continue; // Check why it may be NULL in the second loop
            if (instance->isUsed == USED) continue; 
            if (strstr(instance->lib_cell_name, "FF") == NULL) continue;
            FF* ff = NULL;
            HASH_FIND_STR(ff_blocks->map, instance->lib_cell_name, ff);
            if (ff->bits != 1) continue;

            if (count == 0) choosed_instances[count++] = instance;
            else if (count == 1) choosed_instances[count++] = instance;
            else if (count == 2) choosed_instances[count++] = instance;
            else if (count == 3) choosed_instances[count++] = instance;
            else break;
        }

        if (choosed_instances[0] == NULL || choosed_instances[1] == NULL || choosed_instances[2] == NULL || choosed_instances[3] == NULL) continue;
        for (int i = 0; i < 4; i++) {
            choosed_instances[i]->isUsed = USED;
        }

        // TODO: build a table of FFs based on bits
        char* new_ff_name = malloc(MAX_NAME_LEN);
        sprintf(new_ff_name, "FF40_%s_%s_%s_%s", choosed_instances[0]->inst_name, choosed_instances[1]->inst_name, choosed_instances[2]->inst_name, choosed_instances[3]->inst_name);
        Inst* new_instance = malloc(sizeof(Inst));
        strcpy(new_instance->inst_name, new_ff_name);
        strcpy(new_instance->lib_cell_name, "FF40");
        new_instance->width     = 867;
        new_instance->height    = 84;
        new_instance->isUsed    = NOT_USED;
        
        FF* ff = NULL;
        HASH_FIND_STR(ff_blocks->map, "FF40", ff);
        if (place_ff(choosed_instances[0], new_instance, grid, 10000)) {
        } else {
            for (int i = 0; i < 4; i++) {
                choosed_instances[i]->isUsed = NOT_USED;
            }
            free(new_ff_name);
            free(new_instance);
            continue;
        } 
        // printf("2\n");
        // Update the nets: 1. Find all nets contain pin of choosed_instances 2. Replace the pin with new_instance
        Net* net = NULL;
        for (net = nets->map; net != NULL; net = net->hh.next) {
            NetPin* pin = NULL;
            HASH_FIND_STR(net->map, choosed_instances[0]->inst_name, pin);
            if (pin == NULL) continue; // Check why it may be NULL in the second loop
            if (pin != NULL) {
                NetPin* new_pin = malloc(sizeof(NetPin));
                strcpy(new_pin->instName, new_ff_name);
                // Check pin type
                if (strstr(pin->libPinName, "Q") != NULL) {
                    strcpy(new_pin->libPinName, "Q0");
                } else if (strstr(pin->libPinName, "D") != NULL) {
                    strcpy(new_pin->libPinName, "D0");
                } else if (strstr(pin->libPinName, "CLK") != NULL) {
                    strcpy(new_pin->libPinName, "CLK");
                } else {
                    perror("Unknown pin type");
                }
                char* key = malloc(MAX_NAME_LEN);
                sprintf(key, "%s/%s", new_pin->instName, new_pin->libPinName);
                strcpy(new_pin->key, key);
                HASH_ADD_STR(net->map, instName, new_pin);
                HASH_DEL(net->map, pin);
            }
            HASH_FIND_STR(net->map, choosed_instances[1]->inst_name, pin);
            if (pin == NULL) continue;
            if (pin != NULL) {
                NetPin* new_pin = malloc(sizeof(NetPin));
                strcpy(new_pin->instName, new_ff_name);
                // Check pin type
                if (strstr(pin->libPinName, "Q") != NULL) {
                    strcpy(new_pin->libPinName, "Q1");
                } else if (strstr(pin->libPinName, "D") != NULL) {
                    strcpy(new_pin->libPinName, "D1");
                } else if (strstr(pin->libPinName, "CLK") != NULL) {
                    strcpy(new_pin->libPinName, "CLK");
                } else {
                    perror("Unknown pin type");
                }
                char* key = malloc(MAX_NAME_LEN);
                sprintf(key, "%s/%s", new_pin->instName, new_pin->libPinName);
                strcpy(new_pin->key, key);
                HASH_ADD_STR(net->map, instName, new_pin);
                HASH_DEL(net->map, pin);
            }
            HASH_FIND_STR(net->map, choosed_instances[2]->inst_name, pin);
            if (pin == NULL) continue;
            if (pin != NULL) {
                NetPin* new_pin = malloc(sizeof(NetPin));
                strcpy(new_pin->instName, new_ff_name);
                // Check pin type
                if (strstr(pin->libPinName, "Q") != NULL) {
                    strcpy(new_pin->libPinName, "Q2");
                } else if (strstr(pin->libPinName, "D") != NULL) {
                    strcpy(new_pin->libPinName, "D2");
                } else if (strstr(pin->libPinName, "CLK") != NULL) {
                    strcpy(new_pin->libPinName, "CLK");
                } else {
                    perror("Unknown pin type");
                }
                char* key = malloc(MAX_NAME_LEN);
                sprintf(key, "%s/%s", new_pin->instName, new_pin->libPinName);
                strcpy(new_pin->key, key);
                HASH_ADD_STR(net->map, instName, new_pin);
                HASH_DEL(net->map, pin);
            }
            HASH_FIND_STR(net->map, choosed_instances[3]->inst_name, pin);
            if (pin == NULL) continue;
            if (pin != NULL) {
                NetPin* new_pin = malloc(sizeof(NetPin));
                strcpy(new_pin->instName, new_ff_name);
                // Check pin type
                if (strstr(pin->libPinName, "Q") != NULL) {
                    strcpy(new_pin->libPinName, "Q3");
                } else if (strstr(pin->libPinName, "D") != NULL) {
                    strcpy(new_pin->libPinName, "D3");
                } else if (strstr(pin->libPinName, "CLK") != NULL) {
                    strcpy(new_pin->libPinName, "CLK");
                } else {
                    perror("Unknown pin type");
                }
                char* key = malloc(MAX_NAME_LEN);
                sprintf(key, "%s/%s", new_pin->instName, new_pin->libPinName);
                strcpy(new_pin->key, key);
                HASH_ADD_STR(net->map, instName, new_pin);
                HASH_DEL(net->map, pin);
            }
        }

        // printf("3\n");
        // Add new instance to new_instances
        HASH_ADD_STR(instances->map, inst_name, new_instance);

        for (int i = 0; i < 4; i++) {
            // HASH_DEL(instances->map, choosed_instances[i]);
            // free(choosed_instances[i]);
            choosed_instances[i]->isUsed = USED;
            choosed_instances[i] = NULL;
        }

        HASH_CLEAR(hh, clk_nets->map);
    }

    printf("After banking:\n");
    print_ff_blocks(instances, ff_blocks);
    printf("Complete banking\n");
}


Net* find_net_connected_to_pin(const char* inst_name, const char* pin_name, Nets* nets) {
    Net* net;
    for (net = nets->map; net != NULL; net = (Net*)net->hh.next) {
        NetPin* pin;
        for (pin = net->map; pin != NULL; pin = (NetPin*)pin->hh.next) {
            if (strcmp(pin->instName, inst_name) == 0 && strcmp(pin->libPinName, pin_name) == 0) {
                return net;
            }
        }
    }
    return NULL;
}

void find_ff_pairs_recursive(Inst* instance, Nets* nets, Insts* insts, FFs* ffs) {
    if (!instance) return;

    // Find the net connected to the D pin of the current instance
    Net* net = find_net_connected_to_pin(instance->inst_name, "D", nets);
    if (!net) return;

    // Traverse the net to find connected instances
    Inst* next_instance = find_instance_connected_to_net(net, "Q", insts);
    if (next_instance) {
        // Check if the next instance is a FF
        FF* next_ff;
        HASH_FIND_STR(ffs->map, next_instance->lib_cell_name, next_ff);
        if (next_ff) {
            // Found a FF pair
            // fprintf(output_file, "FF Pair: %s -> %s\n", instance->inst_name, next_instance->inst_name);
            // Continue to find pairs recursively
            find_ff_pairs_recursive(next_instance, nets, insts, ffs);
        } else {
            // If not a FF, continue searching through connected gates
            find_ff_pairs_recursive(next_instance, nets, insts, ffs);
        }
    }
}

Inst* find_instance_connected_to_net(Net* net, const char* pin_name, Insts* insts) {
    NetPin* pin;
    for (pin = net->map; pin != NULL; pin = (NetPin*)pin->hh.next) {
        if (strcmp(pin->libPinName, pin_name) == 0) {
            Inst* instance;
            HASH_FIND_STR(insts->map, pin->instName, instance);
            if (instance != NULL) {
                return instance;
            }
        }
    }
    return NULL;
}