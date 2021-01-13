#ifndef PCSTATS_HH
#define PCSTATS_HH

#include <fstream>
#include <iostream>
#include <vector>
using namespace std;

class pcstats {
private:
    struct cpu_info {
        string name;
        int cores;
        
        double cpuUsage;
        vector<double> coresUsage;
        vector<unsigned long> coresLast;
        unsigned long long  lastUser, lastNice, lastSystem, lastIdle;
        int usageCounter;
        double maxUsage, avgUsage;
        
        double cpuFreq;
        vector<double> coresFreq;
        int freqCounter;
        double maxFreq, avgFreq;
        
        string cpuTempFolder;
        vector<pair<string,double>> coreTemps;
        int tempCounter;
        double maxTemp, avgTemp;
    };

    struct ram_info {
        unsigned long long totalRam;
        int usageCounter;
        double ramUsage;
        double totalRamd, usedRamd, avgUsage, maxUsage;
    };

    cpu_info cpu;
    ram_info ram;
    const double divisor = 1048576;

public:
    pcstats();
    
    void update_cpu_freq();
    
    void update_ram_usage();

    void update_cpu_usage();
    
    void update_cpu_temp();
    
    string cpu_name() const;

    double get_total_ram() const;
    
    double get_ram_usage() const;
    
    double get_core_usage(int core) const;
    
    pair<string,double> get_core_temp(int core) const;
    
    double get_core_freq(int core) const;
    
    int cpu_cores() const;
    
    int cpu_sensors() const;
    
    void reset_saved_stats();
};

#endif
