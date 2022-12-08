#include "procswindow.hh"

ProcsWindow::ProcsWindow() {
    processInfo = new ProcessInfo();
    columns = 10;
    columnsName = {"NAME", "PID", "STATUS", "THREADS", "CPU (%)", "WRITE", "READ"};
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
    for (int i = 2, procIdx = i + scrollPos;
         procIdx < processInfo->getNProcs() and i < maxProcsWinHeight - 1;
         ++i, ++procIdx)
    {
        if (searchedProcPos == procIdx) wattron(procsWin, A_STANDOUT);
        else if (i % 2 == 0) {
            wattron(procsWin, COLOR_PAIR(1));
            for (int j = 1; j < maxProcsWinWidth - 1; ++j)  {
                wmove(procsWin, i, j);
                waddch(procsWin, ' ');
            }
        }
        wmove(procsWin, i, 1);
        wprintw(procsWin, "%s", processInfo->getProcName(procIdx).substr(0, pidPos - 2).c_str());
        wmove(procsWin, i, pidPos);
        wprintw(procsWin, "%d", processInfo->getProcPid(procIdx));
        wmove(procsWin, i, statusPos);
        wprintw(procsWin, "%s", processInfo->getProcState(procIdx).c_str());
        wmove(procsWin, i, threadsPos);
        wprintw(procsWin, "%d", processInfo->getProcThreads(procIdx));
        wmove(procsWin, i, cpuUsagePos);
        wprintw(procsWin, "%.2f", processInfo->getCpuUsage(procIdx));
        wmove(procsWin, i, writePos);
        wprintw(procsWin, "%.2f", processInfo->getWriteKB(procIdx));
        wmove(procsWin, i, readPos);
        wprintw(procsWin, "%.2f", processInfo->getReadKB(procIdx));
        if (searchedProcPos == procIdx) wattroff(procsWin, A_STANDOUT);
        else if (i % 2 == 0) {
            wattroff(procsWin, COLOR_PAIR(1));
        }
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
            else {
                if (searchKey == KEY_BACKSPACE) {
                    if (searchedProcName.size() > 0) searchedProcName.pop_back();
                }
                else searchedProcName.push_back(char(searchKey));
                int matchProcIdx = processInfo->getProcIndex(searchedProcName);
                if (matchProcIdx != -1) {
                    searchedProcPos = matchProcIdx;
                    scrollPos = matchProcIdx;
                    print();
                    refresh();
                } else if (searchedProcPos != -1) {
                    searchedProcPos = -1;
                    print();
                    refresh();
                }
            }
        }
    }
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
