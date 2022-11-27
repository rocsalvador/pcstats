#ifndef RAMINFO_HH
#define RAMINFO_HH

#include <fstream>
#include <iostream>
#include <vector>
using namespace std;

class RamInfo {
private:
    unsigned long long totalRam;
    int usageCounter;
    double ramUsage;
    double totalRamd, usedRamd, avgUsage, maxUsage;

    constexpr static double divisor = 1048576;

public:
    RamInfo();

    void update_ram_usage();

    void update_stats();

    double get_total_ram() const;

    double get_ram_usage() const;

    void reset_saved_stats();
};

#endif
