#ifndef PCSTATS_H
#define PCSTATS_H

#include <fstream>
#include <iostream>
#include <signal.h>
using namespace std;

class pcstats {
private:
    struct cpu_info {
        string name;
        unsigned long long  lastUser, lastNice, lastSystem, lastIdle;
        double max_freq, max_usage, avg_usage, avg_freq;
    };

    struct ram_info {
        unsigned long long totalRam;
        double totalRamd, usedRamd, avg_usage, max_usage;
    };

    cpu_info cpu;
    ram_info ram;
    const double divisor = 1048576;
    int counter;

public:
    pcstats();

    void reset_saved_stats();

    double get_ram_usage();

    double get_cpu_usage();

    double get_avg_cpu_usage();

    double get_avg_ram_usage();

    double get_cpu_freq();
};

#endif // PCSTATS_H