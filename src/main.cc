#include "main_window.hh"

void usage() {
    cout << "Usage:" << endl;
    cout << "pcstats [OPTIONS]" << endl;
    cout << "-n refresh_rate    set refresh_rate (default: 1s)" << endl;
}

int main(int argc, char* argv[]) {
    double refresh_rate = 1;
    
    vector<string> arguments = {"-n"};
    if(argc > 1) {
        if(argc == 3 and argv[1] == arguments[0]) {
            refresh_rate = stod(argv[2]);
            if(refresh_rate < 0.01) refresh_rate = 1;
        }
        else if(argc >= 2) {
            usage();
            return 0;
        }
    }
    
    main_window win(refresh_rate);
    
    win.show();
}
