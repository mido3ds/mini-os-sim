#include <signal.h> // pid_t
#include <fstream>
#include <vector>
#include <unordered_map>
#include <iostream>
#include "Channel.hpp"
#include <unistd.h>
using namespace std;

// Global Variables
long long int PRC_CLK = 0;

/* Definiton for Disk SIGUSR2 */
void sigusr2_prc_handler(int signum)
{
	PRC_CLK++;
   signal(SIGUSR2, sigusr2_prc_handler);
}

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
   // read data from the input file and save it in data vector
   ifstream file(inputFilePath);

   if(!file.is_open()) {
      cout << "file not exists please make sure from the file name/path" <<endl;
      return -1;        // error in process creation
   }

   vector<string> data;
   string line;
   while (getline (file, line)) {
      data.push_back(line);
   }
   file.close();

   // process requests text
   unordered_map<int,string> requests;
   for (int i=0; i<data.size(); i++) {
      size_t idx = data[i].find(" ");
      int time = stoi(data[i].substr(0, idx));
      string req = data[i].substr(idx+1);
      if (req[0] == 'A') {
         idx = req.find(" ");
         string prc_req = "A ";
         prc_req.append(req.substr(idx+1));
         requests[time] = prc_req;
      }
      else {
         idx = req.find(" ");
         string prc_req = "D ";
         prc_req.append(req.substr(idx+1));
         requests[time] = prc_req;
      }
   }

   // set the siguser2 handler
   signal(SIGUSR2, sigusr2_prc_handler);

   string message;
	long msg_type;

   // loop over all requests and send them with no wait
   while (!requests.empty()) { 
      if(requests.find(PRC_CLK) != requests.end()) { // found a request to be sent at the current clock cycle
         if(requests[PRC_CLK][0] == 'D') { // delete operation
            msg_type = 0;
         } else if (requests[PRC_CLK][2] == 'A') { // add operation
            msg_type = 1;
         } else {
            cout << " invalid action " << endl;
            return -1;       // error in process creation
         }
         message = requests[PRC_CLK];
         kernelChannel.send(message , msg_type); // send message
         requests.erase(PRC_CLK); // delete from requests
      }
   }

   // when process finishes it sends its exit code
   return 0;
}