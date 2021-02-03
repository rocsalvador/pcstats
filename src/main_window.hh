#include "pcstats.hh"
#include <cstdlib>
#include <list>
#include <ncurses.h>
using namespace std;

class main_window {
private:
    WINDOW* cpu_usage_win;
    WINDOW* ram_usage_win;
    WINDOW* core_usage_win;
    WINDOW* core_temps_win;
    WINDOW* core_freq_win;
    WINDOW* refresh_rate_win;
    
    int max_stdsrc_height, max_stdsrc_width;
    
    list<int> cpu_usage_history, ram_usage_history;
    
    string refresh_rate;
    
    pcstats stats;
    
    void clear_box(WINDOW* win);
    
public:
    main_window(const string& refresh_rate);
    
    void print_cpu_graphic();
    
    void print_ram_graphic();
    
    void print_core_usage();
    
    void print_core_temps();
    
    void print_core_freq();
    
    void print_all_win();
    
    void refresh_all_win();
    
    void set_refresh_rate(string refresh_rate);
    
    void resize();
    
    void update_stats();
};
