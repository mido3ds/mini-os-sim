#include <signal.h> // pid_t
#include <sys/wait.h> // wait
#include <fstream>
#include <vector>
#include <iostream>
#include "Channel.hpp"
#include <unistd.h>
using namespace std;

#include <iostream>
#include <chrono>

/* Definiton for SIGCHLD handler */
void sigchld_handler(int signum) {
    int pid, stat_loc;
    pid = wait(&stat_loc);
    signal(SIGCHLD, sigchld_handler);   
}

/*
 * starting point of kernel, given pid of disk and the processes
 * 
 * @diskPID: pid of disk
 * @diskChannel: up/down streams to talk to disk
 * @procs: pairs of proc pid and its up/down channel
 * 
 * @return exit code of kernel
 */
int kernel_main(pid_t diskPID, Channel diskChannel, vector<pair<pid_t, Channel>> procs) {
    // set SIGCHLD handler of kernel
    signal(SIGCHLD, sigchld_handler);
    
    // initialize some variables
    long long int KERNEL_CLK = 0;
    chrono::time_point<chrono::system_clock> start = chrono::system_clock::now(); 
    string proc_msg;
    long proc_msg_type = 0;
    string disk_msg;
    long disk_msg_type = 0;

    // create/open log file
    ofstream event_log;
    event_log.open("logger/log.txt");
    
    // loop over all processes until no more left
    while (procs.size() != 0)
    {
        // get current process
        pair<pid_t, Channel> cur_proc = procs[procs.size()-1];
        procs.pop_back();

        // request handling
        if (cur_proc.second.recv(proc_msg, proc_msg_type)) // check whether current process have a request in message queue
        {
            // log print process request
            event_log << "#SYS_CALL:" << endl;
            event_log << endl;
            event_log << "process #" << cur_proc.first << " requested: " << proc_msg << endl;
            // get number of free slots in disk
            int free_slots = -1;
            kill(diskPID, SIGUSR1); // sigusr1 signal
            if (diskChannel.recv(disk_msg, disk_msg_type))
            {
                event_log << disk_msg << " slots available!" << endl;
                free_slots = stoi(disk_msg);
            }

            if (free_slots == -1) // in case of disk failure
            {
                event_log << "disk failure, no count returned!" << endl;
            }
            else if (proc_msg[0] == 'D' && free_slots == 10) // invalid delete request, no occupied slot
            {
                event_log << "error occured, no slots to delete!" << endl;
            }
            else if (proc_msg[0] == 'A' && free_slots == 0) // invalid add request, no free slots
            {
                event_log << "error occured, no free slots!" << endl;
            }
            else // send request to disk, if valid
            {
                event_log << "request sent to disk" << endl;
                if (proc_msg[0] == 'A') // add request
                {
                    diskChannel.send(proc_msg, 4);
                    event_log << "added successfully, process freed" << endl;
                }
                else if (proc_msg[0] == 'D') // delete request
                {
                    if (int(proc_msg[1]-'0') > 9) // slot number out of range
                    {
                        event_log << "invalid request, process freed" << endl;
                    }
                    else
                    {
                        diskChannel.send(proc_msg, 3);
                        event_log << "deleted successfully, process freed" << endl;
                    }               
                } 
                else // unknown request
                {
                    event_log << "invalid request, process freed" << endl;
                }
            }
        }

        // check if process still exists
        if (0 == kill(cur_proc.first, 0)) 
        {
            procs.insert(procs.begin(), cur_proc);
        }
        kill(diskPID, SIGUSR2);
        for (auto &proc: procs)
        {
            kill(proc.first, SIGUSR2);
        }
        sleep(1);
    }
    
    event_log.close();

    kill(diskPID, SIGKILL); // kill disk process

    return 0;
}