#include <signal.h> // pid_t
#include <fstream>
#include <vector>
#include <iostream>
#include "Channel.hpp"
#include <unistd.h>
using namespace std;

#include <iostream>

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
    // get current process
    pair<pid_t, Channel> cur_proc = procs[procs.size()-1];
    procs.pop_back();
    // initialize some variables
    string proc_msg;
    long proc_msg_type;
    string disk_msg;
    long disk_msg_type;

    // create/open log file
    ofstream event_log("logger/log.txt");
    if (event_log)
    {
        event_log.open("logger/log.txt", std::ios_base::app);
    }
    else
    {
        event_log.open("logger/log.txt");
    }
    
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
                diskChannel.send(proc_msg, 3);
                for (int i=0; i<3; i++)
                {
                    sleep(1);
                    kill(diskPID, SIGUSR2);
                    kill(cur_proc.first, SIGUSR2);
                    for (auto& proc: procs)
                    {
                        kill(proc.first, SIGUSR2);
                    }
                }
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
                    diskChannel.send(proc_msg, 2);
                    sleep(1);
                    kill(diskPID, SIGUSR2);
                    kill(cur_proc.first, SIGUSR2);
                    for (auto& proc: procs)
                    {
                        kill(proc.first, SIGUSR2);
                    }
                    event_log << "deleted successfully, process freed" << endl;
                }               
            } 
            else // unknown request
            {
                event_log << "invalid request, process freed" << endl;
            }
        }
    }

    if (0 == kill(cur_proc.first, 0)) // check if process still exists
    {
        procs.insert(procs.begin(), cur_proc);
    }
    event_log << endl;
    event_log << endl;
    event_log.close();

    if (procs.size() != 0) // check if no more processes, kill disk and exit
    {
        kernel_main(diskPID, diskChannel, procs);
    }

    kill(diskPID, SIGKILL);
    return 0;
}