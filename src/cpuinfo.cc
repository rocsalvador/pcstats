#include "cpuinfo.hh"

CpuInfo::CpuInfo() {
    ifstream file;
    file.open("/proc/cpuinfo");
    if(not file.is_open()) {
        cout << "ERROR: Unable to open /proc/cpu file" << endl;
        exit(1);
    }

    cores = 0;
    bool first = true;
    string aux;
    while(file >> aux) {
        if(aux == "processor") ++cores;
        else if(first and aux == "model") {
            file >> aux;
            if(aux == "name") {
                first = false;
                file >> aux;
                file.seekg(1,ios_base::cur);
                getline(file, name);
            }
        }
    }
    file.close();

    coresFreq = vector<double> (cores);
    coresLast = vector<unsigned long> (cores*4);
    coresUsage = vector<double> (cores);

    file.open("/proc/stat");
    if(not file.is_open()) {
        cout << "ERROR: Unable to open /proc/stat file" << endl;
        exit(1);
    }
    file >> aux >> lastUser >> lastNice >> lastSystem >> lastIdle;
    for(int i = 0; i < cores; ++i) {
        for(int j = 0; j < 7; ++j) file >> aux;
        file >> coresLast[i*4] >> coresLast[i*4+1] >> coresLast[i*4+2] >> coresLast[i*4+3];
    }
    file.close();

    int i = 0;
    cpuTempFolder = "/sys/class/hwmon/hwmon";
    string path = cpuTempFolder;
    path.append(to_string(i));
    bool found = false;
    file.open(path + "/name");
    while(file.is_open() and not found) {
        file >> aux;
        found = (aux == "coretemp");
        file.close();
        ++i;
        path = cpuTempFolder, path.append(to_string(i));
        file.open(path + "/name");
    }
    file.close();
    if(found) {
        cpuTempFolder.append(to_string(i-1));
        i = 1;
        path = cpuTempFolder, path.append("/temp"), path.append(to_string(i));
        file.open(path + "_label");
        while(file.is_open()) {
            getline(file,aux);
            coreTemps.push_back({aux,-1});
            file.close();
            ++i;
            path = cpuTempFolder, path.append("/temp"), path.append(to_string(i));
            file.open(path + "_label");
        }
        file.close();
    }

    resetSavedStats();
}

void CpuInfo::updateCpuUsage() {
    unsigned long long User, Nice, System, Idle;
    string aux;
    ifstream file;
    file.open("/proc/stat");
    if(not file.is_open()) {
        cout << "ERROR: Unable to open /proc/stat file" << endl;
        exit(1);
    }

    file >> aux >> User >> Nice >> System >> Idle;

    int total = (User - lastUser) + (Nice - lastNice) + (System - lastSystem);
    cpuUsage = total;
    total += (Idle - lastIdle);
    if(total > 0) cpuUsage /= total;
    else cpuUsage = 0;
    cpuUsage *= 100;

    lastUser = User;
    lastNice = Nice;
    lastSystem = System;
    lastIdle = Idle;

    avgUsage += cpuUsage;
    ++usageCounter;
    maxUsage = max(maxUsage, cpuUsage);

    for(int i = 0; i < cores; ++i) {
        for(int j = 0; j < 7; ++j) file >> aux;
        file >> User >> Nice >> System >> Idle;
        total = (User - coresLast[i*4]) + (Nice - coresLast[i*4+1]) + (System - coresLast[i*4+2]);
        coresUsage[i] = total;
        total += (Idle - coresLast[i*4+3]);
        if(total > 0) coresUsage[i] /= total;
        else coresUsage[i] = 0;
        coresUsage[i] *= 100;

        coresLast[i*4] = User;
        coresLast[i*4+1] = Nice;
        coresLast[i*4+2] = System;
        coresLast[i*4+3] = Idle;
    }
    file.close();
}

void CpuInfo::updateCpuFreq() {
    ifstream file;
    file.open("/proc/cpuinfo");
    if(not file.is_open()) {
        cout << "ERROR: Unable to open /proc/cpuinfo file" << endl;
        exit(1);
    }

    double coreFreq;
    cpuFreq = 0;
    int i = 0;
    string aux;
    while(file >> aux) {
        if(aux == "cpu") {
            file >> aux;
            if(aux == "MHz") {
                file >> aux >> coreFreq;
                cpuFreq += coreFreq;
                coresFreq[i] = coreFreq;
                ++i;
            }
        }
    }
    cpuFreq /= cores;

    avgFreq += cpuFreq;
    ++freqCounter;
    maxFreq = max(maxFreq, cpuFreq);

    file.close();
}

void CpuInfo::updateCpuTemp() {
    ifstream file;
    double temp;
    int j = 1;
    string path = cpuTempFolder;
    path.append("/temp"), path.append(to_string(j));
    file.open(path + "_input");
    for(unsigned int i = 0; i < coreTemps.size(); ++i) {
        file >> temp;
        coreTemps[i].second = temp/1000;
        file.close();
        ++j;
        path = cpuTempFolder, path.append("/temp"), path.append(to_string(j));
        file.open(path + "_input");
    }
    file.close();
    ++tempCounter;
}

void CpuInfo::updateStats() {
    updateCpuFreq();
    updateCpuTemp();
    updateCpuUsage();
}

string CpuInfo::cpuName() const {
    return name;
}

int CpuInfo::cpuCores() const {
    return cores;
}

int CpuInfo::cpuSensors() const {
    return coreTemps.size();
}


double CpuInfo::getCoreFreq(int core) const {
    if(core == -1) return cpuFreq;
    else return coresFreq[core];
}

double CpuInfo::getCoreUsage(int core) const {
    if(core == -1) return cpuUsage;
    else return coresUsage[core];
}

pair<string,int> CpuInfo::getCoreTemp(int core) const {
    return coreTemps[core];
}

void CpuInfo::resetSavedStats() {
    maxFreq = 0, avgUsage = 0, avgUsage = 0, maxUsage = 0;
    usageCounter = 0, freqCounter = 0;
    maxTemp = 0, avgTemp = 0, tempCounter = 0;
}
