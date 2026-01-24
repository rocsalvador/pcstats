#include "statswindow.hh"
#include <curses.h>
#include <ncurses.h>

StatsWindow::StatsWindow()
{
    cpuInfo = new CpuInfo();
    ramInfo = new RamInfo();
}

void StatsWindow::maximumWinSizes()
{
    coreWinsWidth = 17; // |CPUxx: ---- MHz|
    for (int i = 0; i < cpuInfo->cpuSensors(); ++i)
    {
        if (coreWinsWidth < cpuInfo->getCoreTemp(i).first.size() + 10)
            coreWinsWidth = cpuInfo->getCoreTemp(i).first.size() + 10;
    }
}

void StatsWindow::clearBox(WINDOW *win, int y)
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

void StatsWindow::refresh()
{
    wrefresh(cpuUsageWin);
    wrefresh(ramUsageWin);
    wrefresh(coreUsageWin);
    wrefresh(coreTempsWin);
    wrefresh(coreFreqWin);
    wrefresh(refreshRateWin);
}

void StatsWindow::printCpuGraphic()
{
    double cpu_usage = cpuInfo->getCoreUsage(-1);
    unsigned int maxWinHeight, maxWinWidth;
    getmaxyx(cpuUsageWin, maxWinHeight, maxWinWidth);

    if (cpuUsageHistory.size() == maxWinWidth - 2)
    {
        clearBox(cpuUsageWin, 1);
    }

    int j = 1;
    for (double cpuUsage : cpuUsageHistory)
    {
        uint cpuUsageHeight = cpuUsage / 100 * (maxWinHeight - 2);
        for (uint i = 0; i < cpuUsageHeight; ++i)
        {
            wmove(cpuUsageWin, maxWinHeight - i - 2, j);
            waddch(cpuUsageWin, ACS_CKBOARD);
        }
        ++j;
    }

    wmove(cpuUsageWin, 0, maxStdsrcWidth - coreWinsWidth - 12);
    if (cpu_usage == 100)
        wprintw(cpuUsageWin, "[%.2f]", cpuInfo->getCoreUsage(-1));
    else if (cpu_usage >= 10)
        wprintw(cpuUsageWin, "[ %.2f]", cpuInfo->getCoreUsage(-1));
    else
        wprintw(cpuUsageWin, "[  %.2f]", cpuInfo->getCoreUsage(-1));
}

void StatsWindow::printRamGraphic()
{
    unsigned int maxWinHeight, maxWinWidth;
    getmaxyx(ramUsageWin, maxWinHeight, maxWinWidth);

    if (ramUsageHistory.size() == maxWinWidth - 2)
    {
        clearBox(ramUsageWin, 1);
    }

    int j = 1;
    for (double ramUsage : ramUsageHistory)
    {
        uint ramUsageHeight = ramUsage / 100 * (maxWinHeight - 2);
        for (uint i = 0; i < ramUsageHeight; ++i)
        {
            wmove(ramUsageWin, maxWinHeight - i - 2, j);
            waddch(ramUsageWin, ACS_CKBOARD);
        }
        ++j;
    }
    wmove(ramUsageWin, 0, maxStdsrcWidth - coreWinsWidth - 17);
    wprintw(ramUsageWin, "[%.2f/%.2f GB]", ramInfo->getRamUsage() / 100 * ramInfo->getTotalRam(), ramInfo->getTotalRam());
}

void StatsWindow::resize()
{
    getmaxyx(stdscr, maxStdsrcHeight, maxStdsrcWidth);

    clearBox(cpuUsageWin, 1);
    clearBox(ramUsageWin, 1);

    maximumWinSizes();
    int cpuUsageWinHeight = (maxStdsrcHeight - 1) / 2;
    int ramUsageWinHeight = maxStdsrcHeight - 1 - cpuUsageWinHeight;
    int coreUsageWinHeight = maxStdsrcHeight / 3;
    int coreTempsWinHeight = coreUsageWinHeight;
    int coreFreqWinHeight = maxStdsrcHeight - 2 * coreUsageWinHeight - 1;
    int coreFreqWinPos = 2 * coreUsageWinHeight;
    int graphicsWinWidth = maxStdsrcWidth - coreWinsWidth;

    cpuUsageWin = newwin(cpuUsageWinHeight, graphicsWinWidth, 0, 0);
    box(cpuUsageWin, 0, 0);
    wmove(cpuUsageWin, 0, 1), wprintw(cpuUsageWin, "CPU usage");
    wmove(cpuUsageWin, 0, graphicsWinWidth - 12), wprintw(cpuUsageWin, "[%.2f]", cpuInfo->getCoreUsage(-1));

    ramUsageWin = newwin(ramUsageWinHeight, graphicsWinWidth, cpuUsageWinHeight, 0);
    box(ramUsageWin, 0, 0);
    wmove(ramUsageWin, 0, 1), wprintw(ramUsageWin, "RAM usage");
    wmove(ramUsageWin, 0, graphicsWinWidth - 17);
    wprintw(ramUsageWin, "[%.2f/%.2f GB]", ramInfo->getRamUsage() / 100 * ramInfo->getTotalRam(), ramInfo->getTotalRam());

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

void StatsWindow::printCoreUsage()
{
    clearBox(coreUsageWin, 1);
    wrefresh(coreUsageWin);
    int n;
    int maxWinHeight, maxWinWidth;
    getmaxyx(coreUsageWin, maxWinHeight, maxWinWidth);
    if (cpuInfo->cpuCores() > maxWinHeight - 2)
        n = maxWinHeight - 2;
    else
        n = cpuInfo->cpuCores();
    for (int i = 0; i < n; ++i)
    {
        wmove(coreUsageWin, i + 1, 1);
        wprintw(coreUsageWin, "CPU%d: %.2f %%", i, cpuInfo->getCoreUsage(i));
    }
}

void StatsWindow::printCoreTemps()
{
    clearBox(coreTempsWin, 1);
    wrefresh(coreTempsWin);
    int n;
    int maxWinHeight = getmaxy(coreTempsWin);
    if (cpuInfo->cpuSensors() > maxWinHeight - 2)
        n = maxWinHeight - 2;
    else
        n = cpuInfo->cpuSensors();
    for (int i = 0; i < n; ++i)
    {
        wmove(coreTempsWin, i + 1, 1);
        wprintw(coreTempsWin, "%s: %d ºC", cpuInfo->getCoreTemp(i).first.c_str(), cpuInfo->getCoreTemp(i).second);
    }
}

void StatsWindow::printCoreFreq()
{
    clearBox(coreFreqWin, 1);
    int n;
    int maxWinHeight = getmaxy(coreFreqWin);
    if (cpuInfo->cpuCores() > maxWinHeight - 2)
        n = maxWinHeight - 2;
    else
        n = cpuInfo->cpuCores();
    for (int i = 0; i < n; ++i)
    {
        wmove(coreFreqWin, i + 1, 1);
        wprintw(coreFreqWin, "CPU%d: %d MHz", i, int(cpuInfo->getCoreFreq(i)));
    }
}

void StatsWindow::print()
{
    printCpuGraphic();
    printRamGraphic();
    printCoreUsage();
    printCoreTemps();
    printCoreFreq();
}

void StatsWindow::update()
{
    cpuInfo->updateStats();
    ramInfo->updateRamUsage();

    uint cpuUsageWinWidth = getmaxx(cpuUsageWin);

    while (cpuUsageHistory.size() >= cpuUsageWinWidth - 2)
    {
        cpuUsageHistory.pop_front();
    }

    double cpuUsage = cpuInfo->getCoreUsage(-1);
    cpuUsageHistory.push_back(cpuUsage);

    uint ramUsageWinWidth = getmaxx(ramUsageWin);

    while (ramUsageHistory.size() >= ramUsageWinWidth - 2)
    {
        ramUsageHistory.pop_front();
    }

    double ramUsage = ramInfo->getRamUsage();
    ramUsageHistory.push_back(ramUsage);
}

StatsWindow::~StatsWindow()
{
    delete cpuInfo;
    delete ramInfo;
}
