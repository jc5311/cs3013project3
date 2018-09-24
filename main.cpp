//Team: Juan Caraballo

//The purpose of this project is to exercise the use of semaphores and mailboxes
//to build a message passing system that ca be used among a set of threads
//within the same process.


//includes
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

//defines
#define MAXTHREAD 10
#define RANGE 1
#define ALLDONE 2

//structs and classes
class msg{
public:
	int iSender; //sender of the message (0 .. numThreads)
	int type; //message type (RANGE OR ALLDONE)
	int value1; //first message value
	int value2; //second message value

	msg(){
		//this constructor was just created for practicing the use of one
		iSender = 0;
		type = 0;
		value1 = 0;
		value2 = 0;
	}

	int SendMsg(){
		return 1;
	}

	int RecvMsg(){
		return 2;
	}
};

//global vars
msg mailboxes[MAXTHREAD+1]; //"message = mailbox"

int main (int argc, char* argv[]){
	//lets just test the use of our mailbox and stuff really quick
	cout << "Mailbox 0 iSender value is: " << mailboxes[0].iSender << endl;
	cout << "Calling Mailbox 0's SendMsg method: " << mailboxes[0].SendMsg() << endl;


	return 0;
} //end of main