#include <csignal>
#include <string>
#include <chrono>
#include <thread>
#include "pcstats.hh"

void signal_treatment(int s) {
    if(s == SIGINT) {
        string s;
        cout << " q to quit, any else key to resume: ";
        getline(cin,s);
        if(s == "q") exit(0);
    }
}

void usage() {
    cout << "pcstats [OPTIONS]" << endl;
    cout << "OPTIONS:" << endl;
    cout << "-n refres_rate     set refresh rate (default: 1s)" << endl;
    exit(0);
}

void print_stats(pcstats& stats, double time) {
    stats.update_cpu_freq();
    stats.update_cpu_temp();
    stats.update_cpu_usage();
    stats.update_ram_usage();
    
    system("clear");   
    cout << "CPU model: " << stats.cpu_name() << endl << endl;
    
    cout << "CPU temp:" << endl;
    for(int i = 0; i < stats.cpu_sensors(); ++i) cout << stats.get_core_temp(i).first <<  ": " << stats.get_core_temp(i).second << "ºC" << endl;
    cout << endl;
    
    cout << "CPU usage:" << endl << "CPU  [";
    for(int i = 0; i < int(stats.get_core_usage(-1)); ++i) cout << "#";
    for(int i = 0; i < 100-int(stats.get_core_usage(-1)); ++i) cout << " ";
    cout << "] " << stats.get_core_usage(-1) << "%" << endl;
    
    for(int i = 0; i < stats.cpu_cores(); ++i) {
        cout << "CPU" << i << " [";
        for(int j = 0; j < int(stats.get_core_usage(i)); ++j) cout << "#";
        for(int j = 0; j < 100-int(stats.get_core_usage(i)); ++j) cout << " ";
        cout << "] " << stats.get_core_usage(i) << "%" << endl;
    }
    cout << endl;
    
    cout << "CPU frequency: " << stats.get_core_freq(-1) << " MHz" << endl;
    for(int i = 0; i < stats.cpu_cores(); ++i) cout << "CPU" << i << " frequency: " << stats.get_core_freq(i) << " MHz" << endl;
    cout << endl;
    
    cout << "RAM usage:" << endl << "[";
    for(int i = 0; i < int(stats.get_ram_usage()); ++i) cout << "#";
    for(int i = 0; i < 100-int(stats.get_ram_usage()); ++i) cout << " ";
    cout << "] " << stats.get_ram_usage() << "%" << endl;
    cout << "[" << stats.get_ram_usage()/100*stats.get_total_ram() << "/" << stats.get_total_ram() << "] GB" << endl << endl;
    cout << "Refreshing every " << time << "s" << endl;
    cout << "Ctrl+C to stop" << endl;
}


int main(int argc, char* argv[]) {
    double time = 1; 
    int time_mili = 1000;
    vector<string> arguments = {"-n"};
    if(argc > 1) {
        if(argc == 3 and argv[1] == arguments[0]) {
            time = stod(argv[2]);
            time_mili *= time;
        }
        else if(argc >= 2) usage();
    }
    
    pcstats stats;
    this_thread::sleep_for(chrono::milliseconds(100));
    signal(SIGINT, signal_treatment);
    while(true) {
        print_stats(stats,time);
        this_thread::sleep_for(chrono::milliseconds(time_mili));
    }
}
