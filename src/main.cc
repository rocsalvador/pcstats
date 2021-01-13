#include <unistd.h>
#include <signal.h>
#include "pcstats.hh"


void signal_treatment(int s) {
    if(s == SIGINT) {
        char c;
        cout << " q to quit, any else key to exit: ";
        cin >> c;
        if(c == 'q') exit(0);
    }
}

void init_signals() {
    struct sigaction sa;
    sigfillset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = &signal_treatment;
    sigaction(SIGINT, &sa, NULL);
}

void print_stats(pcstats& stats, double time) {
    stats.update_cpu_freq(), stats.update_cpu_usage(), stats.update_ram_usage(), stats.update_cpu_temp();
    
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


int main() {
    double time;
    system("clear");
    cout << "Refresh rate (s): ";
    cin >> time;
    if(time <= 0) {
        cout << "ERROR: Refresh rate must be a doble greater than 0" << endl;
        return 0;
    }
    pcstats stats;
    init_signals();
    usleep(100000);
    while(true) {
        print_stats(stats,time);
        usleep(time*1000000);
    }
}
