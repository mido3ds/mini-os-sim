#include <vector>
#include <iostream>
#include <sstream>
#include <algorithm> // reverse
#include <signal.h> // pid_t
#include <unistd.h> // fork, getpid, getppid

#include "Channel.hpp"

using namespace std;

// colors for terminal output
#define RED_CLR "\033[1;31m"
#define RESET_CLR "\033[0m"

// declerations of processes starting points.
// Their definitions are in their corresponding source files (kernel.cpp, disk.cpp, process.cpp)

int kernel_main(pid_t diskPID, Channel diskChannel, vector<pair<pid_t, Channel>> procs);
int disk_main(pid_t kernelPID, Channel kernelChannel);
int process_main(pid_t kernelPID, int processNum, Channel kernelChannel, string inputFilePath);

string getInputTextPath(string inputDir, int pNum) {
    ostringstream os;
    os << inputDir << "/" << pNum << ".txt";
    return os.str();
}

/*
 * create n process, disk process and kernel
 * prints and returns their exit codes
 */
int init(int n, string inputDir) {
    static vector<pair<pid_t, Channel>> procs;

    if (n > 0) {
        // init process #n

        auto chnl = Channel::open();

        pid_t pid;
        if ((pid = fork()) == -1) {
            perror("Can't fork process!");
            for (auto ch: procs) { kill(ch.first, SIGKILL); }
            return 1;
        } 

        if (pid == 0) {
            int exitCode = process_main(getppid(), n-1, chnl, getInputTextPath(inputDir, n-1));
            chnl.close();

            //cerr << RED_CLR ">> " RESET_CLR << "Process #" << n-1 << " returned " << exitCode << endl;
            return exitCode;
        }
        
        procs.push_back(make_pair(pid, chnl.flip()));
        return init(n-1, inputDir);
    } else {
        // init disk and kernel
 
        auto chnl = Channel::open();
        
        pid_t pid;
        if ((pid = fork()) == -1) {
            perror("Can't fork disk process!");
            for (auto ch: procs) { kill(ch.first, SIGKILL); }
            return 1;
        }

        if (pid == 0) {
            int exitCode = disk_main(getppid(), chnl);
            chnl.close();

            //cerr << RED_CLR ">> " RESET_CLR << "Disk returned " << exitCode << endl;
            return exitCode;
        }

        reverse(procs.begin(), procs.end());
        int exitCode = kernel_main(pid, chnl.flip(), procs);

        // close channels
        chnl.close();
        for (auto& proc: procs) {
            proc.second.close();
        }

        //cerr << RED_CLR ">> " RESET_CLR << "Kernel returned " << exitCode << endl; 
        return exitCode;
    }

    // dummy return
    return 0;
}

int main(int argc, char const *argv[]) {
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " numOfProcesses inputFilesDir\n";
        return 1;
    }

    // parse args
    int n;

    try {
        n = stoi(argv[1]);
    } catch (invalid_argument _) {
        cerr << "n must be a positive number\n";
        return 1;
    }

    if (n <= 0) {
        cerr << "n must be > 0\n";
        return 1;
    }

    return init(n, argv[2]);
}