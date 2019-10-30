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
	int qid = msgget(ftok(".",'u'), 0); //Obtain message queue

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

	bool validReading = true;   //Flag that will remain true as long as a random number below 100 has not been generated
	int randomValue;   //variable to hold the temporary random value generated
	const int magic_seed = 359193;  //magic seed that will be used in a modulus operation with our random value
	msg.keepGoing = true;  //Set this to true initially and only set it to false when a random number below 100 is generated
	msg.randomInt = -2;    // the -2 value is the int that the DataHub sends over to indicate receipt of a message from Probe A
	__pid_t pidProbeA = getpid();  //this variable holds the Process ID of Probe A
	int messegesSent = 0;   //Number of messages sent to DataHub
	msg.pid = pidProbeA;    //attaches this Probe's process ID to the message that will be sent
	msg.probe = 'a'; //set probe id to A

	//Loops as long as we haven't generated a value lower than 100 and we have received a confirmation of receipt from the DataHub
	while (validReading == true && msg.randomInt == -2) 
	{
		randomValue = rand(); //generate a random value

		//continues to generate a random value until it is divisible by our magic seed. Stops looping if we generate a value less than 100
		while (randomValue % magic_seed != 0 && validReading == true) 
		{
			randomValue = rand(); //obtain random value

			if (randomValue < 100)
			{
				validReading = false; //Set flag to false so we no longer generate random values
			}
		}

		msg.randomInt = randomValue; //store our random value in our message object

		if (validReading == false)  //Executes when a random value less than 100 has been generated. Sends one final message to DataHub and termines Probe A afterwards
		{
			msg.keepGoing = false; //Let's datahub know to not send a message back
			msg.mtype = 117; 	//use mtype 117 for messages between Probe A and DataHub
			msgsnd(qid, (struct msgbuf *)&msg, size, 0); // Send last message to DataHub
			cout << pidProbeA << "(Probe A) : sent its final int to DataHub: " << msg.randomInt << "   Messages Sent: " << messegesSent++ << endl;
		}


		else //Executes when we've generated a random value that is greater than or equal to 100
		{
			msg.mtype = 117; 	//use mtype 117 for messages between Probe A and DataHub
			msgsnd(qid, (struct msgbuf *)&msg, size, 0); //sends message to DataHub
			cout << pidProbeA << "(Probe A) : sent int to DataHub: " << msg.randomInt << "   Messages Sent: " << messegesSent++ << endl;
			msgrcv(qid, (struct msgbuf *)&msg, size, 314, 0); //reads confirmation message from DataHub
			cout << pidProbeA << ": Confirmation from Data Hub received." << endl;
		}	
	}

    exit(0);
}
