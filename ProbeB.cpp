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

int main() {

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
	const int magic_seed = 257;
	msg.keepGoing = true;
	__pid_t pidProbeB = getpid();
	msg.pid = pidProbeB;

	while (true)
	{	
		randomValue = rand();

		while (randomValue % magic_seed != 0)
		{
			randomValue = rand();
		}

		msg.randomInt = randomValue;
		cout << pidProbeB << ": sends int" << msg.randomInt << endl;
		msg.mtype = 255;
		msgsnd(qid, (struct msgbuf *)&msg, size, 0); // sending
	}
	
    exit(0);
}
