#ifndef CPUINFO_HH
#define CPUINFO_HH

#include <fstream>
#include <iostream>
#include <vector>
using namespace std;

class CpuInfo {
private:
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


public:
    CpuInfo();

    void updateCpuFreq();

    void updateCpuUsage();

    void updateCpuTemp();

    void updateStats();

    string cpuName() const;

    double getCoreUsage(int core) const;

    pair<string,int> getCoreTemp(int core) const;

    double getCoreFreq(int core) const;

    int cpuCores() const;

    int cpuSensors() const;

    void resetSavedStats();
};

#endif
