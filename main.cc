#include <unistd.h>
#include "pcstats.hh"
using namespace std;

pcstats stats;

void signal_treatment(int s)
{
    if(s == SIGINT) {
        char op;
        cout << " q to exit, r to reset saved stats, any else key to resume: ";
        cin >> op;
        if(op == 'q') {
            stats.print_saved_stats();
            exit(0);
        }
        else if(op == 'r')
           stats.reset_saved_stats();
        else
            return;
    }
}

void init_signals()
{
    struct sigaction sa;
    sigfillset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = &signal_treatment;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGALRM, &sa, NULL);
}

int main()
{
    init_signals();

    double time;
    system("clear");
    cout << "Refresh rate (s): ";
    cin >> time;
    if(time < 1)
    {
        cout << "ERROR: Refresh rate must be an integer greater than 0" << endl;
        return 0;
    }

    usleep(100000);
    while(true)
    {
        alarm(time);

        stats.print_stats();
        
        sigset_t mask;
        sigfillset(&mask);
        sigdelset(&mask, SIGALRM);
        sigdelset(&mask, SIGINT);
        sigsuspend(&mask);
    }
}
