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

    void updateRamUsage();

    void updateStats();

    double getTotalRam() const;

    double getRamUsage() const;

    void resetSavedStats();
};

#endif
