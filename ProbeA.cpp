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
	};

	buf msg;
	int size = sizeof(msg)-sizeof(long);

	srand(time(0));

	bool validReading = true;
	int randomValue;
	const int magic_seed = 997;

	while (validReading == true)
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

		if (validReading == false)
		{
			msg.keepGoing = false;
			msg.mtype = 117; 	// set message type mtype = 117
			msgsnd(qid, (struct msgbuf *)&msg, size, 0); // sending
		}

		else
		{
			msg.randomInt = randomValue;
			msg.keepGoing = true;
			cout << getpid() << ": sends int" << endl;
			msg.mtype = 117; 	// set message type mtype = 117
			msgsnd(qid, (struct msgbuf *)&msg, size, 0); // sending

			msgrcv(qid, (struct msgbuf *)&msg, size, 314, 0); // reading
			cout << getpid() << ": gets confirmation int" << endl;
			cout << "confirmation int: " << msg.randomInt << endl;
		}
		
	}

    exit(0);

}