#include <ncurses.h>
#include <unistd.h>
#include "pcstats.hh"

void usage() 
{
    cout << "pcstats [OPTIONS]" << endl;
    cout << "OPTIONS:" << endl;
    cout << "-n refres_rate     set refresh rate (default: 1s)" << endl;
}

void print_exit_screen() 
{
    int height, width;
    getmaxyx(stdscr,height,width);
    WINDOW* exit_screen = newwin(5,46,(height-5)/2,(width-46)/2);
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
            yes = not yes;
        }
        else if(c == KEY_RESIZE) {
            wclear(exit_screen);
            wrefresh(exit_screen);
            int height, width;
            getmaxyx(stdscr,height,width);
            exit_screen = newwin(5,46,(height-5)/2,(width-46)/2);
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

void print_usage_bar(double usage,int width) 
{ 
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

void print_stats(pcstats& stats, double time, int pos) 
{
    int height, width;
    getmaxyx(stdscr,height,width);
    
    int num_cores = stats.cpu_cores(), num_sensors = stats.cpu_sensors();
    
    clear();
    for(int i = pos; i < height+pos-1; ++i) {
        if(i == 0) printw("CPU name: %s\n",stats.cpu_name().c_str());
        else if(i == 2) printw("CPU temp:\n");
        else if(i >= 3 and i <= 2+num_sensors) {
            int j = i-3;
            printw("%s: %dºC\n",stats.get_core_temp(j).first.c_str(), stats.get_core_temp(j).second);
        }
        else if(i == 4+num_sensors) printw("CPU usage:\n");
        else if(i == 5+num_sensors) {
            printw("CPU ");
            print_usage_bar(stats.get_core_usage(-1), width);
        }
        else if(i >= 6+num_sensors and i <= 5+num_cores+num_sensors) {
            int j = i-(6+num_sensors);
            printw("CPU%d ",j);
            print_usage_bar(stats.get_core_usage(j), width);
        }
        else if(i == 7+num_cores+num_sensors) printw("CPU freq:\n");
        else if(i >= 8+num_cores+num_sensors and i <= 7+2*num_cores+num_sensors) {
            int j = i-(8+num_cores+num_sensors);
            printw("CPU%d: %f MHz\n",j,stats.get_core_freq(j));
        }
        else if(i == 9+2*num_cores+num_sensors) printw("RAM usage:\n");
        else if(i == 10+2*num_cores+num_sensors) print_usage_bar(stats.get_ram_usage(), width);
        else if(i == 11+2*num_cores+num_sensors) printw("[%f/%f] GB\n\n",stats.get_total_ram()*stats.get_ram_usage()/100,stats.get_total_ram());
        else printw("\n");
    }
    
    printw("Refreshing every %f seconds\n", time);
    
    refresh();
}


int main(int argc, char* argv[]) 
{    
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
    
    int act_line = 0;
    int min_height = 13 + 2*stats.cpu_cores() + stats.cpu_sensors();
    int c = -1;
    
    usleep(100000);
    
    stats.update_stats();
    print_stats(stats,time,act_line);
    
    while(true) {
        int height, width;
        getmaxyx(stdscr,height,width);
        
        timeout(time_mili);
        if((c = getch())) {
            if(c == 'q') {
                clear();
                refresh();
                print_exit_screen();
                endwin();
                return 0;
            }
            else if(c == KEY_RESIZE) {
                if(height >= min_height) act_line = 0;
                print_stats(stats,time,act_line);
            }
            else if(c == KEY_UP and height < min_height and act_line > 0) {
                --act_line;
                print_stats(stats,time,act_line);
            }
            else if(c == KEY_DOWN and height < min_height and act_line+height < min_height) {
                ++act_line;
                print_stats(stats,time,act_line);
            }
            else if(c >= '0' and c <= '9') {
                string num; num.push_back(c);
                
                timeout(-1);
                echo();
                move(height-1,0);
                clrtoeol();
                printw("Refreshing every %s seconds", num.c_str());
                refresh();
                
                bool point = false;
                while((c = getch())) {
                    if(c == '\n') {
                        double ret = atof(num.c_str());
                        if(ret >= 0.1) {
                            time_mili = ret*1000;
                            time = ret;
                        }
                        break;
                    }
                    else if((c >= '0' and c <= '9') or (c == '.' and not point)) {
                        if(c == '.') point = true;
                        num.push_back(c);
                        move(height-1,0);
                        clrtoeol();
                        printw("Refreshing every %s seconds", num.c_str());
                        refresh();
                    }
                    else break;
                }
                noecho();
                print_stats(stats,time,act_line);
            }
            if(c == -1) {
                stats.update_stats();
                print_stats(stats,time,act_line);
            }
        }
    }
}
