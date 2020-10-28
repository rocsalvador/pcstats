#include <fstream>
#include <iostream>
#include <unistd.h>
#include <signal.h>
using namespace std;

string cpu_name;
unsigned long long totalRam, lastUser, lastNice, lastSystem, lastIdle, realusedRam;
const double divisor = 1048576;

void sigint_treatment(int s)
{
    char op;
    cout << " q to exit, any else key to resume: ";
    cin >> op;
    if(op == 'q')
        exit(0);
    else
        return;
}

void init()
{
    ifstream file;
    file.open("/proc/cpuinfo");
    for(int i = 0; i < 5; ++i)
    {
        getline(file,cpu_name);
    }
    cpu_name.erase(0,13);
    file.close();
    
    file.open("/proc/meminfo");
    file.seekg(9);
    file >> totalRam;
    file.close();
    
    file.open("/proc/stat");
    file.seekg(4);
    file >> lastUser >> lastNice >> lastSystem >> lastIdle;
    file.close();

    struct sigaction sa;
    sa.sa_flags = 0;
    sa.sa_handler = &sigint_treatment;
    sigaction(SIGINT, &sa, NULL);
}

double get_ram_usage()
{
    long long freeRam, usedRam, bufferRam, cachedtotalRam, cachedRam, SRecalaimable, Shmem;
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
    usedRam = totalRam - freeRam;
    cachedtotalRam = cachedRam + SRecalaimable - Shmem;
    realusedRam = usedRam - (bufferRam + cachedtotalRam);
    return (realusedRam/double(totalRam))*100;
}

double get_cpu_usage()
{
    unsigned long long User, Nice, System, Idle;
    ifstream file;
    file.open("/proc/stat");
    file.seekg(4);
    file >> User >> Nice >> System >> Idle;
    file.close();
    
    double result;
    int total = (User - lastUser) + (Nice - lastNice) + (System - lastSystem);
    result = total;
    total += (Idle - lastIdle);
    result /= total;
    result *= 100;
    
    lastUser = User;
    lastNice = Nice;
    lastSystem = System;
    lastIdle = Idle;
    
    return result;
}

double get_cpu_freq()
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
        for(int i = 0; i < 27; ++i) getline(file,useless);
        file >> useless >> useless >> useless >> aux;
        freq += aux;
    }
    file.close();
    return freq/8000;
}

int main()
{
    double time;
    system("clear");
    cout << "Refresh rate (s): ";
    cin >> time;
    if(time <= 0)
    {
        cout << "ERROR: Refresh rate must be > 0" << endl;
        return 0;
    }
    time *= 1000000;
    init();
    while(true)
    {
        usleep(time);
        double ram_usage = get_ram_usage(), cpu_usage = get_cpu_usage(), cpu_freq = get_cpu_freq();
        system("clear");
        
        cout << "CPU model: " << cpu_name << endl << endl;
        
        cout << "CPU usage:" << endl << "[";
        for(int i = 0; i < int(cpu_usage); ++i) cout << "#";
        for(int i = cpu_usage; i < 100; ++i) cout << " ";
        cout << "] " << cpu_usage << "%" << endl;
        cout << "CPU frequency: " << cpu_freq << " GHz" << endl << endl;
        
        cout << "RAM usage:" << endl << "[";
        for(int i = 0; i < int(ram_usage); ++i) cout << "#";
        for(int i = ram_usage; i < 100; ++i) cout << " ";
        cout << "] " << ram_usage << "%" << endl;
        cout << "[" << realusedRam/divisor << "/" << totalRam/divisor << "] GB" << endl << endl;
        
        cout << "Refreshing every " << time/1000000 << "s" << endl;
        cout << "Ctrl+C to stop" << endl;
    }
}
