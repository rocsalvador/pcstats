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

    resetSavedStats();
}

double pcstats::getRamUsage() {
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

    double usedRamD;
    usedRam = ram.totalRam - freeRam;
    cachedtotalRam = cachedRam + SRecalaimable - Shmem;
    realusedRam = usedRam - (bufferRam + cachedtotalRam);
    usedRamD = realusedRam/divisor;

    ram.maxUsage = max(usedRamD, ram.maxUsage);

    ram.acumulatedUsage += usedRamD;
    ++ram.counter;

    return usedRamD;
}

double pcstats::getCpuUsage() {
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

    cpu.maxUsage = max(result, cpu.maxUsage);

    cpu.acumulatedUsage += result;
    ++cpu.usageCounter;

    return result;
}

double pcstats::getCpuFreq() {
    ifstream file;
    double freq = 0, aux;
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
    freq /= 8000;

    cpu.maxFreq = max(freq, cpu.maxFreq);

    cpu.acumulatedFreq += freq;
    ++cpu.freqCounter;

    file.close();

    return freq;
}

void pcstats::resetSavedStats() {
    ram.maxUsage = 0, cpu.maxUsage = 0, cpu.maxFreq = 0;
    ram.acumulatedUsage = 0, cpu.acumulatedUsage = 0, cpu.acumulatedFreq = 0;
    cpu.freqCounter = 0, cpu.usageCounter = 0, ram.counter = 0;
}

double pcstats::getAvgCpuUsage() {
    return cpu.acumulatedUsage/cpu.usageCounter;
}

double pcstats::getAvgRamUsage() {
    return ram.acumulatedUsage/ram.counter;
}

double pcstats::getTotalRam() {
    return ram.totalRamd;
}
