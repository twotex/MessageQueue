#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>
#include <force_patch.h>

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
	bool forcePatchUsed = false;

	while(msgA.keepGoing == true)
	{
		msgrcv(qid, (struct msgbuf *)&msgA, size, 117, 0);
		messagesReceived++;
		pidProbeA = msgA.pid;

		if (msgA.keepGoing == true)
		{
			
			cout << pidDataHub << ": gets message from " << msgA.pid << endl;
			cout << "message: " << msgA.randomInt << endl;
			msgA.randomInt = -2;
			cout << pidDataHub << ": sends -2 as a confirmation receipt" << endl;
			msgA.mtype = 314;
			msgsnd(qid, (struct msgbuf *)&msgA, size, 0);
		}

		if (messagesReceived < 10000)
		{
			msgrcv(qid, (struct msgbuf *)&msgB, size, 255, 0);
			messagesReceived++;
			pidProbeB = msgB.pid;

			cout << pidDataHub << ": gets message from " << msgB.pid << endl;
			cout << "message: " << msgB.randomInt << endl;
		}

		else
		{
			if (forcePatchUsed == false)
			{
				force_patch(pidProbeB);
				forcePatchUsed = true;
				counter = messagesReceived;
			}
		}
	}

	cout << getpid() << ": has received its last message" << endl;
	cout << "final message: " << msgA.randomInt << endl;
	cout << "Total Message count: " << messagesReceived << endl;
	cout << endl;
	cout << "Force patch used when messages received was equal to " << counter << endl;
	
	msgctl (qid, IPC_RMID, NULL);
    

    exit(0);

}
