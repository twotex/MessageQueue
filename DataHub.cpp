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
	bool terminatedProbeC = false;

	while(keepReceivingFromA || messagesReceived < 10000 || terminatedProbeC == false)
	{
		
		if (keepReceivingFromA == true)
		{
			msgrcv(qid, (struct msgbuf *)&msgA, size, 117, 0);
			messagesReceived++;
			pidProbeA = msgA.pid;
			keepReceivingFromA = msgA.keepGoing;

			if (keepReceivingFromA == true)
			{
				cout << pidDataHub << "(DataHub): gets message from " << msgA.pid << "(Probe A) ----->  " << msgA.randomInt << endl;
				msgA.randomInt = -2;
				msgA.mtype = 314;
				msgsnd(qid, (struct msgbuf *)&msgA, size, 0);
			}

			else
			{
				cout << pidDataHub << "(DataHub): gets final message from " << msgA.pid << "(Probe A) ----->  " << msgA.randomInt << endl;
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

		if (terminatedProbeC != true)
		{
			msgrcv(qid, (struct msgbuf *)&msgC, size, 900, 0);
			messagesReceived++;
			pidProbeC = msgC.pid;

			if (msgC.randomInt != -5000)
			{
				cout << pidDataHub << "(DataHub): gets message from " << msgC.pid << "(Probe C) ----->  " << msgC.randomInt << endl;
			}

			else
			{
				cout << "Probe C was just terminated!" << endl;
				terminatedProbeC = true;
			}
		}

		cout << "Messages Received thus far: " << messagesReceived << endl;
	}

	cout << endl;
	cout << getpid() << "(DataHub) has received its last message from the queue" << endl;
	
	msgctl (qid, IPC_RMID, NULL);
    cout << "Queue has now been deallocated. " << endl;

    exit(0);

}
