#include "pcstats.hh"

pcstats::pcstats()
{
    ifstream file;
    file.open("/proc/cpuinfo");
    for(int i = 0; i < 5; ++i)
    {
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

void pcstats::print_stats()
{
    double ram_usage = get_ram_usage(), cpu_usage = get_cpu_usage(), cpu_freq = get_cpu_freq();
    ram.avg_usage += ram.usedRamd, cpu.avg_usage += cpu_usage, cpu.avg_freq += cpu_freq;
    cpu.max_usage = max(cpu.max_usage, cpu_usage), ram.max_usage = max(ram.max_usage, ram.usedRamd), cpu.max_freq = max(cpu_freq, cpu.max_freq);

    system("clear");
    cout << "CPU model: " << cpu.name << endl << endl;
    
    cout << "CPU usage:" << endl << "[";
    for(int i = 0; i < cpu_usage; ++i)
        cout << "#";
    for(int i = 0; i < 100-cpu_usage; ++i)
        cout << " ";
    cout << "] " << cpu_usage << "%" << endl;
    cout << "CPU frequency: " << cpu_freq << " GHz" << endl << endl;
    
    cout << "RAM usage:" << endl << "[";
    for(int i = 0; i < ram_usage; ++i)
        cout << "#";
    for(int i = 0; i < 100-ram_usage; ++i)
        cout << " ";
    cout << "] " << ram_usage << "%" << endl;
    cout << "[" << ram.usedRamd << "/" << ram.totalRamd << "] GB" << endl << endl;
    
    cout << "Refreshing every " << time << "s" << endl;
    cout << "Ctrl+C to stop" << endl;
    ++counter;
}

double pcstats::get_ram_usage()
{
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

double pcstats::get_cpu_usage()
{
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

double pcstats::get_cpu_freq()
{
    double freq = 0, aux;
    ifstream file;
    file.open("/proc/cpuinfo");
    string useless;
    for(int i = 0; i < 7; ++i) getline(file,useless);
    file >> useless >> useless >> useless >> aux;
    freq += aux;
    for(int i = 0; i < 7; ++i)
    {
        for(int i = 0; i < 27; ++i) 
            getline(file,useless);
        file >> useless >> useless >> useless >> aux;
        freq += aux;
    }
    file.close();
    return freq/8000;
}

void pcstats::reset_saved_stats()
{
    ram.max_usage = 0, cpu.max_freq = 0, ram.avg_usage = 0, cpu.avg_usage = 0, cpu.avg_usage = 0, cpu.max_usage = 0;
}

void pcstats::print_saved_stats()
{
    system("clear");
    cpu.avg_usage /= counter, cpu.avg_freq /= counter, ram.avg_usage /= counter;
    cout << "Average cpu usage: " << cpu.avg_usage << " %" << endl;
    cout << "Max cpu usage: " << cpu.max_usage << " %" << endl;
    cout << "Average cpu freq: " << cpu.avg_freq << " GHz" << endl;
    cout << "Max cpu freq: " << cpu.max_freq << " GHz" << endl;
    cout << "Average ram usage: " << ram.avg_usage << " GB" << endl;
    cout << "Max ram usage: " << ram.max_usage << " GB" << endl;
}