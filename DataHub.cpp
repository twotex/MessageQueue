#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>
//#include <force_patch.h>

using namespace std;
int main() {

	int messagesReceived = 0;

	// create my msgQ with key value from ftok()
	int qid = msgget(ftok(".",'u'), IPC_EXCL|IPC_CREAT|0600);

	// declare my message buffer
	struct buf {
		long mtype; // required
		int randomInt; //random int that meets criteria
		bool keepGoing; //flag to indicate whether to keep exhanging info
	};

	buf msg;
	int size = sizeof(msg)-sizeof(long);

	msg.keepGoing = true;

	while(msg.keepGoing == true)
	{
		msgrcv(qid, (struct msgbuf *)&msg, size, 117, 0);
		cout << getpid() << ": gets message" << endl;
		cout << "message: " << msg.randomInt << endl;

		msg.randomInt = -2;

		if (msg.keepGoing == true)
		{
			cout << getpid() << ": sends -2 as a confirmation receipt" << endl;
			msg.mtype = 314; // only reading mesg with type mtype = 314
			msgsnd(qid, (struct msgbuf *)&msg, size, 0);
		}
	}

	cout << getpid() << ": gets last message" << endl;
	cout << "final message: " << msg.randomInt << endl;
	
	msgctl (qid, IPC_RMID, NULL);
    

    exit(0);

}
