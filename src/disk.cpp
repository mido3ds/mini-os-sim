#include <signal.h> // pid_t

#include "Channel.hpp"

#include <iostream>
using namespace std;

/*
 * starting point of disk process
 * @kernelPID: pid of kernel
 * @kernelChannel: up/down streams to talk to kernel
 * 
 * @return exit code of disk
 */
int disk_main(pid_t kernelPID, Channel kernelChannel) {
    return 0;
}