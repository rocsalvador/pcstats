#include "pcstats.hh"

pcstats::pcstats()
{
    ifstream file;
    file.open("/proc/cpuinfo");
    if(not file.is_open()) {
        cout << "ERROR: Unable to open /proc/cpu file" << endl;
        exit(1);
    }
    
    cpu.cores = 0;
    bool first = true;
    string aux;
    while(file >> aux) {
        if(aux == "processor") ++cpu.cores;
        else if(first and aux == "model") {
            file >> aux;
            if(aux == "name") {
                first = false;
                file >> aux;
                getline(file,cpu.name);
            }
        }
    }
    file.close();

    file.open("/proc/meminfo");
    if(not file.is_open()) {
        cout << "ERROR: Unable to open /proc/meminfo file" << endl;
        exit(1);
    }
    file.seekg(9);
    file >> ram.totalRam;
    file.close();
    ram.totalRamd = ram.totalRam/divisor;
    
    cpu.coresFreq = vector<double> (cpu.cores);
    cpu.coresLast = vector<unsigned long> (cpu.cores*4);
    cpu.coresUsage = vector<double> (cpu.cores);
    
    file.open("/proc/stat");
    if(not file.is_open()) {
        cout << "ERROR: Unable to open /proc/stat file" << endl;
        exit(1);
    }
    file >> aux >> cpu.lastUser >> cpu.lastNice >> cpu.lastSystem >> cpu.lastIdle;
    for(int i = 0; i < cpu.cores; ++i) {
        for(int j = 0; j < 7; ++j) file >> aux;
        file >> cpu.coresLast[i*4] >> cpu.coresLast[i*4+1] >> cpu.coresLast[i*4+2] >> cpu.coresLast[i*4+3];
    }
    file.close();
    
    reset_saved_stats();
}

void pcstats::print_stats() {
    double ram_usage = get_ram_usage(), cpu_usage = get_cpu_usage(), cpu_freq = get_cpu_freq();
    
    system("clear");
    cout << "CPU model: " << cpu.name << endl << endl;
    
    cout << "CPU usage:" << endl << "CPU  [";
    for(int i = 0; i < int(cpu_usage); ++i) cout << "#";
    for(int i = 0; i < 100-int(cpu_usage); ++i) cout << " ";
    cout << "] " << cpu_usage << "%" << endl;
    
    for(int i = 0; i < cpu.cores; ++i) {
        cout << "CPU" << i << " [";
        for(int j = 0; j < int(cpu.coresUsage[i]); ++j) cout << "#";
        for(int j = 0; j < 100-int(cpu.coresUsage[i]); ++j) cout << " ";
        cout << "] " << cpu.coresUsage[i] << "%" << endl;
    }
    cout << endl;
    
    cout << "CPU frequency: " << cpu_freq << " MHz" << endl;
    for(int i = 0; i < cpu.cores; ++i) cout << "CPU" << i << " frequency: " << cpu.coresFreq[i] << " MHz" << endl;
    cout << endl;
    
    cout << "RAM usage:" << endl << "[";
    for(int i = 0; i < int(ram_usage); ++i) cout << "#";
    for(int i = 0; i < 100-int(ram_usage); ++i) cout << " ";
    cout << "] " << ram_usage << "%" << endl;
    cout << "[" << ram.usedRamd << "/" << ram.totalRamd << "] GB" << endl << endl;
    
    cout << "Refreshing every " << time << "s" << endl;
    cout << "Ctrl+C to stop" << endl;
}

double pcstats::get_ram_usage() {
    ifstream file;
    file.open("/proc/meminfo");
    if(not file.is_open()) {
        cout << "ERROR: Unable to open /proc/meminfo file" << endl;
        exit(1);
    }
    
    long long freeRam, usedRam, bufferRam, cachedtotalRam, cachedRam, SRecalaimable, Shmem, realusedRam;
    string aux;
    double ramUsage;

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
    ramUsage = (realusedRam/double(ram.totalRam))*100;
    
    ram.avgUsage += ram.usedRamd;
    ++ram.usageCounter;
    ram.maxUsage = max(ram.maxUsage, ram.usedRamd);
    
    return ramUsage;
}

double pcstats::get_cpu_usage() {
    unsigned long long User, Nice, System, Idle;
    string aux;
    ifstream file;
    file.open("/proc/stat");
    if(not file.is_open()) {
        cout << "ERROR: Unable to open /proc/stat file" << endl;
        exit(1);
    }
    
    file >> aux >> User >> Nice >> System >> Idle;
    
    double cpuUsage;
    int total = (User - cpu.lastUser) + (Nice - cpu.lastNice) + (System - cpu.lastSystem);
    cpuUsage = total;
    total += (Idle - cpu.lastIdle);
    cpuUsage /= total;
    cpuUsage *= 100;
    
    cpu.lastUser = User;
    cpu.lastNice = Nice;
    cpu.lastSystem = System;
    cpu.lastIdle = Idle;
    
    cpu.avgUsage += cpuUsage;
    ++cpu.usageCounter;
    cpu.maxUsage = max(cpu.maxUsage, cpuUsage);
    
    for(int i = 0; i < cpu.cores; ++i) {
        for(int j = 0; j < 7; ++j) file >> aux;
        file >> User >> Nice >> System >> Idle;
        total = (User - cpu.coresLast[i*4]) + (Nice - cpu.coresLast[i*4+1]) + (System - cpu.coresLast[i*4+2]);
        cpu.coresUsage[i] = total;
        total += (Idle - cpu.coresLast[i*4+3]);
        cpu.coresUsage[i] /= total;
        cpu.coresUsage[i] *= 100;
        
        cpu.coresLast[i*4] = User;
        cpu.coresLast[i*4+1] = Nice;
        cpu.coresLast[i*4+2] = System;
        cpu.coresLast[i*4+3] = Idle;
    }
    file.close();
    
    return cpuUsage;
}

double pcstats::get_cpu_freq() {
    ifstream file;
    file.open("/proc/cpuinfo");
    if(not file.is_open()) {
        cout << "ERROR: Unable to open /proc/cpuinfo file" << endl;
        exit(1);
    }
    
    double freq = 0, coreFreq;
    int i = 0;
    string aux;
    while(file >> aux) {
        if(aux == "cpu") {
            file >> aux;
            if(aux == "MHz") {
                file >> aux >> coreFreq;
                freq += coreFreq;
                cpu.coresFreq[i] = coreFreq;
                ++i;
            }
        }
    }
    freq /= cpu.cores;
    
    cpu.avgFreq += freq;
    ++cpu.freqCounter;
    cpu.maxFreq = max(cpu.maxFreq, freq);
    
    file.close();
    return freq;
}

void pcstats::reset_saved_stats() {
    cpu.maxFreq = 0, cpu.avgUsage = 0, cpu.avgUsage = 0, cpu.maxUsage = 0;
    ram.avgUsage = 0, ram.maxUsage = 0;
    ram.usageCounter = 0;
    cpu.usageCounter = 0, cpu.freqCounter = 0;
}

void pcstats::print_saved_stats() {
    system("clear");
    cpu.avgUsage /= cpu.usageCounter, cpu.avgFreq /= cpu.freqCounter, ram.avgUsage /= ram.usageCounter;
    cout << "Average cpu usage: " << cpu.avgUsage << " %" << endl;
    cout << "Max cpu usage: " << cpu.maxUsage << " %" << endl;
    cout << "Average cpu frequency: " << cpu.avgFreq << " MHz" << endl;
    cout << "Max cpu frequency: " << cpu.maxFreq << " MHz" << endl;
    cout << "Average ram usage: " << ram.avgUsage << " GB" << endl;
    cout << "Max ram usage: " << ram.maxUsage << " GB" << endl;
}
