#include <fstream>
#include <iostream>
#include <unistd.h>
#include <signal.h>
using namespace std;

string cpu_name;
unsigned long long  lastUser, lastNice, lastSystem, lastIdle, totalRam;
const double divisor = 1048576;
double totalRamd, usedRamd, max_ram = 0, max_freq = 0, max_cpu = 0, avg_ram = 0, avg_cpu = 0, avg_freq = 0;
int counter = 0;

void signal_treatment(int s)
{
    if(s == SIGINT) {
        char op;
        cout << " q to exit, r to reset saved stats, any else key to resume: ";
        cin >> op;
        if(op == 'q') {
            system("clear");
            avg_cpu /= counter, avg_freq /= counter, avg_ram /= counter;
            cout << "Average cpu usage: " << avg_cpu << " %" << endl;
            cout << "Max cpu usage: " << max_cpu << " %" << endl;
            cout << "Average cpu freq: " << avg_freq << " GHz" << endl;
            cout << "Max cpu freq: " << max_freq << " GHz" << endl;
            cout << "Average ram usage: " << avg_ram << " GB" << endl;
            cout << "Max ram usage: " << max_ram << " GB" << endl;
            exit(0);
        }
        else if(op == 'r')
            max_ram = 0, max_freq = 0, avg_ram = 0, avg_ram = 0, avg_cpu = 0, max_cpu = 0;
        else
            return;
    }
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
    totalRamd = totalRam/divisor;
    
    file.open("/proc/stat");
    file.seekg(4);
    file >> lastUser >> lastNice >> lastSystem >> lastIdle;
    file.close();

    struct sigaction sa;
    sigfillset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = &signal_treatment;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGALRM, &sa, NULL);
}

double get_ram_usage()
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
    usedRam = totalRam - freeRam;
    cachedtotalRam = cachedRam + SRecalaimable - Shmem;
    realusedRam = usedRam - (bufferRam + cachedtotalRam);
    usedRamd = realusedRam/divisor;
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
    if(time < 1)
    {
        cout << "ERROR: Refresh rate must be an integer greater than 0" << endl;
        return 0;
    }
    init();
    usleep(100000);
    while(true)
    {
        alarm(time);
        double ram_usage = get_ram_usage(), cpu_usage = get_cpu_usage(), cpu_freq = get_cpu_freq();
        avg_ram += usedRamd, avg_cpu += cpu_usage, avg_freq += cpu_freq;
        max_cpu = max(max_cpu, cpu_usage), max_ram = max(max_ram, usedRamd), max_freq = max(cpu_freq, max_freq);
        ++counter;

        system("clear");
        cout << "CPU model: " << cpu_name << endl << endl;
        
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
        cout << "[" << usedRamd << "/" << totalRamd << "] GB" << endl << endl;
        
        cout << "Refreshing every " << time << "s" << endl;
        cout << "Ctrl+C to stop" << endl;
        
        sigset_t mask;
        sigfillset(&mask);
        sigdelset(&mask, SIGALRM);
        sigdelset(&mask, SIGINT);
        sigsuspend(&mask);
    }
}
