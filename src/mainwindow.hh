#ifndef MAIN_WINDOW_HH
#define MAIN_WINDOW_HH

#include "processinfo.hh"
#include "raminfo.hh"
#include "cpuinfo.hh"
#include <cstdlib>
#include <list>
#include <ncurses.h>
using namespace std;

class MainWindow {
private:
    //Private attributes
    
    WINDOW* cpu_usage_win;
    WINDOW* ram_usage_win;
    WINDOW* core_usage_win;
    WINDOW* core_temps_win;
    WINDOW* core_freq_win;
    WINDOW* refresh_rate_win;
    
    int max_stdsrc_height, max_stdsrc_width;

    int core_wins_width;
    
    list<int> cpu_usage_history, ram_usage_history;
    
    double refresh_rate;
    

    CpuInfo *cpuInfo;
    RamInfo *ramInfo;
    ProcessInfo *processInfo;
    
    //Private functions
    
    void clear_box(WINDOW* win);

    void maximum_win_sizes();
    
    void print_cpu_graphic();
    
    void print_ram_graphic();
    
    void print_core_usage();
    
    void print_core_temps();
    
    void print_core_freq();
    
    void print_all_win();
    
    void refresh_all_win();
    
    void set_refresh_rate(string refresh_rate);
    
    void resize();
    
public:
    MainWindow(const double& refresh_rate);
    
    void show();
    
    ~MainWindow();
};

#endif
