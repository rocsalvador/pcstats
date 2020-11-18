#include "pcstats.h"

pcstats::pcstats() {
    ifstream file;
    file.open("/proc/cpuinfo");
    for(int i = 0; i < 5; ++i) {
        getline(file,cpu.name);
    }
    cpu.name.erase(0,13);
    file.close();

    file.open("/proc/meminfo");
    file.seekg(9);
    file >> ram.totalRam;
    file.close();
    ram.totalRamd = ram.totalRam/divisor;

    file.open("/proc/stat");
    file.seekg(4);
    file >> cpu.lastUser >> cpu.lastNice >> cpu.lastSystem >> cpu.lastIdle;
    file.close();

    reset_saved_stats();

    counter = 0;
}

double pcstats::get_ram_usage() {
    long long freeRam, usedRam, bufferRam, cachedtotalRam, cachedRam, SRecalaimable, Shmem, realusedRam;
    string unused;
    ifstream file;
    file.open("/proc/meminfo");
    getline(file, unused);
    file >> unused >> freeRam >> unused;
    for(int i = 0; i < 2; ++i)
        getline(file, unused);
    file >> unused >> bufferRam >> unused >>
        unused >> cachedRam >> unused;
    for(int i = 0; i < 16; ++i)
        getline(file, unused);
    file >> unused >> Shmem >> unused;
    for(int i = 0; i < 3; ++i)
        getline(file, unused);
    file >> unused >> SRecalaimable >> unused;
    file.close();
    usedRam = ram.totalRam - freeRam;
    cachedtotalRam = cachedRam + SRecalaimable - Shmem;
    realusedRam = usedRam - (bufferRam + cachedtotalRam);
    ram.usedRamd = realusedRam/divisor;
    return (realusedRam/double(ram.totalRam))*100;
}

double pcstats::get_cpu_usage() {
    unsigned long long User, Nice, System, Idle;
    ifstream file;
    file.open("/proc/stat");
    file.seekg(4);
    file >> User >> Nice >> System >> Idle;
    file.close();

    double result;
    int total = (User - cpu.lastUser) + (Nice - cpu.lastNice) + (System - cpu.lastSystem);
    result = total;
    total += (Idle - cpu.lastIdle);
    result /= total;
    result *= 100;

    cpu.lastUser = User;
    cpu.lastNice = Nice;
    cpu.lastSystem = System;
    cpu.lastIdle = Idle;

    return result;
}

double pcstats::get_cpu_freq() {
    double freq = 0, aux;
    ifstream file;
    file.open("/proc/cpuinfo");
    string useless;
    for(int i = 0; i < 7; ++i)
        getline(file,useless);
    file >> useless >> useless >> useless >> aux;
    freq += aux;
    for(int i = 0; i < 7; ++i) {
        for(int i = 0; i < 27; ++i)
            getline(file,useless);
        file >> useless >> useless >> useless >> aux;
        freq += aux;
    }
    file.close();
    return freq/8000;
}

void pcstats::reset_saved_stats() {
    ram.max_usage = 0, cpu.max_freq = 0, ram.avg_usage = 0, cpu.avg_usage = 0, cpu.avg_usage = 0, cpu.max_usage = 0;
}

double pcstats::get_avg_cpu_usage() {
    return cpu.avg_usage;
}

double pcstats::get_avg_ram_usage() {
    return ram.avg_usage;
}
