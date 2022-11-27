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

    reset_saved_stats();
}

void RamInfo::update_ram_usage() {
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


void RamInfo::update_stats() {
    update_ram_usage();
}

double RamInfo::get_total_ram() const {
    return totalRamd;
}

double RamInfo::get_ram_usage() const {
    return ramUsage;
}

void RamInfo::reset_saved_stats() {
    avgUsage = 0, maxUsage = 0;
    usageCounter = 0;
}
