#include "mainwindow.hh"

MainWindow::MainWindow(const double& refreshRate) {
    procsWindow = new ProcsWindow();
    statsWindow = new StatsWindow();
    
    this->refreshRate = refreshRate;
    refreshRateStr = to_string(refreshRate);
    refreshRateStr = refreshRateStr.substr(0, refreshRateStr.find(".") + 3);

    // Init curses screen
    initscr();
    noecho();
    start_color();
    curs_set(0);
    keypad(stdscr, true);
    int LIGHT_GREY = 237;
    init_pair(1, COLOR_WHITE, LIGHT_GREY);
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

void MainWindow::printStatusBar() {
    getmaxyx(stdscr, maxStdsrcHeight, maxStdsrcWidth);
    statusBar = newwin(1, maxStdsrcWidth, maxStdsrcHeight - 1, 0);
    wprintw(statusBar, "Refresh rate: %s s ", refreshRateStr.c_str());
    waddch(statusBar, ACS_VLINE);
    if (currentWindow == 1) wattron(statusBar, A_STANDOUT);
    wprintw(statusBar, " STATS WIN (F1) ", refreshRate);
    if (currentWindow == 1) wattroff(statusBar, A_STANDOUT);
    waddch(statusBar, ACS_VLINE);
    if (currentWindow == 2) wattron(statusBar, A_STANDOUT);
    wprintw(statusBar, " PROCS WIN (F2) ", refreshRate);
    if (currentWindow == 2) wattroff(statusBar, A_STANDOUT);
    waddch(statusBar, ACS_VLINE);
    if (currentWindow == 2) {
        wprintw(statusBar, " SEARCH (F3) ", refreshRate);
        waddch(statusBar, ACS_VLINE);
        wprintw(statusBar, " KILL (F4) ", refreshRate);
        waddch(statusBar, ACS_VLINE);
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
    printStatusBar();
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
    wrefresh(statusBar);
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
            refreshRateStr = num;
            printStatusBar();
            wrefresh(statusBar);
            bool point = false;
            timeout(3000);
            while(decimal_size < 4 and (key = getch())) {
                if(key == '.') {
                    if(point) break;
                    else {
                        num.push_back(key);
                        refreshRateStr = num;
                        printStatusBar();
                        wrefresh(statusBar);
                        point = true;
                    }
                }
                else if(key >= '0' and key <= '9') {
                    num.push_back(key);
                    refreshRateStr = num;
                    printStatusBar();
                    wrefresh(statusBar);
                }
                else if(key == '\n') {
                    if(stod(num) >= 0.01) refreshRate = stod(num);
                    break;
                }
                else break;
                
                if(point) ++decimal_size;
            }
            refreshRateStr = num;
            printStatusBar();        
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
