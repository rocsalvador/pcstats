#include "procswindow.hh"

ProcsWindow::ProcsWindow() {
    processInfo = new ProcessInfo();
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
    int pidPos = maxStdsrcWidth / 9 * 2;
    int statusPos = maxStdsrcWidth / 9 * 3;
    int threadsPos = maxStdsrcWidth / 9 * 4;
    int writePos = maxStdsrcWidth / 9 * 5;
    int readPos = maxStdsrcWidth / 9 * 7;
    box(procsWin, 0, 0);
    wattron(procsWin, A_BOLD);
    wmove(procsWin, 1, 1);
    wprintw(procsWin, "NAME");
    wmove(procsWin, 1, pidPos);
    wprintw(procsWin, "PID");
    wmove(procsWin, 1, statusPos);
    wprintw(procsWin, "STATUS");
    wmove(procsWin, 1, threadsPos);
    wprintw(procsWin, "THREADS");
    wmove(procsWin, 1, writePos);
    wprintw(procsWin, "WRITE");
    wmove(procsWin, 1, readPos);
    wprintw(procsWin, "READ");
    wattroff(procsWin, A_BOLD);
}

void ProcsWindow::print() {
    clearBox(procsWin, 2);
    int maxProcsWinHeight, maxProcsWinWidth;
    getmaxyx(procsWin, maxProcsWinHeight, maxProcsWinWidth);
    int pidPos = maxProcsWinWidth / 9 * 2;
    int statusPos = maxProcsWinWidth / 9 * 3;
    int threadsPos = maxProcsWinWidth / 9 * 4;
    int writePos = maxProcsWinWidth / 9 * 5;
    int readPos = maxProcsWinWidth / 9 * 7;

    maxScroll = processInfo->getNProcs();
    for (int i = 0, procIdx = i + scrollPos;
         procIdx < processInfo->getNProcs() and i < maxProcsWinHeight - 3;
         ++i, ++procIdx)
    {
        wmove(procsWin, i + 2, 1);
        wprintw(procsWin, "%s", processInfo->getProcName(procIdx).c_str());
        wmove(procsWin, i + 2, pidPos);
        wprintw(procsWin, "%d", processInfo->getProcPid(procIdx));
        wmove(procsWin, i + 2, statusPos);
        wprintw(procsWin, "%s", processInfo->getProcState(procIdx).c_str());
        wmove(procsWin, i + 2, threadsPos);
        wprintw(procsWin, "%d", processInfo->getProcThreads(procIdx));
        wmove(procsWin, i + 2, writePos);
        wprintw(procsWin, "%.2f", processInfo->getWriteKB(procIdx));
        wmove(procsWin, i + 2, readPos);
        wprintw(procsWin, "%.2f", processInfo->getReadKB(procIdx));
    }
}

void ProcsWindow::input(int key) {
    if (key == KEY_DOWN and scrollPos < maxScroll - 1) ++scrollPos;
    else if (key == KEY_UP and scrollPos > 0) --scrollPos;
    refresh();
    print();
    timeout(0);
}

void ProcsWindow::update() {
    processInfo->update();
    maxScroll = processInfo->getNProcs();
}

ProcsWindow::~ProcsWindow() {
    delete processInfo;
}
