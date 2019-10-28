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

using namespace std;

int main() 
{
	int qid = msgget(ftok(".",'u'), 0);

	// declare my message buffer
	struct buf {
		long mtype; // required
		int randomInt; //random int that meets criteria
		bool keepGoing; //flag to indicate whether to keep exhanging info
		__pid_t pid;
	};

	buf msg;
	int size = sizeof(msg)-sizeof(long);

	srand(time(0));

	bool validReading = true;
	int randomValue;
	const int magic_seed = 997;
	msg.keepGoing = true;
	msg.randomInt = -2;
	__pid_t pidProbeA = getpid();
	int messegesSent = 0;
	msg.pid = pidProbeA;

	while (validReading == true && msg.randomInt == -2)
	{
		randomValue = rand();

		while (randomValue % magic_seed != 0 && validReading == true)
		{
			randomValue = rand();

			if (randomValue < 100)
			{
				validReading = false;
			}
		}

		msg.randomInt = randomValue;

		if (validReading == false)
		{
			msg.keepGoing = false;
			msg.mtype = 117; 	// set message type mtype = 117
			msgsnd(qid, (struct msgbuf *)&msg, size, 0); // last message sent to data hub
			cout << pidProbeA << "(Probe A) : sent its final int to DataHub: " << msg.randomInt << "   Messages Sent: " << messegesSent++ << endl;
		}

		else
		{
			msg.mtype = 117; 	// set message type mtype = 117
			msgsnd(qid, (struct msgbuf *)&msg, size, 0); // sending
			cout << pidProbeA << "(Probe A) : sent int to DataHub: " << msg.randomInt << "   Messages Sent: " << messegesSent++ << endl;
			msgrcv(qid, (struct msgbuf *)&msg, size, 314, 0); // reading
			cout << pidProbeA << ": Confirmation from Data Hub received." << endl;
		}
		
	}

    exit(0);
}