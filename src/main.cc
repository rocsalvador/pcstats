#include <ncurses.h>
#include <unistd.h>
#include "pcstats.hh"

void usage() {
    cout << "pcstats [OPTIONS]" << endl;
    cout << "OPTIONS:" << endl;
    cout << "-n refres_rate     set refresh rate (default: 1s)" << endl;
}

void print_exit_screen() {
    int height, width;
    getmaxyx(stdscr,height,width);
    WINDOW* exit_screen = newwin(5,46,(height-3)/2,(width-46)/2);
    keypad(exit_screen, true);
    box(exit_screen,0,0);
    wmove(exit_screen, 1, 1);
    wprintw(exit_screen, "Do you want to save average stats on a file?");
    wmove(exit_screen, 3, 12);
    wattron(exit_screen, A_STANDOUT);
    wprintw(exit_screen, "yes");
    wmove(exit_screen, 3, 29);
    wattroff(exit_screen, A_STANDOUT);
    wprintw(exit_screen, "no");
    wrefresh(exit_screen);
    int c;
    bool yes = true;
    while((c = wgetch(exit_screen))) {
        if(c == '\n') return;
        else if(c == KEY_RIGHT or c == KEY_LEFT) {
            wmove(exit_screen, 3, 12);
            if(not yes) wattron(exit_screen, A_STANDOUT);
            else wattroff(exit_screen, A_STANDOUT);
            wprintw(exit_screen, "yes");
            
            wmove(exit_screen, 3, 29);
            if(yes) wattron(exit_screen, A_STANDOUT);
            else wattroff(exit_screen, A_STANDOUT);
            wprintw(exit_screen, "no");
            
            wrefresh(exit_screen);
            yes = true;
        }
        else if(c == 410) {
            wclear(exit_screen);
            wrefresh(exit_screen);
            int height, width;
            getmaxyx(stdscr,height,width);
            exit_screen = newwin(5,46,(height-3)/2,(width-46)/2);
            keypad(exit_screen, true);
            box(exit_screen,0,0);
            
            wmove(exit_screen, 1, 1);
            wprintw(exit_screen, "Do you want to save average stats on a file?");
            wmove(exit_screen, 3, 12);
            
            if(yes) wattron(exit_screen, A_STANDOUT);
            else wattroff(exit_screen, A_STANDOUT);
            wprintw(exit_screen, "yes"); wmove(exit_screen, 3, 29);
            
            if(not yes) wattron(exit_screen, A_STANDOUT);
            else wattroff(exit_screen, A_STANDOUT);
            wprintw(exit_screen, "no");
            
            wrefresh(exit_screen);
        }
    }
}

void print_usage_bar(double usage, int height, int width) {
    printw("[");
    if(width >= 119) {
        for(int i = 0; i < int(usage); ++i) printw("=");
        for(int i = 0; i < 100-int(usage); ++i) printw(" ");
    }
    else if(width > 19) {
        int bar_width = width - 19;
        for(int i = 0; i < int(bar_width*usage/100); ++i) printw("=");
        for(int i = 0; i < bar_width-int(bar_width*usage/100); ++i) printw(" ");
    }
    
    printw("] ");
    printw("%f",usage);
    printw("%\n");
}

void print_stats(pcstats& stats, double time) {
    int height, width;
    getmaxyx(stdscr,height,width);
    clear();
    
    int min_height = 16 + 2*stats.cpu_cores() + stats.cpu_sensors();
    
    if(height < min_height or width < 22) return;
    printw("CPU name: %s\n\n",stats.cpu_name().c_str());
    
    printw("CPU temp:\n");
    for(int i = 0; i < stats.cpu_sensors(); ++i) {
        printw("%s: %dºC\n",stats.get_core_temp(i).first.c_str(), stats.get_core_temp(i).second);
    }
    printw("\n");
    
    printw("CPU usage:\nCPU  ");
    print_usage_bar(stats.get_core_usage(-1), height, width);
    for(int i = 0; i < stats.cpu_cores(); ++i) {
        printw("CPU%d ",i);
        print_usage_bar(stats.get_core_usage(i), height, width);
    }
    printw("\n");
    
    printw("CPU freq:\n");
    printw("CPU: %f MHz\n",stats.get_core_freq(-1));
    for(int i = 0; i < stats.cpu_cores(); ++i) {
        printw("CPU%d: %f MHz\n",i,stats.get_core_freq(i));
    }
    printw("\n");
    
    printw("RAM usage:\n");
    print_usage_bar(stats.get_ram_usage(), height, width);
    printw("[%f/%f] GB\n\n",stats.get_total_ram()*stats.get_ram_usage()/100,stats.get_total_ram());
    
    printw("Refresing every %f seconds\n", time);
    
    refresh();
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
        else if(argc >= 2) {
            usage();
            return 0;
        }
    }
    
    pcstats stats;
    
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, true);
    curs_set(0);
    clear();
    
    usleep(100000);
    
    int c = -1;
    while(true) {
        if(c == -1) {
            stats.update_cpu_freq();
            stats.update_cpu_temp();
            stats.update_cpu_usage();
            stats.update_ram_usage();
        }
        print_stats(stats,time);
        timeout(time_mili);
        if((c = getch())) {
            if(c == 'q') {
                clear();
                refresh();
                print_exit_screen();
                endwin();
                return 0;
            }
            else if(c == KEY_UP) {
                time_mili += 50;
                time += 0.05;
            }
            else if(c == KEY_DOWN and time > 0.1) {
                time_mili -= 50;
                time -= 0.05;
            }
            else if(c >= '0' and c <= '9') {
                timeout(-1);
                printw("%c",c);
                refresh();
                echo();
                string num; num.push_back(c);
                char c;
                while((c = getch())) {
                    if(c == '\n') {
                        int ret = atof(num.c_str());
                        if(ret >= 1) {
                            time_mili = ret*1000;
                            time = ret;
                        }
                        break;
                    }
                    else if(c >= '0' and c <= '9') num.push_back(c);
                    else break;
                }
                noecho();
            }
        }
    }
}
