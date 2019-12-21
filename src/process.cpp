#include <signal.h> // pid_t
#include <fstream>
#include <vector>
#include <iostream>
#include "Channel.hpp"
#include <unistd.h>
using namespace std;


/*
 * starting point of kernel
 *
 * @kernelPID: pid of kernel
 * @kernelChannel: up/down streams to talk to kernel
 * @inputFilePath: path to input file (you have to check first if file exists)
 *
 * @return exit code of the process
 */

long long int CLK = 0;

//Definiton for SIGUSR2
void sigusr2_handler(int signum)
{
	//For more confirmation
	signal(SIGUSR2, sigusr2_handler);
	CLK++;
}


int process_main(pid_t kernelPID, int processNum, Channel kernelChannel, string inputFilePath) {
   //read data from the input file and save it in data vector
   ifstream file(inputFilePath);

   if(!file.is_open()) {
      cout << "file not exists please make sure from the file name/path" <<endl;
      return -1;        //error in process creation
   }
   vector<string> data;
   string line;
   while ( getline (file,line) ) {
      data.push_back(line);
   }
   file.close();

   // sort processes based on time ! --> To be done
   //set the siguser2 handler
   signal(SIGUSR2, sigusr2_handler);

   string message;
   //0 for del, 1 for add
	long msg_type;
   while (!data.empty()) {
      if(int(data[0][0]) == CLK) {
         if(data[0][2] == 'D') {
            msg_type = 0;
         } else if (data[0][2] == 'A') {
            msg_type = 1;
         } else {
            cout << " invalid action " << endl;
            return -1;       //error in process creation
         }
         message = data[0].substr(4);
         kernelChannel.send(message , msg_type);
         data.pop_back();

      }

   }
   //when process finishes it sends its exit code
   return 0;
}