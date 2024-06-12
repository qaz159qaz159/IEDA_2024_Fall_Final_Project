#include "place.h"

void bank_flip_flops(Insts* instances, Nets* nets, FFs* ff_blocks);
int check_placement(Insts* instances, Die* die);
int check_overlap(Insts* instances, Insts* new_instances, PlacementsRowsSet* placement_rows_set);
double calculate_score(
    FFs* ffs, Gates* gates, Insts* insts, TimingSlacks* timing_slacks, 
    GatePowers* gate_powers, QpinDelays* qpin_delays, DisplacementDelay* displacement_delay, 
    double alpha, double beta, double gamma, double lambda, Bin* bin
);