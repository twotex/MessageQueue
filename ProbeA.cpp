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

	// declare my message buffer
	struct buf {
		long mtype; // required
		char greeting[50]; // mesg content
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
			//Terminate
		}

		else
		{
			//Send data to hub
		}
		
	}

    exit(0);
}