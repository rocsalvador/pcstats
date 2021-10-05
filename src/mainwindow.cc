#include "mainwindow.hh"
#include "pcstats.hh"
#include "processes.hh"

#include <signal.h>

main_window::main_window(const double& refresh_rate) {
    stats = new pcstats;

    procs = new processes;
    
    this->refresh_rate = refresh_rate;

    showedWin = 0;

    nWins = 2;

    procOffset = 0;

    foundProc = false;

    foundProcOffset = -1;

    procName = "";

    // Init curses screen
    initscr();
    noecho();
    keypad(stdscr, true);
    curs_set(0);
    maximum_win_sizes();
    resizeStatsWin();
}

void main_window::maximum_win_sizes() {
    core_wins_width = 17;   // |CPUxx: ---- MHz|
    for(int i = 0; i < stats->cpu_sensors(); ++i) {
        if(core_wins_width < stats->get_core_temp(i).first.size() + 10) 
            core_wins_width = stats->get_core_temp(i).first.size() + 10;         
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
    unsigned int max_win_height, max_win_width;
    getmaxyx(cpu_usage_win, max_win_height, max_win_width);
    double cpu_usage = stats->get_core_usage(-1);
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
            waddch(cpu_usage_win, ACS_CKBOARD);
        }
        ++j;
    }
    
    wmove(cpu_usage_win, 0, max_stdsrc_width-core_wins_width-12);
    if(cpu_usage == 100) wprintw(cpu_usage_win, "[%.2f%]", stats->get_core_usage(-1));
    else if(cpu_usage >= 10) wprintw(cpu_usage_win, "[ %.2f%]", stats->get_core_usage(-1));
    else wprintw(cpu_usage_win, "[  %.2f%]", stats->get_core_usage(-1));
}

void main_window::print_ram_graphic() {
    unsigned int max_win_height, max_win_width;
    getmaxyx(ram_usage_win, max_win_height, max_win_width);
    double ram_usage = stats->get_ram_usage();
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
            waddch(ram_usage_win, ACS_CKBOARD);
        }
        ++j;
    }
    wmove(ram_usage_win, 0, max_stdsrc_width-core_wins_width-17);
    wprintw(ram_usage_win, "[%.2f/%.2f GB]", stats->get_ram_usage()/100*stats->get_total_ram(), stats->get_total_ram());
}

void main_window::resizeStatsWin() {
    cpu_usage_history.clear();
    ram_usage_history.clear();
        
    getmaxyx(stdscr, max_stdsrc_height, max_stdsrc_width);

    int cpu_usage_win_height = (max_stdsrc_height - 1)/2;
    int ram_usage_win_height = max_stdsrc_height - 1 - cpu_usage_win_height;
    int core_usage_win_height = max_stdsrc_height/3;
    int core_temps_win_height = core_usage_win_height;
    int core_freq_win_height = max_stdsrc_height - 2*core_usage_win_height;
    int core_freq_win_pos = 2*core_usage_win_height;
    int graphics_win_width = max_stdsrc_width-core_wins_width;
    
    cpu_usage_win = newwin(cpu_usage_win_height, graphics_win_width, 0, 0);
    box(cpu_usage_win, 0, 0);
    wmove(cpu_usage_win, 0, 1), wprintw(cpu_usage_win, "CPU usage");
    wmove(cpu_usage_win, 0, graphics_win_width-12), wprintw(cpu_usage_win, "[%.2f%]", stats->get_core_usage(-1));
    
    ram_usage_win = newwin(ram_usage_win_height, graphics_win_width, cpu_usage_win_height, 0);
    box(ram_usage_win, 0, 0);
    wmove(ram_usage_win, 0, 1), wprintw(ram_usage_win, "RAM usage");
    wmove(ram_usage_win, 0, graphics_win_width-17);
    wprintw(ram_usage_win, "[%.2f/%.2f GB]", stats->get_ram_usage()/100*stats->get_total_ram(), stats->get_total_ram());
    
    core_usage_win = newwin(core_usage_win_height, core_wins_width, 0, graphics_win_width);
    box(core_usage_win, 0, 0);
    wmove(core_usage_win, 0, 1), wprintw(core_usage_win, "Core usage");
    
    core_temps_win = newwin(core_temps_win_height, core_wins_width, core_usage_win_height, graphics_win_width);
    box(core_temps_win, 0, 0);
    wmove(core_temps_win, 0, 1), wprintw(core_temps_win, "Core temps");
    
    core_freq_win = newwin(core_freq_win_height, core_wins_width, core_freq_win_pos, graphics_win_width);
    box(core_freq_win, 0, 0);
    wmove(core_freq_win, 0, 1), wprintw(core_freq_win, "Core frequency");
    
    refresh_rate_win = newwin(1, max_stdsrc_width-core_wins_width, max_stdsrc_height-1, 0);
    wprintw(refresh_rate_win, "Refreshing every %.2f seconds", refresh_rate);
}

void main_window::resizeProcsWin()
{
    getmaxyx(stdscr, max_stdsrc_height, max_stdsrc_width);
    procsWin = newwin(max_stdsrc_height-1, max_stdsrc_width, 0, 0);
    procSearchWin = newwin(1, max_stdsrc_width-max_stdsrc_width/2, max_stdsrc_height-1, max_stdsrc_width/2);
    refresh_rate_win = newwin(1, max_stdsrc_width/2, max_stdsrc_height-1, 0);
    box(procsWin, 0, 0);
    int k = (max_stdsrc_width-9)/3;
    wmove(procsWin, 0, k), wprintw(procsWin, "PID");
    wmove(procsWin, 0, k*2), wprintw(procsWin, "STATE");
    wmove(procsWin, 0, k*3), wprintw(procsWin, "THREADS");

    wprintw(refresh_rate_win, "Refreshing every %.2f seconds", refresh_rate);
    wrefresh(refresh_rate_win);

    int maxSearchWinWidth = getmaxx(procSearchWin);
    wclear(procSearchWin);
    wmove(procSearchWin, 0, maxSearchWinWidth-15);
    wattron(procSearchWin, A_BOLD);
    wprintw(procSearchWin, "Type to search");
    wattroff(procSearchWin, A_BOLD);
    wrefresh(procSearchWin);
}

void main_window::printProcWin()
{
    clear_box(procsWin);
    getmaxyx(stdscr, max_stdsrc_height, max_stdsrc_width);
//     wmove(procsWin, 1, max_stdsrc_width-4);
//     wprintw(procsWin, "%d", procs->getNProcs());
    int k = (max_stdsrc_width-9)/3;
    for(int i = 0; i < int(max_stdsrc_height-3) and i+procOffset < procs->getNProcs(); ++i) {
        wmove(procsWin, i+1, 1);
        if(foundProc and procOffset+i == foundProcOffset) wattron(procsWin, A_STANDOUT);
        wprintw(procsWin, "%s", procs->getProcName(i+procOffset).c_str());
        wmove(procsWin, i+1, k);
        wprintw(procsWin, "%d", procs->getProcPid(i+procOffset));
        wmove(procsWin, i+1, k*2);
        wprintw(procsWin, "%s", procs->getProcState(i+procOffset).c_str());
        wmove(procsWin, i+1, k*3);
        wprintw(procsWin, "%d", procs->getProcThreads(i+procOffset));
        wattroff(procsWin, A_STANDOUT);
    }
}

void main_window::set_refresh_rate(string refresh_rate) {
    wclear(refresh_rate_win);
    wprintw(refresh_rate_win, "Refreshing every %s seconds", refresh_rate.c_str());
    wrefresh(refresh_rate_win);
}

void main_window::print_core_usage() {
    clear_box(core_usage_win);
    wrefresh(core_usage_win);
    int n;
    int max_win_height, max_win_width;
    getmaxyx(core_usage_win, max_win_height, max_win_width);
    if(stats->cpu_cores() > max_win_height-2) n = max_win_height-2;
    else n = stats->cpu_cores();
    for(int i = 0; i < n; ++i) {
        wmove(core_usage_win, i+1, 1);
        wprintw(core_usage_win, "CPU%d: %.2f % %", i, stats->get_core_usage(i));
    }
}

void main_window::print_core_temps() {
    clear_box(core_temps_win);
    wrefresh(core_temps_win);
    int n;
    int max_win_height = getmaxy(core_temps_win);
    if(stats->cpu_sensors() > max_win_height-2) n = max_win_height-2;
    else n = stats->cpu_sensors();
    for(int i = 0; i < n; ++i) {
        wmove(core_temps_win, i+1, 1);
        wprintw(core_temps_win, "%s: %d ºC", stats->get_core_temp(i).first.c_str(), stats->get_core_temp(i).second);
    }
}

void main_window::print_core_freq() {
    clear_box(core_freq_win);
    int n;
    int max_win_height = getmaxy(core_freq_win);
    if(stats->cpu_cores() > max_win_height-2) n = max_win_height-2;
    else n = stats->cpu_cores();
    for(int i = 0; i < n; ++i) {
        wmove(core_freq_win, i+1, 1);
        wprintw(core_freq_win, "CPU%d: %d MHz", i, int(stats->get_core_freq(i)));
    }
}

void main_window::print_all_win() {
    print_cpu_graphic();
    print_ram_graphic();
    print_core_usage();
    print_core_temps();
    print_core_freq();
}

void main_window::show() {
    int c;
    timeout(100);
    while((c = getch())) {
        if(c == -1) {
            switch(showedWin) {
                case 0:
                    stats->update_stats();
                    print_all_win();
                    refresh_all_win();
                    break;
                case 1:
                    procs->update();
                    foundProc = procs->getProcIndex(procName) != -1;
                    if(foundProc) foundProcOffset = procs->getProcIndex(procName);

                    printProcWin();
                    wrefresh(procsWin);
                    break;
            }
        }
        else if(c == KEY_RESIZE) {
            if(showedWin == 0) resizeStatsWin();
            else if(showedWin == 1) resizeProcsWin();
        }
        else if(c == KEY_RIGHT) {
            if(showedWin < nWins-1) ++showedWin;
            if(showedWin == 1) {
                procs->update();
                foundProc = procs->getProcIndex(procName) != -1;
                if(foundProc) foundProcOffset = procs->getProcIndex(procName);

                resizeProcsWin();
                printProcWin();
                wrefresh(procsWin);
            }
        }
        else if(c == KEY_LEFT) {
            if(showedWin > 0) {
                --showedWin;
                if(showedWin == 0) {
                    resizeStatsWin();
                    print_all_win();
                    refresh_all_win();
                }
            }
        }
        else if(c >= '0' and c <= '9') {
            int decimal_size = 0;
            string num;
            num.push_back(c);
            set_refresh_rate(num);
            bool point = false;
            timeout(3000);
            while(decimal_size < 4 and (c = getch())) {
                if(c == '.') {
                    if(point) break;
                    else {
                        num.push_back(c);
                        set_refresh_rate(num);
                        point = true;
                    }
                }
                else if(c >= '0' and c <= '9') {
                    num.push_back(c);
                    set_refresh_rate(num);
                }
                else if(c == '\n') {
                    if(stod(num) >= 0.1) refresh_rate = stod(num);
                    break;
                }
                else break;

                if(point) ++decimal_size;
            }
            wclear(refresh_rate_win);
            wprintw(refresh_rate_win, "Refreshing every %.2f seconds", refresh_rate);
            wrefresh(refresh_rate_win);
        }
        else if(c == 'q') break;

        if(showedWin == 1) {
            if(c == KEY_UP) {
                if(procOffset > 0) {
                    --procOffset;
                    printProcWin();
                    wrefresh(procsWin);
                }
            }
            else if(c == KEY_DOWN) {
                if(procOffset < procs->getNProcs()-1) {
                    ++procOffset;
                    printProcWin();
                    wrefresh(procsWin);
                }
            }
            else if(c >= 33 and c <= 126) {
                int maxSearchWinWidth = getmaxx(procSearchWin);
                procName = "";
                procName.push_back(c);
                wclear(procSearchWin);
                wmove(procSearchWin, 0, maxSearchWinWidth-1-procName.size());
                wprintw(procSearchWin, "%s", procName.c_str());
                wrefresh(procSearchWin);
                timeout(3000);
                while((c = getch()))
                {
                    if(c == KEY_ENTER) break;
                    else if((c >= 33 and c <= 126) or c == KEY_BACKSPACE) {
                        if(c == KEY_BACKSPACE) {
                            if(procName.size() > 0) {
                                procName.pop_back();
                                wclear(procSearchWin);
                            }
                        }
                        else procName.push_back(c);
                        wmove(procSearchWin, 0, maxSearchWinWidth-1-procName.size());
                        wprintw(procSearchWin, "%s", procName.c_str());
                        wrefresh(procSearchWin);
                    }
                    else break;
                }

                foundProc = procs->getProcIndex(procName) != -1;
                if(foundProc) {
                    procOffset = foundProcOffset = procs->getProcIndex(procName);
                    printProcWin();
                    wrefresh(procsWin);
                }
                else {
                    wmove(procSearchWin, 0, maxSearchWinWidth-18-procName.size());
                    wprintw(procSearchWin, "No process named %s", procName.c_str());
                    wrefresh(procSearchWin);
                }
            }
            else if(c == (0x1f & 'k') and foundProc) {
                int maxSearchWinWidth = getmaxx(procSearchWin);
                int ret = kill(procs->getProcPid(procs->getProcIndex(procName)), SIGKILL);
                if(ret == 0) {
                    wmove(procSearchWin, 0, maxSearchWinWidth-20-procName.size());
                    wprintw(procSearchWin, "Terminated process %s", procName.c_str());
                }
                else {
                    wmove(procSearchWin, 0, maxSearchWinWidth-16-procName.size());
                    wprintw(procSearchWin, "Unable to kill %s", procName.c_str());
                }
                wrefresh(procSearchWin);
            }
        }
        timeout(refresh_rate*1000);
    }
}

main_window::~main_window() {
    endwin();
    delete procs;
    delete stats;
}
