#ifndef PROCS_WINDOW_HH
#define PROCS_WINDOW_HH

#include "processinfo.hh"
#include <cstdlib>
#include <list>
#include <ncurses.h>
using namespace std;

class ProcsWindow
{
private:
    WINDOW *procsWin;
    WINDOW *statusBar;

    int maxStdsrcHeight, maxStdsrcWidth;

    int coreWinsWidth;

    double refreshRate;

    ProcessInfo *processInfo;

    int currentWindow = 1;

    int scrollPos = 0, maxScroll = 0;
    int searchedProcPos = -1;
    string searchedProcName;

    int columns;
    vector<string> columnsName;
    vector<int> columnsWeight;

    ProcessInfo::sortBy currentSortBy = ProcessInfo::sortBy::NAME;

    // Private functions

    void clearBox(WINDOW *win, int y);

public:
    ProcsWindow();

    void print();

    void printProcsWin();

    void printStatusBar(int key);

    void refresh();

    void resize();

    void update();

    void input(int key);

    ~ProcsWindow();
};

#endif
