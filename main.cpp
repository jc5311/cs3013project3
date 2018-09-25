//Team: Juan Caraballo

//The purpose of this project is to exercise the use of semaphores and mailboxes
//to build a message passing system that ca be used among a set of threads
//within the same process.


//includes
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
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
static msg mailboxes[MAXTHREAD+1]; //"message = mailbox"; mailboxes
static sem_t sem_addr[MAXTHREAD+1]; //semaphore addresses for each mailbox
static pthread_t tids[MAXTHREAD + 1]; //thread tids
int testval = 1;


//thread routine
void *pt_routine(void *arg){
	return NULL;
}


//main
int main (int argc, char* argv[]){
	/*
	//testing the use of threads
	cout << "We are here 1." << endl;
	retval = pthread_create(tid, NULL, pt_routine, (void*)&testval);
	cout << "pthread_create returned: " << retval << endl;
	sleep(3);
	cout << "The new value for testval is: " << testval << endl;
	*/

	//collect num arg from command line
	if (argc > 1){
		int numarg = atoi(argv[1]);
		cout << numarg << endl;
	}

	//create 10 semaphores
	for (int i = 1; i <= MAXTHREAD; i++){
		cout << "Creating sem " << i << endl;
		int retval = sem_init(&sem_addr[i], 0, 1);
		cout << "Attempt complete." << endl;
		if (retval < 0){
			cerr << "sem_init error";
		}
	}

	//create 10 threads and call the pt_thread routine on each
	for (int i = 1; i <= MAXTHREAD; i++){
		cout << "Creating thread " << i << endl;
		int retval = pthread_create(&tids[i], NULL, pt_routine, &i);
		if (retval != 0){
			cerr << "pthread_create error";
		}
		cout << "Attempt complete." << endl;
	}

	return 0;
} //end of main