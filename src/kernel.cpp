#include <vector>
#include <signal.h> // pid_t

#include "Channel.hpp"

using namespace std;

/*
 * starting point of kernel, given pid of disk and the processes
 * @diskPID: pid of disk
 * @diskChannel: up/down streams to talk to disk
 * @procs: pairs of proc pid and its up/down channel
 * 
 * @return exit code of kernel
 */
int kernel_main(pid_t diskPID, Channel diskChannel, 
    vector<pair<pid_t, Channel>> procs) {
    return 0;
}