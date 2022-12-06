#include "procswindow.hh"

ProcsWindow::ProcsWindow() {
    processInfo = new ProcessInfo();
    columns = 10;
    columnsName = {"NAME", "PID", "STATUS", "THREADS", "CPU", "WRITE", "READ"};
    columnsWeight = {0, 2, 3, 4, 5, 7, 9};
}

void ProcsWindow::clearBox(WINDOW* win, int y) {
    int maxWinHeight, maxWinWidth;
    getmaxyx(win, maxWinHeight, maxWinWidth);
    for(int i = y; i < maxWinHeight-1; ++i) {
        wmove(win, i, 1);
        wclrtoeol(win);
        wmove(win, i, maxWinWidth-1);
        waddch(win, ACS_VLINE);
    }
}

void ProcsWindow::refresh() {
    wrefresh(procsWin);
}

void ProcsWindow::resize() {
    getmaxyx(stdscr, maxStdsrcHeight, maxStdsrcWidth);
    procsWin = newwin(maxStdsrcHeight - 1, maxStdsrcWidth, 0, 0);
    wclear(procsWin);
    int columnSize = maxStdsrcWidth / columns;
    vector<int> columnsPos = vector<int> (columnsName.size());
    for (int i = 0; i < columnsPos.size(); ++i) columnsPos[i] = columnsWeight[i] * columnSize + 1;
    box(procsWin, 0, 0);
    wattron(procsWin, A_BOLD);
    for (int i = 0; i < columnsName.size(); ++i) {
        wmove(procsWin, 1, columnsPos[i]);
        wprintw(procsWin, columnsName[i].c_str());
    }
    wattroff(procsWin, A_BOLD);
}

void ProcsWindow::print() {
    clearBox(procsWin, 2);
    int maxProcsWinHeight, maxProcsWinWidth;
    getmaxyx(procsWin, maxProcsWinHeight, maxProcsWinWidth);
    int columnSize = maxProcsWinWidth / columns;
    int pidPos = columnSize * 2 + 1;
    int statusPos = columnSize * 3 + 1;
    int threadsPos = columnSize * 4 + 1;
    int cpuUsagePos = columnSize * 5 + 1;
    int writePos = columnSize * 7 + 1;
    int readPos = columnSize * 9 + 1;

    maxScroll = processInfo->getNProcs();
    for (int i = 0, procIdx = i + scrollPos;
         procIdx < processInfo->getNProcs() and i < maxProcsWinHeight - 3;
         ++i, ++procIdx)
    {
        if (searchedProcPos == procIdx) wattron(procsWin, A_STANDOUT);
        wmove(procsWin, i + 2, 1);
        wprintw(procsWin, "%s", processInfo->getProcName(procIdx).substr(0, pidPos - 2).c_str());
        wmove(procsWin, i + 2, pidPos);
        wprintw(procsWin, "%d", processInfo->getProcPid(procIdx));
        wmove(procsWin, i + 2, statusPos);
        wprintw(procsWin, "%s", processInfo->getProcState(procIdx).c_str());
        wmove(procsWin, i + 2, threadsPos);
        wprintw(procsWin, "%d", processInfo->getProcThreads(procIdx));
        wmove(procsWin, i + 2, cpuUsagePos);
        wprintw(procsWin, "%.2f %", processInfo->getCpuUsage(procIdx));
        wmove(procsWin, i + 2, writePos);
        wprintw(procsWin, "%.2f", processInfo->getWriteKB(procIdx));
        wmove(procsWin, i + 2, readPos);
        wprintw(procsWin, "%.2f", processInfo->getReadKB(procIdx));
        if (searchedProcPos == procIdx) wattroff(procsWin, A_STANDOUT);
    }
}

void ProcsWindow::input(int key) {
    if (key == KEY_DOWN and scrollPos < maxScroll - 1) ++scrollPos;
    else if (key == KEY_UP and scrollPos > 0) --scrollPos;
    else if (key == KEY_F(3)) {
        timeout(3000);
        int searchKey;
        searchedProcName = "";
        while ((searchKey = getch())) {
            if (searchKey == '\n') break;
            else searchedProcName.push_back(char(searchKey));
        }

        searchedProcPos = processInfo->getProcIndex(searchedProcName);
        if (searchedProcPos != -1) scrollPos = searchedProcPos;
    }
    refresh();
    print();
    timeout(0);
}

void ProcsWindow::update() {
    processInfo->update();
    maxScroll = processInfo->getNProcs();
    if (searchedProcPos != -1) {
        searchedProcPos = processInfo->getProcIndex(searchedProcName);
    }
}

ProcsWindow::~ProcsWindow() {
    delete processInfo;
}
