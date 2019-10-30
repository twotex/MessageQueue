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

using namespace std;

int main() {

	struct buf {
		long mtype; // required
		char probe; //represents the specific probe character
		int randomInt; //will hold random int generated that is divisible by magic seed
		bool keepGoing; //flag sent to DataHub to determine whether or not to send a message back to our Probe
		__pid_t pid; //holds process ID of probe that will be sending data
	};


	buf theExitMessage; //buf that contains our exit message for the DataHub
	buf msg; //buf that contains our normal communication with the DataHub
	int size = sizeof(msg)-sizeof(long);


	theExitMessage.mtype = 117; //use mtype 900 for our exit message
	theExitMessage.keepGoing = false; //This flag is used by ProbeA and is not used here. Just storing false in it so it has a value.
	theExitMessage.pid = getpid();  //Attach our Process ID to the exit message sent to DataHub
	theExitMessage.randomInt = -5000; //Flag sent to DataHub to let it know we are no longer going to send it messages
	theExitMessage.probe = 'c';

	int qid = msgget(ftok(".",'u'), 0); //Obtain message queue
	kill_patch(qid, (msgbuf*)&theExitMessage, size, 117); //Execute kill patch immediately so it can be called later when Probe C needs to be terminated

	srand(time(0)); //Seed random number generator from the milliseconds elapsed since Jan 1st 1970

	int randomValue; //variable to hold the temporary random value generated
	const int magic_seed = 112985; //magic seed that will be used in a modulus operation with our random value
	msg.keepGoing = true; //Confimation flag within our message used by Probe A and DataHub
	__pid_t pidProbeC = getpid(); //this variable holds the Process ID of Probe B
	msg.pid = pidProbeC; //attaches this Probe's process ID to the message that will be sent
	int messegesSent = 0; //Number of messages sent to DataHub
	msg.probe = 'c';

	while (true) //Infinite loop that executes until we use our kill patch in the terminal
	{	
		randomValue = rand(); //generate a random value

		while (randomValue % magic_seed != 0) //Loop until we generate a random value that is divisible by our magic seed
		{
			randomValue = rand();
		}

		msg.randomInt = randomValue; //attach our random value to the message being sent to the DataHub
		msg.mtype = 117; //use mtype 900 for messages between Probe C and DataHub
		msgsnd(qid, (struct msgbuf *)&msg, size, 0); //sends message to DataHub
		cout << pidProbeC << "(Probe C) : sent int to DataHub: " << msg.randomInt << "   Messages Sent: " << messegesSent++ << endl;
	}

    exit(0);
}
