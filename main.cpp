#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define MAXTHREAD 10
#define RANGE 1
#define ALLDONE 2

using namespace std;

//struct
//structs
typedef struct{

	int iSender; //sender of message
	int type; //message type
	int value1; //first value
	int value2; //second vlaue
} msg;

//global vars
pthread_t tids[MAXTHREAD+1];
sem_t rcvsem[MAXTHREAD+1];
sem_t sendsem[MAXTHREAD+1];

msg mailboxes[MAXTHREAD+1];
int mailbox_ids[MAXTHREAD+1] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
int num_threads = 0;
int sum_to = 1;


//functions
int SendMsg(int iTo, msg *pMsg){
	
	//check if the receive semaphore is available
	//meaning the mailbox is empty
	//mutex the action of sending
	sem_wait(&sendsem[iTo]);
	mailboxes[iTo] = *pMsg;
	sem_post(&rcvsem[iTo]);
	return 0;
}

int RecvMsg(int iFrom, msg *pMsg){
	//check if send semaphore is available
	//meaning mailbox is full
	sem_wait(&rcvsem[iFrom]);
	//mutex the action of receiving
	*pMsg = mailboxes[iFrom];
	sem_post(&sendsem[iFrom]);
	return 0;
}

//thread routines
void *wthreadRoutine(void *arg){
	int m_id = *(int*)arg; //lets pass in the mailbox id as an argument
	msg message;
	
	RecvMsg(m_id, &message);
	int sum = 0;
	for (int i = message.value1; i <= message.value2; i++){
		sum += i;
	}
	message.iSender = m_id;
	message.type = ALLDONE;
	message.value1 = sum;
	message.value2 = sum;
	//send ALLDONE message back to parent
	SendMsg(0, &message);
	
	return 0;
}

void *pthreadRoutine(void *arg){
	//send messages to all thread mailboxes
	int sum = 0;
	int jincrementer;
	for (int i = 1; i <= num_threads; i++){
		msg message;
		message.iSender = 0;
		message.type = RANGE;
		message.value1 = 1;
		message.value2 = sum_to;
		SendMsg(i, &message);
	}

	//wait for messages from everybody
	for (int i = 1; i <= num_threads; i++){
		msg message;
		RecvMsg(0, &message);
		if (message.type == ALLDONE){
			sum += message.value1;
		}
	}

	cout << "The total for 1 to " << sum_to << " using " << num_threads << 
			" threads is " << sum << endl;
	return 0;
}

int main(int argc, char *argv[]){
	//are there two args?
	//if so, collect their values and properly assign the num_threads we want
	//and the max number for threads to sum up to
	if (argc == 3){
		int nt = atoi(argv[1]);
		sum_to = atoi(argv[2]);
		if (nt <= 0){
			cout << "Error, please enter 1-10 threads as the first argument."
			<< endl;
		}
		else if ((nt > 0) && (nt < MAXTHREAD)){
			//if nt is between 0 and MAXTHREAD 
			num_threads = nt;
		}
		else{
			//if nt is >= MAXTHREAD
			num_threads = MAXTHREAD;
		}
	}
	else{
		//if two args aren't given, just break the program for now.
		return 1;
	}

	//create send semaphores
	for (int i = 0; i <= num_threads; i++){
		if ( sem_init(&sendsem[i], 0, 1) == 0){
			//created semaphore successfully...nothing more to do?
		}
		else{
			cerr << "sem_init error";
		}
	}

	//create receive semaphores
	for (int i = 0; i <= num_threads; i++){
		if ( sem_init(&rcvsem[i], 0, 0) == 0){
			//created semaphore successfully...nothing more to do?
		}
		else{
			cerr << "sem_init error";
		}
	}

	//create parent thread
	if (pthread_create(&tids[0], NULL, pthreadRoutine, &mailbox_ids[0]) == 0){
		//thread created successfully
	}
	else{
		cerr << "pthread_create error";
		return 1;
	}

	//create worker threads
	for (int i = 1; i <= num_threads; i++){
		if ( pthread_create(&tids[i], NULL, wthreadRoutine, &mailbox_ids[i]) == 0){
			//thread created successfully...do nothing for now?
		}
		else{
			cerr << "pthread_create error";
			return 1;
		}
	}

	//join threads
	for (int i = 0; i <= num_threads; i++){
		int retval;
		void* res;
		retval = pthread_join(tids[i], &res);
		if (retval != 0){
			cerr << "pthread_join error";
		}
	}

	//destroy semaphores
	for (int i = 0; i <= num_threads; i++){
		sem_destroy(&sendsem[i]);
		sem_destroy(&rcvsem[i]);
	}


	return 0;
}