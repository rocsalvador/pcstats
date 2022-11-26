#ifndef CPUINFO_HH
#define CPUINFO_HH

#include <fstream>
#include <iostream>
#include <vector>
using namespace std;

class cpuinfo {
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
        vector<pair<string,int>> coreTemps;
        int tempCounter;
        double maxTemp, avgTemp;
    };


    cpu_info cpu;

public:
    cpuinfo();

    void updateCpuFreq();

    void updateCpuUsage();

    void update_cpu_temp();

    void update_stats();

    string cpu_name() const;

    double get_core_usage(int core) const;

    pair<string,int> get_core_temp(int core) const;

    double get_core_freq(int core) const;

    int cpu_cores() const;

    int cpu_sensors() const;

    void reset_saved_stats();
};

#endif
