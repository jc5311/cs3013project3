//Team: Juan Caraballo

//The purpose of this project is to exercise the use of semaphores and mailboxes
//to build a message passing system that ca be used among a set of threads
//within the same process.


//includes
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

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
static msg mailboxes[MAXTHREAD+1]; //"message = mailbox"
int *testval;


//thread routine
void *pt_routine(void *arg){
	*(int*)arg += 5;
	return NULL;
}


//main
int main (int argc, char* argv[]){
	//testing the use of threads
	int retval = 10;
	int testval = 4;
	pthread_t *tid; //testing
	cout << "We are here 1." << endl;
	retval = pthread_create(tid, NULL, pt_routine, (void*)&testval);
	cout << "pthread_create returned: " << retval << endl;
	sleep(3);
	cout << "The new value for testval is: " << testval << endl;
	
	return 0;
} //end of main