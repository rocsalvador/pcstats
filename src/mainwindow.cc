#include "mainwindow.hh"

MainWindow::MainWindow(const double& refreshRate) {
    procsWindow = new ProcsWindow();
    statsWindow = new StatsWindow();
    
    this->refreshRate = refreshRate;

    // Init curses screen
    initscr();
    noecho();
    curs_set(0);
    keypad(stdscr, true);
    resize();
}

void MainWindow::clearBox(WINDOW* win, int y) {
    int maxWinHeight, maxWinWidth;
    getmaxyx(win, maxWinHeight, maxWinWidth);
    for(int i = y; i < maxWinHeight-1; ++i) {
        wmove(win, i, 1);
        wclrtoeol(win);
        wmove(win, i, maxWinWidth-1);
        waddch(win, ACS_VLINE);
    }
}

void MainWindow::setRefreshRate(string refresh_rate) {
    wclear(refreshRateWin);
    wprintw(refreshRateWin, "Refresh rate: %s s", refresh_rate.c_str());
    wrefresh(refreshRateWin);
}

void MainWindow::print() {
    switch (currentWindow) {
        case 1:
            statsWindow->print();
            break;
        case 2:
            procsWindow->print();
            break;
    }
}

void MainWindow::resize() {
    switch (currentWindow) {
        case 1:
            statsWindow->resize();
            break;
        case 2:
            procsWindow->resize();
            break;
    }
    getmaxyx(stdscr, maxStdsrcHeight, maxStdsrcWidth);
    refreshRateWin = newwin(1, maxStdsrcWidth / 2, maxStdsrcHeight - 1, 0);
    wprintw(refreshRateWin, "Refresh rate: %.2f s", refreshRate);
    currentWindowWin = newwin(1, maxStdsrcWidth / 2, maxStdsrcHeight - 1, maxStdsrcWidth / 2);
    wattron(currentWindowWin, A_BOLD);
    waddch(currentWindowWin, ACS_VLINE);
    if (currentWindow == 1) wattron(currentWindowWin, A_STANDOUT);
    wprintw(currentWindowWin, " STATS WIN (F1) ", refreshRate);
    if (currentWindow == 1) wattroff(currentWindowWin, A_STANDOUT);
    waddch(currentWindowWin, ACS_VLINE);
    if (currentWindow == 2) wattron(currentWindowWin, A_STANDOUT);
    wprintw(currentWindowWin, " PROCS WIN (F2) ", refreshRate);
    if (currentWindow == 2) wattroff(currentWindowWin, A_STANDOUT);
    waddch(currentWindowWin, ACS_VLINE);
    wattroff(currentWindowWin, A_BOLD);
}

void MainWindow::refresh() {
    switch (currentWindow) {
        case 1:
            statsWindow->refresh();
            break;
        case 2:
            procsWindow->refresh();
            break;
    }
    wrefresh(refreshRateWin);
    wrefresh(currentWindowWin);
}

void MainWindow::show() {
    int key;
    timeout(100);
    while((key = getch())) {
        if(key == -1) {
            statsWindow->update();
            procsWindow->update();
            print();
            refresh();
            timeout(refreshRate*1000);
        }
        else if(key == KEY_RESIZE) {
            resize();
            timeout(100);
        }
        else if(key >= '0' and key <= '9') {
            int decimal_size = 0;
            string num;
            num.push_back(key);
            setRefreshRate(num);
            bool point = false;
            timeout(3000);
            while(decimal_size < 4 and (key = getch())) {
                if(key == '.') {
                    if(point) break;
                    else {
                        num.push_back(key);
                        setRefreshRate(num);
                        point = true;
                    }
                }
                else if(key >= '0' and key <= '9') {
                    num.push_back(key);
                    setRefreshRate(num);
                }
                else if(key == '\n') {
                    if(stod(num) >= 0.01) refreshRate = stod(num);
                    break;
                }
                else break;
                
                if(point) ++decimal_size;
            }
            wclear(refreshRateWin);
            wprintw(refreshRateWin, "Refresh rate: %.2f s", refreshRate);
            wrefresh(refreshRateWin);
            timeout(refreshRate * 1000);
        }
        else if (key == KEY_F(2) and currentWindow != 2) {
            currentWindow = 2;
            resize();
            timeout(0);
        }
        else if (key == KEY_F(1) and currentWindow != 1) {
            currentWindow = 1;
            resize();
            timeout(0);
        }
        else if(key == 'q') break;
        else if (currentWindow == 2) procsWindow->input(key); 
    }
}

MainWindow::~MainWindow() {
    endwin();
    delete procsWindow;
    delete statsWindow;
}
