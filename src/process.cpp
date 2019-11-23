#include <signal.h> // pid_t

#include "Channel.hpp"

/*
 * starting point of kernel
 * 
 * @kernelPID: pid of kernel
 * @kernelChannel: up/down streams to talk to kernel
 * @inputFilePath: path to input file (you have to check first if file exists)
 * 
 * @return exit code of the process
 */
int process_main(pid_t kernelPID, int processNum, Channel kernelChannel, string inputFilePath) {
    // TODO
    return 0;
}