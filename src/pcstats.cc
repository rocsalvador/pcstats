#include "pcstats.hh"

pcstats::pcstats() {
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
                file.seekg(1,ios_base::cur);
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
    while(file >> aux and aux != "MemTotal:");
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
    
    int i = 0;
    cpu.cpuTempFolder = "/sys/class/hwmon/hwmon";
    string path = cpu.cpuTempFolder;
    path.append(to_string(i));
    bool found = false;
    file.open(path + "/name");
    while(file.is_open() and not found) {
        file >> aux;
        found = (aux == "coretemp");
        file.close();
        ++i;
        path = cpu.cpuTempFolder, path.append(to_string(i));
        file.open(path + "/name");
    }
    file.close();
    if(found) {
        cpu.cpuTempFolder.append(to_string(i-1));
        i = 1;
        path = cpu.cpuTempFolder, path.append("/temp"), path.append(to_string(i));
        file.open(path + "_label");
        while(file.is_open()) {
            getline(file,aux);
            cpu.coreTemps.push_back({aux,-1});
            file.close();
            ++i;
            path = cpu.cpuTempFolder, path.append("/temp"), path.append(to_string(i));
            file.open(path + "_label");
        }
        file.close();
    }
    
    reset_saved_stats();
}

void pcstats::update_ram_usage() {
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

void pcstats::update_cpu_usage() {
    unsigned long long User, Nice, System, Idle;
    string aux;
    ifstream file;
    file.open("/proc/stat");
    if(not file.is_open()) {
        cout << "ERROR: Unable to open /proc/stat file" << endl;
        exit(1);
    }
    
    file >> aux >> User >> Nice >> System >> Idle;
    
    int total = (User - cpu.lastUser) + (Nice - cpu.lastNice) + (System - cpu.lastSystem);
    cpu.cpuUsage = total;
    total += (Idle - cpu.lastIdle);
    if(total > 0) cpu.cpuUsage /= total;
    else cpu.cpuUsage = 0;
    cpu.cpuUsage *= 100;
    
    cpu.lastUser = User;
    cpu.lastNice = Nice;
    cpu.lastSystem = System;
    cpu.lastIdle = Idle;
    
    cpu.avgUsage += cpu.cpuUsage;
    ++cpu.usageCounter;
    cpu.maxUsage = max(cpu.maxUsage, cpu.cpuUsage);
    
    for(int i = 0; i < cpu.cores; ++i) {
        for(int j = 0; j < 7; ++j) file >> aux;
        file >> User >> Nice >> System >> Idle;
        total = (User - cpu.coresLast[i*4]) + (Nice - cpu.coresLast[i*4+1]) + (System - cpu.coresLast[i*4+2]);
        cpu.coresUsage[i] = total;
        total += (Idle - cpu.coresLast[i*4+3]);
        if(total > 0) cpu.coresUsage[i] /= total;
        else cpu.coresUsage[i] = 0;
        cpu.coresUsage[i] *= 100;
        
        cpu.coresLast[i*4] = User;
        cpu.coresLast[i*4+1] = Nice;
        cpu.coresLast[i*4+2] = System;
        cpu.coresLast[i*4+3] = Idle;
    }
    file.close();
}

void pcstats::update_cpu_freq() {
    ifstream file;
    file.open("/proc/cpuinfo");
    if(not file.is_open()) {
        cout << "ERROR: Unable to open /proc/cpuinfo file" << endl;
        exit(1);
    }
    
    double coreFreq;
    cpu.cpuFreq = 0;
    int i = 0;
    string aux;
    while(file >> aux) {
        if(aux == "cpu") {
            file >> aux;
            if(aux == "MHz") {
                file >> aux >> coreFreq;
                cpu.cpuFreq += coreFreq;
                cpu.coresFreq[i] = coreFreq;
                ++i;
            }
        }
    }
    cpu.cpuFreq /= cpu.cores;
    
    cpu.avgFreq += cpu.cpuFreq;
    ++cpu.freqCounter;
    cpu.maxFreq = max(cpu.maxFreq, cpu.cpuFreq);
    
    file.close();
}

void pcstats::update_cpu_temp() {
    ifstream file;
    double temp;
    int j = 1;
    string path = cpu.cpuTempFolder;
    path.append("/temp"), path.append(to_string(j));
    file.open(path + "_input");
    for(unsigned int i = 0; i < cpu.coreTemps.size(); ++i) {
        file >> temp;
        cpu.coreTemps[i].second = temp/1000;
        file.close();
        ++j;
        path = cpu.cpuTempFolder, path.append("/temp"), path.append(to_string(j));
        file.open(path + "_input");
    }
    file.close();
    ++cpu.tempCounter;
}

void pcstats::update_stats() {
    update_cpu_freq();
    update_cpu_temp();
    update_cpu_usage();
    update_ram_usage();
}

string pcstats::cpu_name() const {
    return cpu.name;
}

int pcstats::cpu_cores() const {
    return cpu.cores;
}

int pcstats::cpu_sensors() const {
    return cpu.coreTemps.size();
}

double pcstats::get_total_ram() const {
    return ram.totalRamd;
}

double pcstats::get_ram_usage() const {
    return ram.ramUsage;
}

double pcstats::get_core_freq(int core) const {
    if(core == -1) return cpu.cpuFreq;
    else return cpu.coresFreq[core];
}

double pcstats::get_core_usage(int core) const {
    if(core == -1) return cpu.cpuUsage;
    else return cpu.coresUsage[core];
}

pair<string,int> pcstats::get_core_temp(int core) const {
    return cpu.coreTemps[core];
}

void pcstats::reset_saved_stats() {
    cpu.maxFreq = 0, cpu.avgUsage = 0, cpu.avgUsage = 0, cpu.maxUsage = 0;
    ram.avgUsage = 0, ram.maxUsage = 0;
    ram.usageCounter = 0;
    cpu.usageCounter = 0, cpu.freqCounter = 0;
    cpu.maxTemp = 0, cpu.avgTemp = 0, cpu.tempCounter = 0;
}
