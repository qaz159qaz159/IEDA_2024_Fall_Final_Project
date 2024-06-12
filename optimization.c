#include "place.h"
#include <time.h>

double calculate_score(
    FFs* ffs, Gates* gates, Insts* insts, TimingSlacks* timing_slacks, 
    GatePowers* gate_powers, QpinDelays* qpin_delays, DisplacementDelay* displacement_delay, 
    double alpha, double beta, double gamma, double lambda, Bin* bin
) {
    double total_tns = 0.0;
    double total_power = 0.0;
    double total_area = 0.0;
    double density_penalty = 0.0;

    // Calculate TNS TODO
    for (Inst* inst = insts->map; inst != NULL; inst = (Inst*)(inst->hh.next)) {
        TimingSlack* slack;
        HASH_FIND_STR(timing_slacks->map, inst->inst_name, slack);
        if (slack) {
            if (slack->slack < 0) {
                total_tns += slack->slack;
            }
        }
    }

    // Calculate Power and Area
    for (Inst* inst = insts->map; inst != NULL; inst = (Inst*)(inst->hh.next)) {
        GatePower* power;
        HASH_FIND_STR(gate_powers->map, inst->lib_cell_name, power);
        if (power) {
            total_power += power->powerConsumption;
        }

        FF* ff;
        HASH_FIND_STR(ffs->map, inst->lib_cell_name, ff);
        if (ff) {
            total_area += (ff->width * ff->height);
        } else {
            Gate* gate;
            HASH_FIND_STR(gates->map, inst->lib_cell_name, gate);
            if (gate) {
                total_area += (gate->width * gate->height);
            }
        }
    }

    // Calculate Density Penalty
    uint32_t bin_width = bin->width;
    uint32_t bin_height = bin->height;
    double max_util = bin->maxUtil;
    double bin_area = bin_width * bin_height;
    uint32_t bin_counts = (bin_area > 0) ? (total_area / bin_area) : 0;
    if (bin_counts > max_util) {
        density_penalty = lambda * (bin_counts - max_util);
    }

    // Calculate final score
    double score = alpha * total_tns + beta * total_power + gamma * total_area + density_penalty;
    return score;
}




// Check all instances must be placed within the die region.  
int check_placement(Insts* instances, Die* die) {
    Inst* instance;
    // Need get instances' width and height
    for (instance = instances->map; instance != NULL; instance = instance->hh.next) {
        if (instance->x < die->lower_left_x || instance->x + instance->width > die->upper_right_x) {
            return 0;
        }
        if (instance->y < die->lower_left_y || instance->y + instance->height > die->upper_right_y) {
            return 0;
        }
    }
    return 1;
}



// All instances must be without overlap and placed on-site of PlacementRows.
int check_overlap(Insts* instances, PlacementsRowsSet* placement_rows_set) {
    Inst* instance;
    for (instance = instances->map; instance != NULL; instance = instance->hh.next) {
        Inst* tmp;
        for (tmp = instances->map; tmp != NULL; tmp = tmp->hh.next) {
            if (instance == tmp) continue;
            if (instance->x < tmp->x + tmp->width && instance->x + instance->width > tmp->x && instance->y < tmp->y + tmp->height && instance->y + instance->height > tmp->y) {
                printf("Overlap detected\n");
                return 0;
            }
        }
        // Check if the instance is placed on-site
        uint8_t isPlaced = 0;
        PlacementsRows* row;
        for (uint32_t i = 0; i < placement_rows_set->count; i++) {
            row = placement_rows_set->items[i];
            // The x should be the placements' x + n * site width
            for (uint32_t j = 0; j < row->totalNumOfSites; j++) {
                if (instance->x == row->start_x + j * row->width) {
                    isPlaced = 1;
                    break;
                }
            }
        }
        if (isPlaced == 0) return 0;
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
    // printf("Number of FF blocks: %d\n", HASH_COUNT(ff_blocks->map));
    // printf("Number of instances: %d\n", HASH_COUNT(instances->map));
    for (Inst* instance = instances->map; instance != NULL; instance = instance->hh.next) {
        if (strstr(instance->lib_cell_name, "FF") == NULL) continue;
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
    for (net = total_nets->map; net != NULL; net = net->hh.next) {
        for (pin = net->map; pin != NULL; pin = pin->hh.next) {
            if (strstr(pin->libPinName, "CLK") != NULL) {
                Net* clk_net = malloc(sizeof(Net));
                memcpy(clk_net, net, sizeof(Net));
                HASH_ADD_STR(clk_nets->map, name, clk_net);
                break;
            }
        }
    }
}

void bank_flip_flops(Insts* instances, Nets* nets, FFs* ff_blocks) {
    Nets* clk_nets = malloc(sizeof(Nets)); //  Get Nets of the same CLK (Only FFs have CLK pin)

    find_clk_nets(nets, clk_nets);

    printf("Before banking:\n");
    print_ff_blocks(instances, ff_blocks);
    // printf("Number of CLK nets: %d\n", HASH_COUNT(clk_nets->map));

    // Find 2 instance with the same CLK net and they are 1-bit FF
    Inst** choosed_instances = malloc(2 * sizeof(Inst*));
    choosed_instances[0] = NULL;
    choosed_instances[1] = NULL;
    Net* net = NULL;
    // printf("1\n");
    for (net = clk_nets->map; net != NULL; net = net->hh.next) {
        NetPin* pin = NULL;
        uint8_t count = 0;
    printf("2\n");

        // This for loop is to find 2 instances with the same CLK net
        for (pin = net->map; pin != NULL; pin = pin->hh.next) {
            Inst* instance = NULL;
            
            HASH_FIND_STR(instances->map, pin->instName, instance);
            // Check not used
            if (instance->isUsed == USED) continue; 
            // Check if the instance is FF
            if (strstr(instance->lib_cell_name, "FF") == NULL) continue;
            // Check 1-bit FF
            FF* ff = NULL;
            HASH_FIND_STR(ff_blocks->map, instance->lib_cell_name, ff);
            if (ff->bits != 1) continue;

            if (count == 0) choosed_instances[count++] = instance;
            else if (count == 1) choosed_instances[count++] = instance;
            else break;
        }
    printf("3\n");

        // Random choose a 1-bit FF from ff_blocks
        // TODO: build a table of FFs based on bits

        // Only use FF32, FF32 is the first 2-bit FF in the ff_blocks
        char* new_ff_name = malloc(MAX_NAME_LEN * 2);
        sprintf(new_ff_name, "FF32_%s_%s", choosed_instances[0]->inst_name, choosed_instances[1]->inst_name);
        Inst* new_instance = malloc(sizeof(Inst));
        strcpy(new_instance->inst_name, new_ff_name);
        strcpy(new_instance->lib_cell_name, "FF32");
        new_instance->x = choosed_instances[0]->x; // Use the same position as the first FF
        new_instance->y = choosed_instances[0]->y;
        new_instance->isUsed = USED;
    printf("4\n");

        // Update the nets: 1. Find all nets contain pin of choosed_instances 2. Replace the pin with new_instance
        Net* net = NULL;
        for (net = nets->map; net != NULL; net = net->hh.next) {
            NetPin* pin = NULL;
            HASH_FIND_STR(net->map, choosed_instances[0]->inst_name, pin);
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
                HASH_ADD_STR(net->map, instName, new_pin);
                HASH_DEL(net->map, pin);
            }
            HASH_FIND_STR(net->map, choosed_instances[1]->inst_name, pin);
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
                HASH_ADD_STR(net->map, instName, new_pin);
                HASH_DEL(net->map, pin);
            }
        }

        // Add new instance to new_instances
        HASH_ADD_STR(instances->map, inst_name, new_instance);
        HASH_DEL(instances->map, choosed_instances[0]);
        HASH_DEL(instances->map, choosed_instances[1]);
        choosed_instances[0] = NULL;
        choosed_instances[1] = NULL;

        // Combine the new instance to instances
        // HASH_ADD_STR(instances->map, inst_name, new_instance);
        HASH_CLEAR(hh, clk_nets->map);
        // free(choosed_instances);
    }

    printf("After banking:\n");
    print_ff_blocks(instances, ff_blocks);
    printf("Complete banking\n");

    // printf("Number of new instances: %d\n", HASH_COUNT(new_instances->map));
    // Print end time
    // time(&t);
    // printf("End time: %s", ctime(&t));
}