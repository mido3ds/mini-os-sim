#include <signal.h> // pid_t
#include "Channel.hpp"
#include <iostream>
#include <string>  
#include <vector>
#include <unistd.h>
using namespace std;


//Global Variables
long long int DISK_CLK = 0;
int msg_count = 0;
const int MAX_Count = 10;
Channel *ch;
vector<string> disk_messages;

/* Definiton for Disk SIGUSR1 */
void sigusr1_disk_handler(int signum)
{
	long msg_type = 5; // message type 4 for Disk->Kernel 
	// send the Kernel a message with disk Count
	if (ch->send(to_string(10-msg_count), msg_type) == -1)
		cout << "Error in upstreaming the number of messages in Disk!" << endl;
	signal(SIGUSR1, sigusr1_disk_handler);
}

/* Definiton for Disk SIGUSR2 */
void sigusr2_disk_handler(int signum)
{
	DISK_CLK++;
	signal(SIGUSR2, sigusr2_disk_handler);
}

/*
* starting point of disk process
*
* @kernelPID: pid of kernel
* @kernelChannel: up/down streams to talk to kernel
*
* @return exit code of disk
*/
int disk_main(pid_t kernelPID, Channel kernelChannel) {
	// some definitions
	ch = &kernelChannel;
	signal(SIGUSR2, sigusr2_disk_handler);
	signal(SIGUSR1, sigusr1_disk_handler);
	// initialize disk slots
	for (int i = 0; i < 10; i++)
		disk_messages[i] = "empty";
	string message;
	long msg_type = 0;
	// loop to get all requests by kernel
	while (true)
	{
		// check the queue for new messages
		if (ch->recv(message, msg_type))
		{
			// get message content
			message = message.substr(2);
			// decide option based on message type
			if (msg_type == 4)
			{	
				// add operation
				for (int i = 0; i < 3e6; i++)
					usleep(1);
				int k = 0;
				while (disk_messages[k] != "empty" && k<= MAX_Count)
					k++;
				k++;
				// add on the first empty slot
				disk_messages[k] = message;
				msg_count++;
			}
			else 
			{
				// delete operation
				for (int i = 0; i < 1e6; i++)
					usleep(1);
				if (disk_messages[stoi(message)] != "empty")
				{
					disk_messages[stoi(message)] = "empty";
					msg_count--;
				}
			}
		}
	}

	//Disk exit code
	return 0;
}

