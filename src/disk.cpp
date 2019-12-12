#include <signal.h> // pid_t
#include "Channel.hpp"
#include <iostream>
#include <string>  
#include <vector>
#include <unistd.h>
using namespace std;


//Global Variables
long long int CLK = 0;
int msg_count = 0;
const int MAX_Count = 10;
Channel ch;
vector<string> disk_messages;
//Definiton for SIGUSR1
void sigusr1_handler(int signum)
{
	//For more confirmation
	signal(SIGUSR1, sigusr1_handler);
	//Needs to be defined
	long msg_type = 7;
	//Sends the Kernel a MSG with your Count
	if (ch.send(to_string(msg_count), msg_type) == -1)
		cout << "Error in upstreaming the number of messages in Disk!" << endl;
	return;
}

//Definiton for SIGUSR2
void sigusr2_handler(int signum)
{
	//For more confirmation
	signal(SIGUSR2, sigusr2_handler);
	CLK++;
	return;
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
	// TODO
	ch = kernelChannel;
	signal(SIGUSR2, sigusr2_handler);
	signal(SIGUSR1, sigusr1_handler);
	for (int i = 0; i < 10; i++)
		disk_messages[i] = "empty";
	
	bool termination = false;
	string message;
	long msg_type;
	//0 for del, 1 for add
	int option = 0;
	int time = 0;
	while (!termination)
	{
		//In each loop, I'll check the Msg Q for new msgs
		while (ch.recv(message, msg_type))
		{
			//Decide your option based on message type
			option = 1;


			if (option == 1)
			{	
				if (msg_count == MAX_Count)
				{
					cout << "Problem occured while adding the message, No space allowed" << endl;
				}
				time = 3e6;
				for (int i = 0; i < time; i++)
					sleep(1);
				int k = 0;
				while (disk_messages[k] != "empty" && k<= MAX_Count)
					k++;
				k++;
				//Added on the first empty slot
				disk_messages[k] = message;
				msg_count++;
			}
			else 
			{
				//Delete operation
				time = 1e6;
				for (int i = 0; i < time; i++)
					sleep(1);
				disk_messages[stoi(message)] = "empty";
				msg_count--;
			}

		}
	}

	//Disk exit code
	return 0;
}

