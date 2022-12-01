#ifndef STATS_WINDOW_HH
#define STATS_WINDOW_HH

#include "processinfo.hh"
#include "raminfo.hh"
#include "cpuinfo.hh"
#include <cstdlib>
#include <curses.h>
#include <list>
#include <ncurses.h>
using namespace std;

class StatsWindow {
private:
    //Private attributes
    
    WINDOW* cpuUsageWin;
    WINDOW* ramUsageWin;
    WINDOW* coreUsageWin;
    WINDOW* coreTempsWin;
    WINDOW* coreFreqWin;
    WINDOW* refreshRateWin;
    WINDOW* currentWindowWin;
    
    int maxStdsrcHeight, maxStdsrcWidth;

    int coreWinsWidth;
    
    list<int> cpuUsageHistory, ramUsageHistory;
    
    CpuInfo *cpuInfo;
    RamInfo *ramInfo;

    int scrollPos = 0, maxScroll = 0;
    
    //Private functions
    
    void clearBox(WINDOW* win, int y);

    void maximumWinSizes();
    
    void printCpuGraphic();
    
    void printRamGraphic();
    
    void printCoreUsage();
    
    void printCoreTemps();
    
    void printCoreFreq();
    
    void printAllWin();
        
public:
    StatsWindow();

    void print();

    void refresh();

    void resize();

    void update();
        
    ~StatsWindow();
};

#endif
