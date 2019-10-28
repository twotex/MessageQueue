#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>
#include "force_patch.h"

using namespace std;

int main() {

	int messagesReceived = 0;

	// create my msgQ with key value from ftok()
	int qid = msgget(ftok(".",'u'), IPC_EXCL|IPC_CREAT|0600);

	// declare my message buffer
	struct buf {
		long mtype; // required
		int randomInt; //random int that meets criteria
		bool keepGoing; //flag to indicate whether to keep exhanging info
		__pid_t pid;
	};

	buf msgA;
	buf msgB;
	buf msgC;
	int size = sizeof(msgA)-sizeof(long);
	msgA.keepGoing = true;
	__pid_t pidDataHub = getpid();
	__pid_t pidProbeA;
	__pid_t pidProbeB;
	__pid_t pidProbeC;
	int counter;
	bool keepReceivingFromA = true;
	bool forcePatchUsed = false;
	

	while(keepReceivingFromA || messagesReceived < 10000)
	{
		
		if (keepReceivingFromA == true)
		{
			msgrcv(qid, (struct msgbuf *)&msgA, size, 117, 0);
			messagesReceived++;
			pidProbeA = msgA.pid;
			keepReceivingFromA = msgA.keepGoing;
			cout << "beep" << endl;

			if (keepReceivingFromA == true)
			{
				cout << pidDataHub << "(DataHub): gets message from " << msgA.pid << "(Probe A) ----->  " << msgA.randomInt << endl;
				msgA.randomInt = -2;
				//cout << pidDataHub << "(DataHub): sends -2 as a confirmation receipt to Probe A" << endl;
				msgA.mtype = 314;
				msgsnd(qid, (struct msgbuf *)&msgA, size, 0);
			}
		}

		if (messagesReceived < 10000)
		{
			msgrcv(qid, (struct msgbuf *)&msgB, size, 600, 0);
			messagesReceived++;
			pidProbeB = msgB.pid;

			cout << pidDataHub << "(DataHub): gets message from " << msgB.pid << "(Probe B) ----->  " << msgB.randomInt << endl;
		}

		else
		{
			if (forcePatchUsed == false)
			{
				force_patch(pidProbeB);
				forcePatchUsed = true;
				counter = messagesReceived;
				cout << "Force Patch Used on Probe B!" << endl;
			}
		}
		cout << "Messages Received thus far: " << messagesReceived << endl;
	}

	cout << getpid() << "(DataHub) has received its last message" << endl;
	cout << "Final message: " << msgA.randomInt << endl;
	cout << "Total Message count: " << messagesReceived << endl;
	cout << endl;
	cout << "Force patch used when messages received was equal to " << counter << endl;
	
	msgctl (qid, IPC_RMID, NULL);
    

    exit(0);

}
