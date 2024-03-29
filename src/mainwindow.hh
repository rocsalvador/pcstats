#ifndef MAIN_WINDOW_HH
#define MAIN_WINDOW_HH

#include "processinfo.hh"
#include "raminfo.hh"
#include "cpuinfo.hh"
#include "statswindow.hh"
#include "procswindow.hh"
#include <cstdlib>
#include <curses.h>
#include <list>
#include <ncurses.h>
using namespace std;

class MainWindow {
private:
    //Private attributes
    StatsWindow* statsWindow;
    ProcsWindow* procsWindow;

    WINDOW* statusBar;
    
    int maxStdsrcHeight, maxStdsrcWidth;

    int coreWinsWidth;
    
    double refreshRate;

    string refreshRateStr;

    int currentWindow = 1;

    int scrollPos = 0, maxScroll = 0;
    
    //Private functions
    
    void clearBox(WINDOW* win, int y);
    
    void print();
    
    void refresh();

    void resize();

    void printStatusBar();
    
public:
    MainWindow(const double& refreshRate);
    
    void show();
    
    ~MainWindow();
};

#endif
