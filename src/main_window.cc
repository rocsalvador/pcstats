#include "main_window.hh"

main_window::main_window(string refresh_rate) {
    this->refresh_rate = refresh_rate;
    resize();
}

void main_window::print_graphic(WINDOW* win, double data, int x) {
    int max_win_height, max_win_width;
    getmaxyx(win, max_win_height, max_win_width);
    int height = data/100*(max_win_height-2);
    for(int i = 0; i < height; ++i) {
        wmove(win, max_win_height-i-2, x);
        waddch(win, '#');
    }
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
    print_graphic(cpu_usage_win, stats.get_core_usage(-1), x_cpu);
    int max_win_height, max_win_width;
    getmaxyx(cpu_usage_win, max_win_height, max_win_width);
    if(x_cpu == max_win_width-1) {
        clear_box(cpu_usage_win);
        x_cpu = 1;
    }
    else ++x_cpu;
    double cpu_usage = stats.get_core_usage(-1);
    wmove(cpu_usage_win, 0, max_stdsrc_width/2-16);
    if(cpu_usage == 100) wprintw(cpu_usage_win, "[%f%]", stats.get_core_usage(-1));
    else if(cpu_usage >= 10) wprintw(cpu_usage_win, "[ %f%]", stats.get_core_usage(-1));
    else wprintw(cpu_usage_win, "[  %f%]", stats.get_core_usage(-1));
}

void main_window::print_ram_graphic() {
    print_graphic(ram_usage_win, stats.get_ram_usage(), x_ram);
    int max_win_height, max_win_width;
    getmaxyx(ram_usage_win, max_win_height, max_win_width);
    if(x_ram == max_win_width-1) {
        clear_box(ram_usage_win);
        x_ram = 1;
    }
    else ++x_ram;
    wmove(ram_usage_win, 0, max_stdsrc_width/2-25), wprintw(ram_usage_win, "[%f/%f GB]", stats.get_ram_usage()/100*stats.get_total_ram(), stats.get_total_ram());
}

void main_window::resize() {
    x_cpu = 1, x_ram = 1;
    
    getmaxyx(stdscr, max_stdsrc_height, max_stdsrc_width);
    
    cpu_usage_win = newwin((max_stdsrc_height-1)/2, max_stdsrc_width/2, 0, 0);
    box(cpu_usage_win, 0, 0);
    wmove(cpu_usage_win, 0, 1), wprintw(cpu_usage_win, "CPU usage");
    wmove(cpu_usage_win, 0, max_stdsrc_width/2-16), wprintw(cpu_usage_win, "[%f%]", stats.get_core_usage(-1));
    
    ram_usage_win = newwin(max_stdsrc_height-1-(max_stdsrc_height-1)/2, max_stdsrc_width/2, (max_stdsrc_height-1)/2, 0);
    box(ram_usage_win, 0, 0);
    wmove(ram_usage_win, 0, 1), wprintw(ram_usage_win, "RAM usage");
    wmove(ram_usage_win, 0, max_stdsrc_width/2-25);
    wprintw(ram_usage_win, "[%f/%f GB]", stats.get_ram_usage()/100*stats.get_total_ram(), stats.get_total_ram());
    
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
        wprintw(core_usage_win, "CPU%d: %f%", i, stats.get_core_usage(i));
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
        wprintw(core_freq_win, "CPU%d: %f MHz", i, stats.get_core_freq(i));
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
