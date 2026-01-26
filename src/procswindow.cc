#include "procswindow.hh"
#include <csignal>

ProcsWindow::ProcsWindow()
{
    processInfo = new ProcessInfo();
    columns = 12;
    columnsName = {"NAME", "PID", "STATUS", "THREADS", "CPU (%)", "MEM (MB)", "WRITE (KB/s)", "READ (KB/s)"};
    columnsWeight = {0, 2, 3, 4, 5, 7, 9, 11};
}

void ProcsWindow::clearBox(WINDOW *win, int y)
{
    int maxWinHeight, maxWinWidth;
    getmaxyx(win, maxWinHeight, maxWinWidth);
    for (int i = y; i < maxWinHeight - 1; ++i)
    {
        wmove(win, i, 1);
        wclrtoeol(win);
        wmove(win, i, maxWinWidth - 1);
        waddch(win, ACS_VLINE);
    }
}

void ProcsWindow::refresh()
{
    wrefresh(procsWin);
    wrefresh(statusBar);
}

void ProcsWindow::resize()
{
    getmaxyx(stdscr, maxStdsrcHeight, maxStdsrcWidth);
    procsWin = newwin(maxStdsrcHeight - 2, maxStdsrcWidth, 0, 0);
    statusBar = newwin(1, maxStdsrcWidth, maxStdsrcHeight - 2, 0);
    wclear(procsWin);
    wclear(statusBar);
    int columnSize = maxStdsrcWidth / columns;
    vector<int> columnsPos = vector<int>(columnsName.size());
    for (uint i = 0; i < columnsPos.size(); ++i)
        columnsPos[i] = columnsWeight[i] * columnSize + 1;
    box(procsWin, 0, 0);
    wattron(procsWin, A_BOLD);
    for (uint i = 0; i < columnsName.size(); ++i)
    {
        wmove(procsWin, 1, columnsPos[i]);
        wprintw(procsWin, columnsName[i].c_str());
    }
    wattroff(procsWin, A_BOLD);
}

void ProcsWindow::printProcsWin()
{
    clearBox(procsWin, 2);
    int maxProcsWinHeight, maxProcsWinWidth;
    getmaxyx(procsWin, maxProcsWinHeight, maxProcsWinWidth);
    int columnSize = maxProcsWinWidth / columns;
    int pidPos = columnSize * 2 + 1;
    int statusPos = columnSize * 3 + 1;
    int threadsPos = columnSize * 4 + 1;
    int cpuUsagePos = columnSize * 5 + 1;
    int memUsagePos = columnSize * 7 + 1;
    int writePos = columnSize * 9 + 1;
    int readPos = columnSize * 11 + 1;

    maxScroll = processInfo->getNProcs();
    for (int i = 2, procIdx = i + scrollPos - 2;
         procIdx < processInfo->getNProcs() and i < maxProcsWinHeight - 1;
         ++i, ++procIdx)
    {
        if (searchedProcPos == procIdx)
        {
            wattron(procsWin, A_STANDOUT);
            for (int j = 1; j < maxProcsWinWidth - 1; ++j)
            {
                wmove(procsWin, i, j);
                waddch(procsWin, ' ');
            }
        }
        else if (i % 2 == 0)
        {
            wattron(procsWin, COLOR_PAIR(1));
            for (int j = 1; j < maxProcsWinWidth - 1; ++j)
            {
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
        wmove(procsWin, i, memUsagePos);
        wprintw(procsWin, "%.2f", processInfo->getMemUsage(procIdx));
        wmove(procsWin, i, writePos);
        wprintw(procsWin, "%.2f", processInfo->getWriteKB(procIdx));
        wmove(procsWin, i, readPos);
        wprintw(procsWin, "%.2f", processInfo->getReadKB(procIdx));
        if (searchedProcPos == procIdx)
            wattroff(procsWin, A_STANDOUT);
        else if (i % 2 == 0)
            wattroff(procsWin, COLOR_PAIR(1));
    }
}

void ProcsWindow::printStatusBar(int key)
{
    wclear(statusBar);
    wattron(statusBar, COLOR_PAIR(1));
    wattron(statusBar, A_BOLD);

    uint maxStatusBarWidth = getmaxx(statusBar);
    for (uint j = 0; j < maxStatusBarWidth; ++j)
    {
        wmove(statusBar, 0, j);
        waddch(statusBar, ' ');
    }

    wmove(statusBar, 0, 0);
    waddch(statusBar, ACS_VLINE);

    if (key == KEY_F(3))
    {
        wattron(statusBar, A_STANDOUT);
        wprintw(statusBar, searchedProcName.c_str());
        for (uint i = searchedProcName.size() + 1; i < 14; ++i)
            waddch(statusBar, ' ');
        wattroff(statusBar, A_STANDOUT);
    }
    else
        wprintw(statusBar, " SEARCH (F3) ");

    waddch(statusBar, ACS_VLINE);

    wprintw(statusBar, " KILL (F4) ");

    waddch(statusBar, ACS_VLINE);

    wprintw(statusBar, " SORT BY ");
    if (currentSortBy == ProcessInfo::sortBy::NAME)
        wattron(statusBar, A_STANDOUT);
    wprintw(statusBar, "NAME");
    if (currentSortBy == ProcessInfo::sortBy::NAME)
        wattroff(statusBar, A_STANDOUT);
    wprintw(statusBar, "/");
    if (currentSortBy == ProcessInfo::sortBy::CPU)
        wattron(statusBar, A_STANDOUT);
    wprintw(statusBar, "CPU");
    if (currentSortBy == ProcessInfo::sortBy::CPU)
        wattroff(statusBar, A_STANDOUT);
    wprintw(statusBar, "/");
    if (currentSortBy == ProcessInfo::sortBy::MEM)
        wattron(statusBar, A_STANDOUT);
    wprintw(statusBar, "MEM");
    if (currentSortBy == ProcessInfo::sortBy::MEM)
        wattroff(statusBar, A_STANDOUT);

    wprintw(statusBar, " (F5/F6/F7) ");

    waddch(statusBar, ACS_VLINE);

    wattroff(statusBar, COLOR_PAIR(1));
    wattroff(statusBar, A_BOLD);
}

void ProcsWindow::print()
{
    printProcsWin();

    printStatusBar(-1);
}

void ProcsWindow::input(int key)
{
    if (key == KEY_DOWN and scrollPos < maxScroll - 1)
        ++scrollPos;
    else if (key == KEY_UP and scrollPos > 0)
        --scrollPos;
    else if (key == KEY_NPAGE)
    {
        scrollPos += (maxStdsrcHeight - 3);
        if (scrollPos > maxScroll - 1)
            scrollPos = maxScroll - 1;
    }
    else if (key == KEY_PPAGE)
    {
        scrollPos -= (maxStdsrcHeight - 3);
        if (scrollPos < 0)
            scrollPos = 0;
    }
    else if (key == KEY_F(3))
    {
        timeout(3000);
        int searchKey;
        searchedProcName = "";
        printStatusBar(KEY_F(3));
        wrefresh(statusBar);
        while ((searchKey = getch()))
        {
            if (searchKey == '\n' or searchKey == -1)
                break;
            else
            {
                if (searchKey == KEY_BACKSPACE and searchedProcName.size() > 0)
                    searchedProcName.pop_back();
                else
                    searchedProcName.push_back(char(searchKey));
                printStatusBar(KEY_F(3));
                wrefresh(statusBar);
                int matchProcIdx = processInfo->getProcIndex(searchedProcName);
                if (matchProcIdx != -1)
                {
                    searchedProcPos = matchProcIdx;
                    scrollPos = matchProcIdx;
                    printProcsWin();
                    wrefresh(procsWin);
                }
                else if (searchedProcPos != -1)
                {
                    searchedProcPos = -1;
                    print();
                    refresh();
                }
            }
        }
    }
    else if (key == KEY_F(4))
    {
        if (searchedProcPos != -1)
        {
            kill(processInfo->getProcPid(searchedProcPos), SIGKILL);
        }
    }
    else if (key >= KEY_F(5) and key <= KEY_F(7))
    {
        currentSortBy = static_cast<ProcessInfo::sortBy>(key - KEY_F(5));
        processInfo->sortProcesses(currentSortBy);
        searchedProcPos = -1;
        scrollPos = 0;
    }
    timeout(0);
}

void ProcsWindow::update()
{
    processInfo->update();
    maxScroll = processInfo->getNProcs();
    processInfo->sortProcesses(currentSortBy);
    if (searchedProcPos != -1)
        searchedProcPos = processInfo->getProcIndex(searchedProcName);
}

ProcsWindow::~ProcsWindow()
{
    delete processInfo;
}
