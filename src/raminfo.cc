#include "raminfo.hh"

raminfo::raminfo() {
    ifstream file;
    string aux;
    file.open("/proc/meminfo");
    if(not file.is_open()) {
        cout << "ERROR: Unable to open /proc/meminfo file" << endl;
        exit(1);
    }
    while(file >> aux and aux != "MemTotal:");
    file >> ram.totalRam;
    file.close();
    ram.totalRamd = ram.totalRam/divisor;

    reset_saved_stats();
}

void raminfo::update_ram_usage() {
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

    usedRam = ram.totalRam - freeRam;
    cachedtotalRam = cachedRam + SRecalaimable - Shmem;
    realusedRam = usedRam - (bufferRam + cachedtotalRam);
    ram.usedRamd = realusedRam/divisor;
    ram.ramUsage = (realusedRam/double(ram.totalRam))*100;

    ram.avgUsage += ram.usedRamd;
    ++ram.usageCounter;
    ram.maxUsage = max(ram.maxUsage, ram.usedRamd);
}


void raminfo::update_stats() {
    update_ram_usage();
}

double raminfo::get_total_ram() const {
    return ram.totalRamd;
}

double raminfo::get_ram_usage() const {
    return ram.ramUsage;
}

void raminfo::reset_saved_stats() {
    ram.avgUsage = 0, ram.maxUsage = 0;
    ram.usageCounter = 0;
}
