#include "main_window.hh"

main_window::main_window(const string& refresh_rate) {
    this->refresh_rate = refresh_rate;
    resize();
}

void main_window::clear_box(WINDOW* win) {
    int max_win_height, max_win_width;
    getmaxyx(win, max_win_height, max_win_width);
    for(int i = 1; i < max_win_height-1; ++i) {
        wmove(win, i, 1);
        wclrtoeol(win);
        wmove(win, i, max_win_width-1);
        waddch(win, ACS_VLINE);
    }
}

void main_window::refresh_all_win() {
    wrefresh(cpu_usage_win);
    wrefresh(ram_usage_win);
    wrefresh(core_usage_win);
    wrefresh(core_temps_win);
    wrefresh(core_freq_win);
    wrefresh(refresh_rate_win);
}


void main_window::print_cpu_graphic() {
    int max_win_height, max_win_width;
    getmaxyx(cpu_usage_win, max_win_height, max_win_width);
    double cpu_usage = stats.get_core_usage(-1);
    int height = cpu_usage/100*(max_win_height-2);
    
    cpu_usage_history.push_back(height);
    if(max_win_width-1 == cpu_usage_history.size()) {
        cpu_usage_history.pop_front();
        clear_box(cpu_usage_win);
    }
    int j = 1;
    for(int it : cpu_usage_history) {
        for(int i = 0; i < it; ++i) {
            wmove(cpu_usage_win, max_win_height-i-2, j);
            waddch(cpu_usage_win, '#');
        }
        ++j;
    }
    
    wmove(cpu_usage_win, 0, max_stdsrc_width/2-12);
    if(cpu_usage == 100) wprintw(cpu_usage_win, "[%.2f%]", stats.get_core_usage(-1));
    else if(cpu_usage >= 10) wprintw(cpu_usage_win, "[ %.2f%]", stats.get_core_usage(-1));
    else wprintw(cpu_usage_win, "[  %.2f%]", stats.get_core_usage(-1));
}

void main_window::print_ram_graphic() {
    int max_win_height, max_win_width;
    getmaxyx(ram_usage_win, max_win_height, max_win_width);
    double ram_usage = stats.get_ram_usage();
    int height = ram_usage/100*(max_win_height-2);
    
    ram_usage_history.push_back(height);
    if(max_win_width-1 == ram_usage_history.size()) {
        ram_usage_history.pop_front();
        clear_box(ram_usage_win);
    }
    int j = 1;
    for(int it : ram_usage_history) {
        for(int i = 0; i < it; ++i) {
            wmove(ram_usage_win, max_win_height-i-2, j);
            waddch(ram_usage_win, '#');
        }
        ++j;
    }
    wmove(ram_usage_win, 0, max_stdsrc_width/2-17), wprintw(ram_usage_win, "[%.2f/%.2f GB]", stats.get_ram_usage()/100*stats.get_total_ram(), stats.get_total_ram());
}

void main_window::resize() {
    cpu_usage_history.clear();
    ram_usage_history.clear();
        
    getmaxyx(stdscr, max_stdsrc_height, max_stdsrc_width);
    
    cpu_usage_win = newwin((max_stdsrc_height-1)/2, max_stdsrc_width/2, 0, 0);
    box(cpu_usage_win, 0, 0);
    wmove(cpu_usage_win, 0, 1), wprintw(cpu_usage_win, "CPU usage");
    wmove(cpu_usage_win, 0, max_stdsrc_width/2-12), wprintw(cpu_usage_win, "[%.2f%]", stats.get_core_usage(-1));
    
    ram_usage_win = newwin(max_stdsrc_height-1-(max_stdsrc_height-1)/2, max_stdsrc_width/2, (max_stdsrc_height-1)/2, 0);
    box(ram_usage_win, 0, 0);
    wmove(ram_usage_win, 0, 1), wprintw(ram_usage_win, "RAM usage");
    wmove(ram_usage_win, 0, max_stdsrc_width/2-17);
    wprintw(ram_usage_win, "[%.2f/%.2f GB]", stats.get_ram_usage()/100*stats.get_total_ram(), stats.get_total_ram());
    
    core_usage_win = newwin(max_stdsrc_height/3, max_stdsrc_width/2, 0, max_stdsrc_width/2);
    box(core_usage_win, 0, 0);
    wmove(core_usage_win, 0, 1), wprintw(core_usage_win, "Core usage");
    
    core_temps_win = newwin(2*max_stdsrc_height/3-max_stdsrc_height/3, max_stdsrc_width/2, max_stdsrc_height/3, max_stdsrc_width/2);
    box(core_temps_win, 0, 0);
    wmove(core_temps_win, 0, 1), wprintw(core_temps_win, "Core temps");
    
    core_freq_win = newwin(max_stdsrc_height-2*max_stdsrc_height/3, max_stdsrc_width/2, 2*max_stdsrc_height/3, max_stdsrc_width/2);
    box(core_freq_win, 0, 0);
    wmove(core_freq_win, 0, 1), wprintw(core_freq_win, "Core frequency");
    
    refresh_rate_win = newwin(1, max_stdsrc_width/2, max_stdsrc_height-1, 0);
    wprintw(refresh_rate_win, "Refreshing every %s seconds", refresh_rate.c_str());
}

void main_window::set_refresh_rate(string refresh_rate) {
    this->refresh_rate = refresh_rate;
    wclear(refresh_rate_win);
    wprintw(refresh_rate_win, "Refreshing every %s seconds", refresh_rate.c_str());
    wrefresh(refresh_rate_win);
}

void main_window::print_core_usage() {
    clear_box(core_usage_win);
    int n;
    int max_win_height, max_win_width;
    getmaxyx(core_usage_win, max_win_height, max_win_width);
    if(stats.cpu_cores() > max_win_height-2) n = max_win_height-2;
    else n = stats.cpu_cores();
    for(int i = 0; i < n; ++i) {
        wmove(core_usage_win, i+1, 1);
        wprintw(core_usage_win, "CPU%d: %.2f % %", i, stats.get_core_usage(i));
    }
}

void main_window::print_core_temps() {
    clear_box(core_temps_win);
    int n;
    int max_win_height = getmaxy(core_temps_win);
    if(stats.cpu_sensors() > max_win_height-2) n = max_win_height-2;
    else n = stats.cpu_sensors();
    for(int i = 0; i < n; ++i) {
        wmove(core_temps_win, i+1, 1);
        wprintw(core_temps_win, "%s: %d ºC", stats.get_core_temp(i).first.c_str(), stats.get_core_temp(i).second);
    }
}

void main_window::print_core_freq() {
    clear_box(core_freq_win);
    int n;
    int max_win_height = getmaxy(core_freq_win);
    if(stats.cpu_cores() > max_win_height-2) n = max_win_height-2;
    else n = stats.cpu_cores();
    for(int i = 0; i < n; ++i) {
        wmove(core_freq_win, i+1, 1);
        wprintw(core_freq_win, "CPU%d: %d MHz", i, int(stats.get_core_freq(i)));
    }
}

void main_window::print_all_win() {
    print_cpu_graphic();
    print_ram_graphic();
    print_core_usage();
    print_core_temps();
    print_core_freq();
}

void main_window::update_stats() {
    stats.update_stats();
}
