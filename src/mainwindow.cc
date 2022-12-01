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
    wprintw(refreshRateWin, "Refreshing every %s seconds", refresh_rate.c_str());
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
    refreshRateWin = newwin(1, maxStdsrcWidth, maxStdsrcHeight - 1, 0);
    wprintw(refreshRateWin, "Refreshing every %.2f seconds", refreshRate);
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
}

void MainWindow::show() {
    int c;
    timeout(100);
    while((c = getch())) {
        if(c == -1) {
            statsWindow->update();
            procsWindow->update();
            print();
            refresh();
        }
        
        if(c == KEY_RESIZE) {
            resize();
            timeout(100);
        }
        else if(c >= '0' and c <= '9') {
            int decimal_size = 0;
            string num;
            num.push_back(c);
            setRefreshRate(num);
            bool point = false;
            timeout(3000);
            while(decimal_size < 4 and (c = getch())) {
                if(c == '.') {
                    if(point) break;
                    else {
                        num.push_back(c);
                        setRefreshRate(num);
                        point = true;
                    }
                }
                else if(c >= '0' and c <= '9') {
                    num.push_back(c);
                    setRefreshRate(num);
                }
                else if(c == '\n') {
                    if(stod(num) >= 0.01) refreshRate = stod(num);
                    break;
                }
                else break;
                
                if(point) ++decimal_size;
            }
            wclear(refreshRateWin);
            wprintw(refreshRateWin, "Refreshing every %.2f seconds", refreshRate);
            wrefresh(refreshRateWin);
            timeout(refreshRate * 1000);
        }
        else if (c == KEY_F(2) and currentWindow != 2) {
            currentWindow = 2;
            resize();
            timeout(0);
        }
        else if (c == KEY_F(1) and currentWindow != 1) {
            currentWindow = 1;
            resize();
            timeout(0);
        }
        else if (c == KEY_DOWN and currentWindow == 2 and scrollPos < maxScroll - 1) {
            ++scrollPos;
            refresh();
            print();
            timeout(0);
        }
        else if (c == KEY_UP and currentWindow == 2 and scrollPos > 0) {
            --scrollPos;
            refresh();
            print();
            timeout(0);
        }
        else if(c == 'q') break;
        else timeout(refreshRate*1000);
    }
}

MainWindow::~MainWindow() {
    endwin();
    delete procsWindow;
    delete statsWindow;
}
