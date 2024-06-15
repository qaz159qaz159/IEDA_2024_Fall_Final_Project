//
// Created by Ming-Hao Hsu on 2024/6/15.
//

#ifndef IEDA_FINAL_PROJECT_STRUCTURE_H
#define IEDA_FINAL_PROJECT_STRUCTURE_H

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <unordered_map>
#include <string>

using namespace std;

class Pin {
public:
    string name;
    uint16_t x;
    uint16_t y;

    Pin() : name(), x(), y() {}
};

class IO {
public:
    string name;
    uint32_t x;
    uint32_t y;

    IO() : name(), x(), y() {}
};

using Input = IO;
using Output = IO;

class IOs {
public:
    uint32_t count;
    unordered_map<string, IO> map;

    IOs() : count(0) {}
};

using Inputs = IOs;
using Outputs = IOs;

class FF {
public:
    string name;
    uint16_t bits;
    uint16_t width;
    uint16_t height;
    uint16_t pin_count;
    unordered_map<string, Pin> map;

    FF() : name(), bits(0), width(0), height(0), pin_count(0) {}
};

class Gate {
public:
    string name;
    uint16_t width;
    uint16_t height;
    uint16_t pin_count;
    unordered_map<string, Pin> map;

    Gate() : name(), width(0), height(0), pin_count(0) {}
};

class FFs {
public:
    uint32_t count;
    unordered_map<string, FF> map;

    FFs() : count(0) {}
};

class Gates {
public:
    uint32_t count;
    unordered_map<string, Gate> map;

    Gates() : count(0) {}
};

class Inst {
public:
    string inst_name;
    string lib_cell_name;
    uint32_t x;
    uint32_t y;
    uint32_t width;
    uint32_t height;
    bool isUsed;

    Inst() : inst_name(), lib_cell_name(), x(0), y(0), width(0), height(0), isUsed(false) {}
};

class Insts {
public:
    uint32_t count;
    unordered_map<string, Inst> map;

    Insts() : count(0) {}
};

class NetPin {
public:
    string instName;
    string libPinName;
    string key;

    NetPin() : instName(), libPinName(), key() {}
};

class Net {
public:
    string name;
    int pinCount;
    unordered_map<string, NetPin> map;

    Net() : name(), pinCount(0) {}
};

class Nets {
public:
    uint32_t count;
    unordered_map<string, Net> map;

    Nets() : count(0) {}
};

class PlacementsRows {
public:
    uint32_t start_x;
    uint32_t start_y;
    uint32_t width;
    uint32_t height;
    uint32_t totalNumOfSites;

    PlacementsRows() : start_x(0), start_y(0), width(0), height(0), totalNumOfSites(0) {}
};

class PlacementsRowsSet {
public:
    uint32_t count;
    unordered_map<string, PlacementsRows> items;

    PlacementsRowsSet() : count(0) {}
};

class QpinDelay {
public:
    string libCellName;
    double delay;

    QpinDelay() : libCellName(), delay(0.0) {}
};

class QpinDelays {
public:
    uint32_t count;
    unordered_map<string, QpinDelay> map;

    QpinDelays() : count(0) {}
};

class TimingSlack {
public:
    string instanceCellName;
    string pinName;
    double slack;

    TimingSlack() : instanceCellName(), pinName(), slack(0.0) {}
};

class TimingSlacks {
public:
    uint32_t count;
    unordered_map<string, TimingSlack> map;

    TimingSlacks() : count(0) {}
};

class GatePower {
public:
    string libCellName;
    double powerConsumption;

    GatePower() : libCellName(), powerConsumption(0.0) {}
};

class GatePowers {
public:
    uint32_t count;
    unordered_map<string, GatePower> map;

    GatePowers() : count(0) {}
};

class DisplacementDelay {
public:
    double coefficient;

    DisplacementDelay() : coefficient(0.0) {}
};

class Bin {
public:
    uint32_t width;
    uint32_t height;
    double maxUtil;

    Bin() : width(0), height(0), maxUtil(0.0) {}
};

class InstNetMapping {
public:
    string instName;
    Net *net;

    InstNetMapping() : instName(), net(nullptr) {}
};

class Die {
public:
    uint32_t lower_left_x;
    uint32_t lower_left_y;
    uint32_t upper_right_x;
    uint32_t upper_right_y;
    uint32_t width;
    uint32_t height;

    Die() : lower_left_x(0), lower_left_y(0), upper_right_x(0), upper_right_y(0), width(0), height(0) {}
};

#endif //IEDA_FINAL_PROJECT_STRUCTURE_H