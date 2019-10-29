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

	struct buf {
		long mtype; // required
		int randomInt; //will hold random int generated that is divisible by magic seed
		bool keepGoing; //flag sent to DataHub to determine whether or not to send a message back to our Probe
		__pid_t pid; //holds process ID of probe that will be sending data
	};

	buf msgA; //Buf that will hold messages from Probe A
	buf msgB; //Buf that will hold messages from Probe B
	buf msgC; //Buf that will hold messages from Probe C
	int size = sizeof(msgA)-sizeof(long);
	msgA.keepGoing = true; //Flag from message A that will indicate whether or not to send a confrimation message back to it
	__pid_t pidDataHub = getpid(); //Obtain the process ID for our DataHub
	__pid_t pidProbeA; //Variable that will hold process ID for Probe A
	__pid_t pidProbeB; //Variable that will hold process ID for Probe B
	__pid_t pidProbeC; //Variable that will hold process ID for Probe C
	int counter; //variable that holds our message count when force_patch was used on Probe B
	bool keepReceivingFromA = true; //Flag to indicate if we should keep send a message back to Probe A
	bool forcePatchUsed = false; //Flag that indicates if we have used our force patch on Probe B
	bool terminatedProbeC = false; //Flag that indicates if we whether or not we have received the exit message from Probe C

	while(keepReceivingFromA || messagesReceived < 10000 || terminatedProbeC == false) //Keeps looping as long as one of the Probes is still sending messages
	{
		
		if (keepReceivingFromA == true) //executes if we still have communication with Probe A
		{
			msgrcv(qid, (struct msgbuf *)&msgA, size, 117, 0); //receive message from Probe A
			messagesReceived++; //Add one to our total messages received
			pidProbeA = msgA.pid; //Obtain the process ID from Probe A
			keepReceivingFromA = msgA.keepGoing; //Updates our local variable that determines whether or not communication with Probe A should continue

			if (keepReceivingFromA == true) //Executes if we should send a confirmation message back to Probe A
			{
				cout << pidDataHub << "(DataHub): gets message from " << msgA.pid << "(Probe A) ----->  " << msgA.randomInt << endl;
				msgA.randomInt = -2; //Uses -2 as a flag to indicate that DataHub has received Probe A's message
				msgA.mtype = 314; //use mtype 314 for messages between sent back to Probe A
				msgsnd(qid, (struct msgbuf *)&msgA, size, 0); //Send message to Probe A
			}

			//Executes when we have received our final message from Probe A and we don't need to send a confirmation message back to it
			else
			{
				cout << pidDataHub << "(DataHub): gets final message from " << msgA.pid << "(Probe A) ----->  " << msgA.randomInt << endl;
			}
			
		}

		//Keep receiving messages from Probe B as long as our message count is below 10,000
		if (messagesReceived < 10000)
		{
			msgrcv(qid, (struct msgbuf *)&msgB, size, 600, 0); //Receives message from Probe B
			messagesReceived++; //Add one to our total messages received
			pidProbeB = msgB.pid; //Obtain the process ID from Probe B

			cout << pidDataHub << "(DataHub): gets message from " << msgB.pid << "(Probe B) ----->  " << msgB.randomInt << endl;
		}

		//Messages received by DataHub has passed 10,000. Executes force patch to terminate Probe B
		else
		{
			//If we haven't used our force patch yet, use it now
			if (forcePatchUsed == false)
			{
				force_patch(pidProbeB); //uses force patch
				forcePatchUsed = true; //sets our force patch used flag to true
				counter = messagesReceived; //updates counter to let it know at what message count our force patch was used. Should be at 10,000
				cout << "Force Patch Used on Probe B!" << endl;
			}
		}

		if (terminatedProbeC != true) //If Probe C has not been terminated yet
		{
			msgrcv(qid, (struct msgbuf *)&msgC, size, 900, 0); //receive message from Probe C
			messagesReceived++; //Add one to our total messages received
			pidProbeC = msgC.pid; //Obtain the process ID from Probe C

			if (msgC.randomInt != -5000) //If we haven't received the exit message from Probe C
			{
				cout << pidDataHub << "(DataHub): gets message from " << msgC.pid << "(Probe C) ----->  " << msgC.randomInt << endl;
			}

			else //Exit message was received from Probe C
			{
				cout << "Probe C was just terminated!" << endl;
				terminatedProbeC = true;
			}
		}

		cout << "Messages Received thus far: " << messagesReceived << endl;
	}

	cout << endl;
	cout << getpid() << "(DataHub) has received its last message from the queue" << endl;
	
	msgctl (qid, IPC_RMID, NULL); //Deallocates Queue
    cout << "Queue has now been deallocated. " << endl;

    exit(0);

}
