#include "PCB.h"
#include "SCHEDULE.h"
#include "System.h"
#include <dos.h>

extern int lockFlag;
extern int context_switch_requested;

extern PCB* runningThread;
extern PCB* idleThread;
extern PCB* userThread;

extern Lists* sleepingThreads;

extern void returnToSystem();

volatile ID PCB::GLOBAL_ID = 0;

PCB::PCB(Thread* myThread, StackSize stackSize, Time timeSlice){

	// lock i unlock su obezbedjeni pri pozivu konstruktora iz Thread klase

	stackSize /= sizeof(unsigned);
	stack = new unsigned[stackSize];

	stack[stackSize - 3] = 0x200;
	stack[stackSize - 4] = FP_SEG(PCB::threadRunWrapper);
	stack[stackSize - 5] = FP_OFF(PCB::threadRunWrapper);

	mySP = FP_OFF(stack + stackSize - 14);
	mySS = FP_SEG(stack + stackSize - 14);
	myBP = mySP;

	this->myThread = myThread;
	this->myTime = timeSlice;
	this->myID = ++GLOBAL_ID;
	this->waitingThreads = new Lists();
	this->threadStatus = NEW;

	// Prva nit koja se kreira je userMain nit (System.cpp -> main(int argc, char** argv))
	if (myID == 1)
		userThread = this;

	// Druga nit koja se kreira je idleThraed nit
	if (myID == 2)
		idleThread = this;
}

PCB::~PCB(){

	delete waitingThreads;
	delete stack;
}

void PCB::unblock(){

	threadStatus = RUNNING;
	Scheduler::put(this);
}

void PCB::threadRunWrapper(){

	Thread* temp = runningThread->myThread;

	if (temp!= 0){

		temp->run();

		lock

		// Posto je nit zavrsila svoj tok, niti koje je cekaju se mogu odblokirati
		temp->myPCB->waitingThreads->unblockAllThreads();
		temp->myPCB->threadStatus = FINISHED;

		unlock
	}

	dispatch();
}

UserThreadClass::UserThreadClass(int argc, char** argv): Thread(){

	this->argc = argc;
	this->argv = argv;
	this->myResult = 0;

	userThread = myPCB;
	userThread->threadStatus = PCB::READY;
	runningThread = userThread;
}

extern int userMain(int, char**);

void UserThreadClass::run(){

	myResult = userMain(argc, argv);
	returnToSystem();		// Funkcija koja omogucava povratak u sistemski tok, definisana je u System.h
}

IdleThreadClass::IdleThreadClass(): Thread(256 ,1){

	idleThread->threadStatus = PCB::SYSTEM;
}

void IdleThreadClass::run(){

	while(1);
}