#include <fstream>
#include <iostream>
#include <signal.h>
using namespace std;

class pcstats {
private:
    struct cpu_info {
        string name;
        unsigned long long  lastUser, lastNice, lastSystem, lastIdle;
        int usageCounter, freqCounter;
        double maxFreq, maxUsage, avgUsage, avgFreq;
    };

    struct ram_info {
        unsigned long long totalRam;
        int usageCounter;
        double totalRamd, usedRamd, avgUsage, maxUsage;
    };

    cpu_info cpu;
    ram_info ram;
    const double divisor = 1048576;

public:
    pcstats();

    void print_stats();

    void reset_saved_stats();

    void print_saved_stats();

    double get_ram_usage();

    double get_cpu_usage();

    double get_cpu_freq();
};
