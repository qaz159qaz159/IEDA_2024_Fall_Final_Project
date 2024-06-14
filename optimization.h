#ifndef OPTIMIZATION_H
#define OPTIMIZATION_H

#include "place.h"
#include "constraints.h"
#include "grid.h"
#include "utils.h"

void bank_flip_flops(Insts* instances, Nets* nets, FFs* ff_blocks, Grid* grid);
int check_placement(Insts* instances, Die* die);
int check_overlap(Insts* instances, PlacementsRowsSet* placements_rows_set);
int check_one_overlap(Insts* instances, Inst* instance);
double calculate_score(
    FFs* ffs, Gates* gates, Insts* insts, TimingSlacks* timing_slacks, 
    GatePowers* gate_powers, QpinDelays* qpin_delays, DisplacementDelay* displacement_delay, 
    double alpha, double beta, double gamma, double lambda, Bin* bin, Nets* nets, Inputs* inputs, Outputs* outputs, Die* die
);
Net* find_net_connected_to_pin(const char* inst_name, const char* pin_name, Nets* nets);
void find_ff_pairs_recursive(Inst* instance, Nets* nets, Insts* insts, FFs* ffs);
Inst* find_instance_connected_to_net(Net* net, const char* pin_name, Insts* insts);

#endif // OPTIMIZATION_H