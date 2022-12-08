#ifndef PROCESSES_HH
#define PROCESSES_HH

#include <filesystem>
#include <fstream>
#include <map>
#include <iostream>
#include <vector>
using namespace std;

class ProcessInfo {
private:
    struct process {
        string name;
        string state;
        int threads;
        int pid;
        double readKB;
        double writeKB;
        double lastWriteKB;
        double lastReadKB;
        double cpuUsage;
        double lastUTime;
        double lastSTime;
        double lastSysUptime;
        double memUsage;
    };

    double lastSysUptime = 0;

    map<string, process*> procsNameMap;
    map<int, process*> procsPidMap;

public:
    ProcessInfo();

    int getNProcs() const;

    string getProcName(int i) const;

    string getProcState(int i) const;

    int getProcThreads(int i) const;

    int getProcPid(int i) const;
    
    double getWriteKB(int i) const;
    
    double getReadKB(int i) const;

    double getCpuUsage(int i) const;

    double getMemUsage(int i) const;

    int getProcIndex(string procName) const;

    process getProcByPid(int pid) const;

    process getProcByName(string name) const;

    void update();
};

#endif
