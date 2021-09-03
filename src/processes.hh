#ifndef PROCESSES_HH
#define PROCESSES_HH

#include <filesystem>
#include <fstream>
#include <map>
#include <iostream>
using namespace std;

class processes {
private:
    struct process {
        string state;
        int threads;
    };

    map<string, process> procsInfo;

    int nProcs;
public:
    processes();

    int getNProcs() const;

    string getProcName(int i) const;

    string getProcState(int i) const;

    int getProcThreads(int i) const;

    void update();
};

#endif
