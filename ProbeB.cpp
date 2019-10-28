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

#include <thread>
#include <chrono>

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

	int randomValue;
	const int magic_seed = 257;
	msg.keepGoing = true;
	__pid_t pidProbeB = getpid();
	msg.pid = pidProbeB;
	int messegesSent = 0;

	while (true)
	{	
		randomValue = rand();

		while (randomValue % magic_seed != 0)
		{
			randomValue = rand();
		}

		msg.randomInt = randomValue;
		msg.mtype = 600;
		msgsnd(qid, (struct msgbuf *)&msg, size, 0); // sending
		cout << pidProbeB << "(Probe B) : sent int to DataHub: " << msg.randomInt << "   Messages Sent: " << messegesSent++ << endl;
		this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	
    exit(0);
}
