#ifndef MAIN_WINDOW_HH
#define MAIN_WINDOW_HH

#include "pcstats.hh"
#include "processes.hh"

#include <cstdlib>
#include <list>
#include <ncurses.h>
using namespace std;

class main_window {
private:
    //Private attributes
    
    int showedWin;

    int nWins;

    WINDOW* cpu_usage_win;
    WINDOW* ram_usage_win;
    WINDOW* core_usage_win;
    WINDOW* core_temps_win;
    WINDOW* core_freq_win;
    WINDOW* refresh_rate_win;

    WINDOW* procsWin;
    WINDOW* procSearchWin;
    
    unsigned int max_stdsrc_height, max_stdsrc_width;

    unsigned int core_wins_width;
    
    list<unsigned int> cpu_usage_history, ram_usage_history;
    
    double refresh_rate;
    
    pcstats *stats;

    processes *procs;

    int procOffset;

    bool foundProc;

    int foundProcOffset;

    string procName;

    
    //Private functions
    
    void clear_box(WINDOW* win);

    void maximum_win_sizes();
    
    void print_cpu_graphic();
    
    void print_ram_graphic();
    
    void print_core_usage();
    
    void print_core_temps();
    
    void print_core_freq();
    
    void print_all_win();

    void printProcWin();
    
    void refresh_all_win();
    
    void set_refresh_rate(string refresh_rate);
    
    void resizeStatsWin();

    void resizeProcsWin();
    
public:
    main_window(const double& refresh_rate);
    
    void show();
    
    ~main_window();
};

#endif
