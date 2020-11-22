#ifndef PCSTATS_H
#define PCSTATS_H

#include <fstream>
#include <iostream>
using namespace std;

class pcstats {
private:
    struct cpu_info {
        string name;
        unsigned long long  lastUser, lastNice, lastSystem, lastIdle;
        double maxFreq, maxUsage, acumulatedUsage, acumulatedFreq;
        int freqCounter, usageCounter;
    };

    struct ram_info {
        unsigned long long totalRam;
        double totalRamd, acumulatedUsage, maxUsage;
        int counter;
    };

    cpu_info cpu;
    ram_info ram;
    const double divisor = 1048576;

public:
    pcstats();

    void resetSavedStats();

    // Returns: Ram usage in GB
    double getRamUsage();

    // Returns the total Ram in the system
    double getTotalRam();

    double getCpuUsage();

    double getCpuFreq();

    double getAvgCpuUsage();

    double getAvgFreq();

    double getAvgRamUsage();
};

#endif // PCSTATS_H
