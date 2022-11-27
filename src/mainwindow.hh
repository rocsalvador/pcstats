#ifndef MAIN_WINDOW_HH
#define MAIN_WINDOW_HH

#include "processinfo.hh"
#include "raminfo.hh"
#include "cpuinfo.hh"
#include <cstdlib>
#include <list>
#include <ncurses.h>
using namespace std;

class MainWindow {
private:
    //Private attributes
    
    WINDOW* cpuUsageWin;
    WINDOW* ramUsageWin;
    WINDOW* coreUsageWin;
    WINDOW* coreTempsWin;
    WINDOW* coreFreqWin;
    WINDOW* refreshRateWin;
    
    int maxStdsrcHeight, maxStdsrcWidth;

    int coreWinsWidth;
    
    list<int> cpuUsageHistory, ramUsageHistory;
    
    double refreshRate;

    CpuInfo *cpuInfo;
    RamInfo *ramInfo;
    ProcessInfo *processInfo;
    
    //Private functions
    
    void clearBox(WINDOW* win);

    void maximumWinSizes();
    
    void printCpuGraphic();
    
    void printRamGraphic();
    
    void printCoreUsage();
    
    void printCoreTemps();
    
    void printCoreFreq();
    
    void printAllWin();
    
    void refreshAllWin();
    
    void setRefreshRate(string refreshRate);
    
    void resize();
    
public:
    MainWindow(const double& refreshRate);
    
    void show();
    
    ~MainWindow();
};

#endif
