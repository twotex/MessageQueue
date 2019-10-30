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

	int qid = msgget(ftok(".",'u'), 0); //Obtain message queue

	// declare my message buffer
	struct buf {
		long mtype; // required
		char probe; //represents the specific probe character
		int randomInt; //will hold random int generated that is divisible by magic seed
		bool keepGoing; //flag sent to DataHub to determine whether or not to send a message back to our Probe
		__pid_t pid; //holds process ID of probe that will be sending data
	};

	buf msg;
	int size = sizeof(msg)-sizeof(long);

	srand(time(0)); //Seed random number generator from the milliseconds elapsed since Jan 1st 1970

	int randomValue; //variable to hold the temporary random value generated
	const int magic_seed = 117397; //magic seed that will be used in a modulus operation with our random value
	msg.keepGoing = true; //Confimation flag within our message used by Probe A and DataHub
	__pid_t pidProbeB = getpid(); //this variable holds the Process ID of Probe B
	msg.pid = pidProbeB; //attaches this Probe's process ID to the message that will be sent
	msg.probe = 'b';  //set probe id to B
	int messegesSent = 0; //Number of messages sent to DataHub

	while (true) //Infinite loop terminated by force_patch, which executes when the DataHub has received 10,000 messages
	{	
		randomValue = rand(); //generate a random value

		while (randomValue % magic_seed != 0) //Loop until we generate a random value that is divisible by our magic seed
		{
			randomValue = rand();
		}

		msg.randomInt = randomValue; //attach our random value to the message being sent to the DataHub
		msg.mtype = 117; //use mtype 117 for messages between Probe B and DataHub
		msgsnd(qid, (struct msgbuf *)&msg, size, 0); //sends message to DataHub
		cout << pidProbeB << "(Probe B) : sent int to DataHub: " << msg.randomInt << "   Messages Sent: " << messegesSent++ << endl;
	}
	
    exit(0);
}
