#include "statswindow.hh"
#include <curses.h>
#include <ncurses.h>

StatsWindow::StatsWindow() {
    cpuInfo = new CpuInfo();
    ramInfo = new RamInfo();
}

void StatsWindow::maximumWinSizes() {
    coreWinsWidth = 17;   // |CPUxx: ---- MHz|
    for(int i = 0; i < cpuInfo->cpuSensors(); ++i) {
        if(coreWinsWidth < cpuInfo->getCoreTemp(i).first.size() + 10)
            coreWinsWidth = cpuInfo->getCoreTemp(i).first.size() + 10;
    }
}

void StatsWindow::clearBox(WINDOW* win, int y) {
    int maxWinHeight, maxWinWidth;
    getmaxyx(win, maxWinHeight, maxWinWidth);
    for(int i = y; i < maxWinHeight-1; ++i) {
        wmove(win, i, 1);
        wclrtoeol(win);
        wmove(win, i, maxWinWidth-1);
        waddch(win, ACS_VLINE);
    }
}

void StatsWindow::refresh() {
    wrefresh(cpuUsageWin);
    wrefresh(ramUsageWin);
    wrefresh(coreUsageWin);
    wrefresh(coreTempsWin);
    wrefresh(coreFreqWin);
    wrefresh(refreshRateWin);
}


void StatsWindow::printCpuGraphic() {
    unsigned int maxWinHeight, maxWinWidth;
    getmaxyx(cpuUsageWin, maxWinHeight, maxWinWidth);
    double cpu_usage = cpuInfo->getCoreUsage(-1);
    int height = cpu_usage/100*(maxWinHeight-2);
    
    cpuUsageHistory.push_back(height);
    if(maxWinWidth-1 == cpuUsageHistory.size()) {
        cpuUsageHistory.pop_front();
        clearBox(cpuUsageWin, 1);
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

void StatsWindow::printRamGraphic() {
    unsigned int maxWinHeight, maxWinWidth;
    getmaxyx(ramUsageWin, maxWinHeight, maxWinWidth);
    double ram_usage = ramInfo->getRamUsage();
    int height = ram_usage/100*(maxWinHeight-2);
    
    ramUsageHistory.push_back(height);
    if(maxWinWidth-1 == ramUsageHistory.size()) {
        ramUsageHistory.pop_front();
        clearBox(ramUsageWin, 1);
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

void StatsWindow::resize() {
    cpuUsageHistory.clear();
    ramUsageHistory.clear();
        
    getmaxyx(stdscr, maxStdsrcHeight, maxStdsrcWidth);

    maximumWinSizes();
    int cpuUsageWinHeight = (maxStdsrcHeight - 1)/2;
    int ramUsageWinHeight = maxStdsrcHeight - 1 - cpuUsageWinHeight;
    int coreUsageWinHeight = maxStdsrcHeight/3;
    int coreTempsWinHeight = coreUsageWinHeight;
    int coreFreqWinHeight = maxStdsrcHeight - 2*coreUsageWinHeight - 1;
    int coreFreqWinPos = 2*coreUsageWinHeight;
    int graphicsWinWidth = maxStdsrcWidth-coreWinsWidth;
    
    cpuUsageWin = newwin(cpuUsageWinHeight, graphicsWinWidth, 0, 0);
    box(cpuUsageWin, 0, 0);
    wmove(cpuUsageWin, 0, 1), wprintw(cpuUsageWin, "CPU usage");
    wmove(cpuUsageWin, 0, graphicsWinWidth-12), wprintw(cpuUsageWin, "[%.2f]", cpuInfo->getCoreUsage(-1));
    
    ramUsageWin = newwin(ramUsageWinHeight, graphicsWinWidth, cpuUsageWinHeight, 0);
    box(ramUsageWin, 0, 0);
    wmove(ramUsageWin, 0, 1), wprintw(ramUsageWin, "RAM usage");
    wmove(ramUsageWin, 0, graphicsWinWidth-17);
    wprintw(ramUsageWin, "[%.2f/%.2f GB]", ramInfo->getRamUsage()/100*ramInfo->getTotalRam(), ramInfo->getTotalRam());
    
    coreUsageWin = newwin(coreUsageWinHeight, coreWinsWidth, 0, graphicsWinWidth);
    box(coreUsageWin, 0, 0);
    wmove(coreUsageWin, 0, 1), wprintw(coreUsageWin, "Core usage");
    
    coreTempsWin = newwin(coreTempsWinHeight, coreWinsWidth, coreUsageWinHeight, graphicsWinWidth);
    box(coreTempsWin, 0, 0);
    wmove(coreTempsWin, 0, 1), wprintw(coreTempsWin, "Core temps");
    
    coreFreqWin = newwin(coreFreqWinHeight, coreWinsWidth, coreFreqWinPos, graphicsWinWidth);
    box(coreFreqWin, 0, 0);
    wmove(coreFreqWin, 0, 1), wprintw(coreFreqWin, "Core frequency");
}

void StatsWindow::printCoreUsage() {
    clearBox(coreUsageWin, 1);
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

void StatsWindow::printCoreTemps() {
    clearBox(coreTempsWin, 1);
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

void StatsWindow::printCoreFreq() {
    clearBox(coreFreqWin, 1);
    int n;
    int maxWinHeight = getmaxy(coreFreqWin);
    if(cpuInfo->cpuCores() > maxWinHeight-2) n = maxWinHeight-2;
    else n = cpuInfo->cpuCores();
    for(int i = 0; i < n; ++i) {
        wmove(coreFreqWin, i+1, 1);
        wprintw(coreFreqWin, "CPU%d: %d MHz", i, int(cpuInfo->getCoreFreq(i)));
    }
}

void StatsWindow::print() {
    printCpuGraphic();
    printRamGraphic();
    printCoreUsage();
    printCoreTemps();
    printCoreFreq();
}

void StatsWindow::update() {
    cpuInfo->updateStats();
    ramInfo->updateRamUsage();
}


StatsWindow::~StatsWindow() {
    delete cpuInfo, ramInfo;
}
