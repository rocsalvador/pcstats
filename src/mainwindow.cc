#include "mainwindow.hh"
#include <curses.h>

MainWindow::MainWindow(const double& refreshRate) {
    cpuInfo = new CpuInfo();
    ramInfo = new RamInfo();
    processInfo = new ProcessInfo();
    
    this->refreshRate = refreshRate;

    // Init curses screen
    initscr();
    noecho();
    curs_set(0);
    maximumWinSizes();
    resize();
}

void MainWindow::maximumWinSizes() {
    coreWinsWidth = 17;   // |CPUxx: ---- MHz|
    for(int i = 0; i < cpuInfo->cpuSensors(); ++i) {
        if(coreWinsWidth < cpuInfo->getCoreTemp(i).first.size() + 10)
            coreWinsWidth = cpuInfo->getCoreTemp(i).first.size() + 10;
    }
}

void MainWindow::clearBox(WINDOW* win) {
    int maxWinHeight, maxWinWidth;
    getmaxyx(win, maxWinHeight, maxWinWidth);
    for(int i = 1; i < maxWinHeight-1; ++i) {
        wmove(win, i, 1);
        wclrtoeol(win);
        wmove(win, i, maxWinWidth-1);
        waddch(win, ACS_VLINE);
    }
}

void MainWindow::refreshAllWin() {
    wrefresh(cpuUsageWin);
    wrefresh(ramUsageWin);
    wrefresh(coreUsageWin);
    wrefresh(coreTempsWin);
    wrefresh(coreFreqWin);
    wrefresh(refreshRateWin);
}


void MainWindow::printCpuGraphic() {
    unsigned int maxWinHeight, maxWinWidth;
    getmaxyx(cpuUsageWin, maxWinHeight, maxWinWidth);
    double cpu_usage = cpuInfo->getCoreUsage(-1);
    int height = cpu_usage/100*(maxWinHeight-2);
    
    cpuUsageHistory.push_back(height);
    if(maxWinWidth-1 == cpuUsageHistory.size()) {
        cpuUsageHistory.pop_front();
        clearBox(cpuUsageWin);
    }
    int j = 1;
    for(int it : cpuUsageHistory) {
        for(int i = 0; i < it; ++i) {
            wmove(cpuUsageWin, maxWinHeight-i-2, j);
            waddch(cpuUsageWin, ACS_CKBOARD);
        }
        ++j;
    }
    
    wmove(cpuUsageWin, 0, maxStdsrcWidth-coreWinsWidth-12);
    if(cpu_usage == 100) wprintw(cpuUsageWin, "[%.2f]", cpuInfo->getCoreUsage(-1));
    else if(cpu_usage >= 10) wprintw(cpuUsageWin, "[ %.2f]", cpuInfo->getCoreUsage(-1));
    else wprintw(cpuUsageWin, "[  %.2f]", cpuInfo->getCoreUsage(-1));
}

void MainWindow::printRamGraphic() {
    unsigned int maxWinHeight, maxWinWidth;
    getmaxyx(ramUsageWin, maxWinHeight, maxWinWidth);
    double ram_usage = ramInfo->getRamUsage();
    int height = ram_usage/100*(maxWinHeight-2);
    
    ramUsageHistory.push_back(height);
    if(maxWinWidth-1 == ramUsageHistory.size()) {
        ramUsageHistory.pop_front();
        clearBox(ramUsageWin);
    }
    int j = 1;
    for(int it : ramUsageHistory) {
        for(int i = 0; i < it; ++i) {
            wmove(ramUsageWin, maxWinHeight-i-2, j);
            waddch(ramUsageWin, ACS_CKBOARD);
        }
        ++j;
    }
    wmove(ramUsageWin, 0, maxStdsrcWidth-coreWinsWidth-17);
    wprintw(ramUsageWin, "[%.2f/%.2f GB]", ramInfo->getRamUsage()/100*ramInfo->getTotalRam(), ramInfo->getTotalRam());
}

void MainWindow::resize() {
    cpuUsageHistory.clear();
    ramUsageHistory.clear();
        
    getmaxyx(stdscr, maxStdsrcHeight, maxStdsrcWidth);

    int cpuUsageWin_height = (maxStdsrcHeight - 1)/2;
    int ramUsageWin_height = maxStdsrcHeight - 1 - cpuUsageWin_height;
    int coreUsageWin_height = maxStdsrcHeight/3;
    int coreTempsWin_height = coreUsageWin_height;
    int coreFreqWin_height = maxStdsrcHeight - 2*coreUsageWin_height;
    int coreFreqWin_pos = 2*coreUsageWin_height;
    int graphics_win_width = maxStdsrcWidth-coreWinsWidth;
    
    cpuUsageWin = newwin(cpuUsageWin_height, graphics_win_width, 0, 0);
    box(cpuUsageWin, 0, 0);
    wmove(cpuUsageWin, 0, 1), wprintw(cpuUsageWin, "CPU usage");
    wmove(cpuUsageWin, 0, graphics_win_width-12), wprintw(cpuUsageWin, "[%.2f]", cpuInfo->getCoreUsage(-1));
    
    ramUsageWin = newwin(ramUsageWin_height, graphics_win_width, cpuUsageWin_height, 0);
    box(ramUsageWin, 0, 0);
    wmove(ramUsageWin, 0, 1), wprintw(ramUsageWin, "RAM usage");
    wmove(ramUsageWin, 0, graphics_win_width-17);
    wprintw(ramUsageWin, "[%.2f/%.2f GB]", ramInfo->getRamUsage()/100*ramInfo->getTotalRam(), ramInfo->getTotalRam());
    
    coreUsageWin = newwin(coreUsageWin_height, coreWinsWidth, 0, graphics_win_width);
    box(coreUsageWin, 0, 0);
    wmove(coreUsageWin, 0, 1), wprintw(coreUsageWin, "Core usage");
    
    coreTempsWin = newwin(coreTempsWin_height, coreWinsWidth, coreUsageWin_height, graphics_win_width);
    box(coreTempsWin, 0, 0);
    wmove(coreTempsWin, 0, 1), wprintw(coreTempsWin, "Core temps");
    
    coreFreqWin = newwin(coreFreqWin_height, coreWinsWidth, coreFreqWin_pos, graphics_win_width);
    box(coreFreqWin, 0, 0);
    wmove(coreFreqWin, 0, 1), wprintw(coreFreqWin, "Core frequency");
    
    refreshRateWin = newwin(1, maxStdsrcWidth-coreWinsWidth, maxStdsrcHeight-1, 0);
    wprintw(refreshRateWin, "Refreshing every %.2f seconds", refreshRate);
}

void MainWindow::setRefreshRate(string refresh_rate) {
    wclear(refreshRateWin);
    wprintw(refreshRateWin, "Refreshing every %s seconds", refresh_rate.c_str());
    wrefresh(refreshRateWin);
}

void MainWindow::printCoreUsage() {
    clearBox(coreUsageWin);
    wrefresh(coreUsageWin);
    int n;
    int maxWinHeight, maxWinWidth;
    getmaxyx(coreUsageWin, maxWinHeight, maxWinWidth);
    if(cpuInfo->cpuCores() > maxWinHeight-2) n = maxWinHeight-2;
    else n = cpuInfo->cpuCores();
    for(int i = 0; i < n; ++i) {
        wmove(coreUsageWin, i+1, 1);
        wprintw(coreUsageWin, "CPU%d: %.2f % %", i, cpuInfo->getCoreUsage(i));
    }
}

void MainWindow::printCoreTemps() {
    clearBox(coreTempsWin);
    wrefresh(coreTempsWin);
    int n;
    int maxWinHeight = getmaxy(coreTempsWin);
    if(cpuInfo->cpuSensors() > maxWinHeight-2) n = maxWinHeight-2;
    else n = cpuInfo->cpuSensors();
    for(int i = 0; i < n; ++i) {
        wmove(coreTempsWin, i+1, 1);
        wprintw(coreTempsWin, "%s: %d ºC", cpuInfo->getCoreTemp(i).first.c_str(), cpuInfo->getCoreTemp(i).second);
    }
}

void MainWindow::printCoreFreq() {
    clearBox(coreFreqWin);
    int n;
    int maxWinHeight = getmaxy(coreFreqWin);
    if(cpuInfo->cpuCores() > maxWinHeight-2) n = maxWinHeight-2;
    else n = cpuInfo->cpuCores();
    for(int i = 0; i < n; ++i) {
        wmove(coreFreqWin, i+1, 1);
        wprintw(coreFreqWin, "CPU%d: %d MHz", i, int(cpuInfo->getCoreFreq(i)));
    }
}

void MainWindow::printAllWin() {
    printCpuGraphic();
    printRamGraphic();
    printCoreUsage();
    printCoreTemps();
    printCoreFreq();
}

void MainWindow::show() {
    int c;
    timeout(100);
    while((c = getch())) {
        if(c == -1) {
            cpuInfo->updateStats();
            ramInfo->updateStats();
            printAllWin();
            refreshAllWin();
        }
        
        if(c == KEY_RESIZE) {
            resize();
        }
        else if(c >= '0' and c <= '9') {
            int decimal_size = 0;
            string num;
            num.push_back(c);
            setRefreshRate(num);
            bool point = false;
            timeout(3000);
            while(decimal_size < 4 and (c = getch())) {
                if(c == '.') {
                    if(point) break;
                    else {
                        num.push_back(c);
                        setRefreshRate(num);
                        point = true;
                    }
                }
                else if(c >= '0' and c <= '9') {
                    num.push_back(c);
                    setRefreshRate(num);
                }
                else if(c == '\n') {
                    if(stod(num) >= 0.01) refreshRate = stod(num);
                    break;
                }
                else break;
                
                if(point) ++decimal_size;
            }
            wclear(refreshRateWin);
            wprintw(refreshRateWin, "Refreshing every %.2f seconds", refreshRate);
            wrefresh(refreshRateWin);
        }
        else if (c ==KEY_F(2)) {
        }
        else if(c == 'q') break;
        timeout(refreshRate*1000);
    }
}

MainWindow::~MainWindow() {
    endwin();
}
