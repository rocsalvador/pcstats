#include "main_window.hh"
#include "pcstats.hh"

void usage() {
    cout << "Usage:" << endl;
    cout << "pcstats [OPTIONS]" << endl;
    cout << "-n refresh_rate    set refresh_rate (default: 1s)" << endl;
}

int main(int argc, char* argv[]) {
    double time = 1;
    
    vector<string> arguments = {"-n"};
    if(argc > 1) {
        if(argc == 3 and argv[1] == arguments[0]) {
            time = stod(argv[2]);
            if(time < 0.01) time = 1;
        }
        else if(argc >= 2) {
            usage();
            return 0;
        }
    }
    
    //Init screen
    initscr();
    noecho();
    curs_set(0);
    
    //Setup windows
    main_window win(to_string(time));
    
    int c;
    timeout(100);
    while((c = getch())) {
        if(c == -1) {
            win.update_stats();
            win.print_all_win();
            win.refresh_all_win();
        }
        
        if(c == KEY_RESIZE) {
            win.resize();
        }
        else if(c >= '0' and c <= '9') {
            string num;
            num.push_back(c);
            win.set_refresh_rate(num);
            bool point = false;
            timeout(-1);
            while((c = getch())) {
                if(c == '.') {
                    if(point) break;
                    else {
                        num.push_back(c);
                        win.set_refresh_rate(num);
                        point = true;
                    }
                }
                else if(c >= '0' and c <= '9') {
                    num.push_back(c);
                    win.set_refresh_rate(num); 
                }
                else if(c == '\n') {
                    if(stod(num) >= 0.01) time = stod(num);
                    break;
                }
                else break;
            }
            win.set_refresh_rate(to_string(time));
        }
        else if(c == 'q') break;
        timeout(time*1000);
    }
    endwin();
}
