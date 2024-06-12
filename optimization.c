#include "place.h"
#include <time.h>

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
                return 0;
            }
        }
        // Check if the instance is placed on-site
        uint8_t isPlaced = 0;
        PlacementsRows* row;
        for (uint32_t i = 0; i < placement_rows_set->count; i++) {
            row = placement_rows_set->items[i];
            if (instance->x >= row->start_x && instance->x + instance->width <= row->start_x + row->width && instance->y >= row->start_y && instance->y + instance->height <= row->start_y + row->height) {
                isPlaced = 1;
                break;
            }
        }
        if (isPlaced == 0) return 0;
    }
    return 1;
}

// TODO: A list of banking and debanking mapping needs to be provided.

// TODO: Nets connected to the flip-flops must remain functionally equivalent to the data input. The result should not leave any open or short net. 



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

void bank_flip_flops(Insts* instances, Nets* nets, FFs* ff_blocks, Insts* used_insts, Insts* new_insts) {
    Insts* new_instances = malloc(sizeof(Insts));
    Nets* clk_nets = malloc(sizeof(Nets)); //  Get Nets of the same CLK (Only FFs have CLK pin)

    find_clk_nets(nets, clk_nets);

    // Print start time
    // time_t t;
    // time(&t);
    // printf("Start time: %s", ctime(&t));

    // Find 2 instance with the same CLK net and they are 1-bit FF
    Inst** choosed_instances = malloc(2 * sizeof(Inst*));
    choosed_instances[0] = NULL;
    choosed_instances[1] = NULL;
    Net* net = NULL;
    for (net = clk_nets->map; net != NULL; net = net->hh.next) {
        NetPin* pin = NULL;
        uint8_t count = 0;
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

        // Random choose a 1-bit FF from ff_blocks
        // TODO: build a table of FFs based on bits

        // Only use FF32, FF32 is the first 2-bit FF in the ff_blocks
        char* new_ff_name = malloc(MAX_NAME_LEN);
        sprintf(new_ff_name, "FF32_%s_%s", choosed_instances[0]->inst_name, choosed_instances[1]->inst_name);
        Inst* new_instance = malloc(sizeof(Inst));
        strcpy(new_instance->inst_name, new_ff_name);
        strcpy(new_instance->lib_cell_name, "FF32");
        new_instance->x = choosed_instances[0]->x; // Use the same position as the first FF
        new_instance->y = choosed_instances[0]->y;
        new_instance->isUsed = USED;

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
        HASH_ADD_STR(new_instances->map, inst_name, new_instance);
        choosed_instances[0] = NULL;
        choosed_instances[1] = NULL;
    }

    // Print end time
    // time(&t);
    // printf("End time: %s", ctime(&t));
}