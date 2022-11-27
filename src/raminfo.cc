#include "raminfo.hh"

RamInfo::RamInfo() {
    ifstream file;
    string aux;
    file.open("/proc/meminfo");
    if(not file.is_open()) {
        cout << "ERROR: Unable to open /proc/meminfo file" << endl;
        exit(1);
    }
    while(file >> aux and aux != "MemTotal:");
    file >> totalRam;
    file.close();
    totalRamd = totalRam/divisor;

    resetSavedStats();
}

void RamInfo::updateRamUsage() {
    ifstream file;
    file.open("/proc/meminfo");
    if(not file.is_open()) {
        cout << "ERROR: Unable to open /proc/meminfo file" << endl;
        exit(1);
    }

    long long freeRam, usedRam, bufferRam, cachedtotalRam, cachedRam, SRecalaimable, Shmem, realusedRam;
    string aux;

    while(file >> aux and aux != "MemFree:");
    file >> freeRam;
    while(file >> aux and aux != "Buffers:");
    file >> bufferRam;
    while(file >> aux and aux != "Cached:");
    file >> cachedRam;
    while(file >> aux and aux != "Shmem:");
    file >> Shmem;
    while(file >> aux and aux != "SReclaimable:");
    file >> SRecalaimable;
    file.close();

    usedRam = totalRam - freeRam;
    cachedtotalRam = cachedRam + SRecalaimable - Shmem;
    realusedRam = usedRam - (bufferRam + cachedtotalRam);
    usedRamd = realusedRam/divisor;
    ramUsage = (realusedRam/double(totalRam))*100;

    avgUsage += usedRamd;
    ++usageCounter;
    maxUsage = max(maxUsage, usedRamd);
}


void RamInfo::updateStats() {
    updateRamUsage();
}

double RamInfo::getTotalRam() const {
    return totalRamd;
}

double RamInfo::getRamUsage() const {
    return ramUsage;
}

void RamInfo::resetSavedStats() {
    avgUsage = 0, maxUsage = 0;
    usageCounter = 0;
}
