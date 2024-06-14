#ifndef READ_INPUT_H
#define READ_INPUT_H

#include "place.h"
#include "quadtree.h"
#include "grid.h"

int read_input(
    // Input file name
    const char*         filename, 
    // Parameters
    double*             alpha, 
    double*             beta, 
    double*             gamma, 
    double*             lambda, 
    Die*                die,
    // Input and Output
    int*                num_inputs,
    Inputs*             inputs,
    int*                num_outputs,
    Outputs*            outputs,
    // FF and Gate blocks
    FFs*                ff_blocks, 
    Gates*              gate_blocks,
    // Instances
    Insts*              instances, 
    // Nets
    Nets*               nets,
    // Bin
    Bin*                bin,
    // Placements
    PlacementsRowsSet*  placements_rows_set,
    // DisplacementDelay
    DisplacementDelay*  displacement_delay,
    // QpinDelay
    QpinDelays*         qpin_delay,
    // TimingSlack
    TimingSlacks*       timing_slack,
    // GatePower
    GatePowers*         gate_power,
    // Grid
    Grid*               grid
    );


#endif // READ_INPUT_H