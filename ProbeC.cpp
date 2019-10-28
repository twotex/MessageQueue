#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>
#include <time.h>
#include <stdlib.h>
#include "kill_patch.h"

#include <thread>
#include <chrono>

using namespace std;

int main() {

	// declare my message buffer
	struct buf {
		long mtype; // required
		int randomInt; //random int that meets criteria
		bool keepGoing; //flag to indicate whether to keep exhanging info
		__pid_t pid;
	};

	/*
	struct myMsgBuf {
		long mtype;
		char mtext[1];
	};

	myMsgBuf theExitMessage;
	theExitMessage.mtype = 9000;
	theExitMessage.mtext[0] = 'y';
	int exitMessageSize = sizeof(theExitMessage) - sizeof(long);
	*/

	buf theExitMessage;
	buf msg;
	int size = sizeof(msg)-sizeof(long);
	theExitMessage.mtype = 900;
	theExitMessage.keepGoing = false;
	theExitMessage.pid = getpid();
	theExitMessage.randomInt = -5000;
	

	int qid = msgget(ftok(".",'u'), 0);
	kill_patch(qid, (msgbuf*)&theExitMessage, size, 900);

	srand(time(0));

	int randomValue;
	const int magic_seed = 251;
	msg.keepGoing = true;
	__pid_t pidProbeC = getpid();
	msg.pid = pidProbeC;
	int messegesSent = 0;

	while (true)
	{	
		randomValue = rand();

		while (randomValue % magic_seed != 0)
		{
			randomValue = rand();
		}

		msg.randomInt = randomValue;
		msg.mtype = 900;
		msgsnd(qid, (struct msgbuf *)&msg, size, 0); // sending
		cout << pidProbeC << "(Probe C) : sent int to DataHub: " << msg.randomInt << "   Messages Sent: " << messegesSent++ << endl;
		this_thread::sleep_for(std::chrono::milliseconds(10));
	}

    exit(0);
}
