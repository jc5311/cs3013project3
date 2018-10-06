//Program Created by Juan Caraballo
//At its current state this program seems to be getting stuck
//after printing out the mapping of generation 0. I believe it may be
//due to the pending and blocking on a semaphore as I had a similar
//issue with my addem program however however that was fixed and was due
//to a semaphore posted at the wrong time (causing a deadlock). And because
//the message passing system works for addem I question whether the issue
//in this life program is actually due to that same problem.

#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <fstream>
#include <cstdlib>
#include <string.h>
#include <sstream>

#define MAXTHREAD 10
#define MAXGRID 40
#define RANGE 1
#define ALLDONE 2
#define GO 3
#define GENDONE 4
#define STOP 5

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
int numof_gens = 0;
int yes_print = 0; //0 = no
int yes_input = 0; //0 = no
int rows = 0;
int cols = 0;
int **even_gen;
int **odd_gen;
const char* filename;

ifstream file;


//functions

//send a message
int SendMsg(int iTo, msg *pMsg){
	
	//check if the receive semaphore is available
	//meaning the mailbox is empty
	//mutex the action of sending
	sem_wait(&sendsem[iTo]);
	mailboxes[iTo] = *pMsg;
	sem_post(&rcvsem[iTo]);
	return 0;
}

//receive a message
int RecvMsg(int iFrom, msg *pMsg){
	//check if send semaphore is available
	//meaning mailbox is full
	sem_wait(&rcvsem[iFrom]);
	//mutex the action of receiving
	*pMsg = mailboxes[iFrom];
	sem_post(&sendsem[iFrom]);
	return 0;
}

//print the current game grid
void pgrid(int eo){
	for (int i = 0; i < rows; i++){
		for (int j = 0; j < cols; j++){
			if (eo == 1){	//odd ending
				cout << odd_gen[i][j] << " ";
			}else{	//even ending
				cout << even_gen[i][j] << " ";
			}
		}
		cout << endl;
		cout.flush();
	}
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
	int start = 1, end = 0, mod = 0;
	for (int i = 1; i <= num_threads; i++){
		//do some number play since things get weird when the number to sum_to
		//is not evenly divisble by the num_threads
		if (i > 1)
			start = end + 1;
		end = (start - 1) + (rows/num_threads);
		mod = rows % num_threads;

		if (i <= mod)
			end++;

		msg message;
		message.iSender = 0;
		message.type = RANGE;
		message.value1 = start;
		message.value2 = end;
		SendMsg(i, &message);
	}

	int done_count = 0;
	int last_gen = 0;
	for (int i = 1; i <= numof_gens; i++){
		if (yes_print){
			cout << "Generation " << i-1 << endl;
			pgrid((i-1)%2);
		}

		if(yes_input){
			cin.get();
		}

		for(int j = 0; j < num_threads; j++){
			msg message;
			message.iSender = 0;
			message.type = GO;
			SendMsg(j+1, &message);
		}
		
		int same = 1, empty = 1;
		for(int j = 0; j < num_threads; j++){
			msg message;
			int t;
			RecvMsg(0, &message);
			if (message.type == ALLDONE){
				done_count++;
			}
			if (message.value1 == 0){
				empty = 0;
			}
			if (message.value2 == 0)
				same = 0;

		}
		last_gen = i;
		if (same == 1 || empty == 1){
			//Send stop message
			for(int j = 0; j < num_threads; j++){
				msg message;
				message.iSender = 0;
				message.type = STOP;
				SendMsg(j+1, &message);
			}
			break;
		}
		
	}

	//wait for ALLDONE messages from everybody
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
	//check that the user entered the proper amount of cli arguments
	if (argc < 4){
		cout << "Please enter program args in the following format:" << endl;
		cout << "./life (int)threads (FILE)filename (int)generations (y|n)print	(y|n)input" << endl;
			return 1;
	}
	else if (argc >= 4){
		//get number of threads to use
		int nt = atoi(argv[1]);
		if (nt <= 0){
			cout << "Error: Please enter 1-10 threads as the first argument."
			<< endl;
			return 1;
		}
		else if ((nt > 0) && (nt < MAXTHREAD)){
			//if nt is between 0 and MAXTHREAD 
			num_threads = nt;
		}
		else{
			//if nt is >= MAXTHREAD
			num_threads = MAXTHREAD;
		}

		//get file
		filename = argv[2];

		//get generations 
		numof_gens = atoi(argv[3]);
		if (numof_gens <= 0){
			cout << "Error: Numbers of generations should be greater than 0." 
				<< endl;
			return 1;
		}

		//does user want to print results?
		if (argc > 4){
			if (strcmp(argv[4], "y") == 0){
				yes_print = 1;
			}
		}
		if (argc > 5){
			if (strcmp(argv[5], "y") == 0){
				yes_input = 1;
			}
		}

	} //end of cli arg collection


	//check grid
	string line;
	file.open(filename);
	if(!file){
		cerr << "file_open error";
		return 1;
	}
	
	while(getline(file, line)){
		istringstream strstream (line);
		int count = 0, a = 0;
		while (strstream >> a){
			count++;
		}
		if(cols == 0)
			cols = count;
		else if (cols != count && count != 0){	//Check for an even grid ignoreing whitespace
			cout << "Error: Invalid grid" << endl;
			return 1;
		}
		if (count != 0)
			rows++;
	}
	file.close();

	if (rows == 0 || cols == 0 || rows > MAXGRID || cols > MAXGRID){
		cout << "Error: Invalid grid" << endl;
	}

	//create and fill in our game grid
	//reopen the file since we already went through it previously
	//(makes life easier... no pun intended)
	//create two-dimensional arrays
	file.open(filename);
	even_gen = new int* [rows];
	odd_gen = new int* [rows];
	for (int i = 0; i < rows; i++){
		even_gen[i] = new int [cols];
		odd_gen[i] = new int [cols];
	}
	//now fill in those arrays
	for (int i = 0; i < rows; i++){
		for (int j = 0; j < cols; j++){
			int marker;
			file >> marker;
			even_gen[i][j] = marker;
		}
	}

	//make num_threads - rows if neccesary
	if (num_threads > rows)
		num_threads = rows;

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