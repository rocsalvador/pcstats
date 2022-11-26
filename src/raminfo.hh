#ifndef RAMINFO_HH
#define RAMINFO_HH

#include <fstream>
#include <iostream>
#include <vector>
using namespace std;

class raminfo {
private:
    struct ram_info {
        unsigned long long totalRam;
        int usageCounter;
        double ramUsage;
        double totalRamd, usedRamd, avgUsage, maxUsage;
    };

    ram_info ram;
    constexpr static double divisor = 1048576;

public:
    raminfo();

    void update_ram_usage();

    void update_stats();

    double get_total_ram() const;

    double get_ram_usage() const;

    void reset_saved_stats();
};

#endif
