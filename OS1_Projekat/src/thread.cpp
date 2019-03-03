#include "thread.h"
#include "PCB.h"
#include "SCHEDULE.h"
#include "System.h"

extern lockFlag;
extern context_switch_requested;

extern PCB* runningThread;
extern PCB* idleThread;

extern Lists* sleepingThreads;

void interrupt timer(...);

Thread::Thread(StackSize stackSize, Time timeSlice){

	lock

	myPCB = new PCB(this, stackSize, timeSlice);

	unlock
}

Thread::~Thread(){

	this->waitToComplete();

	lock
	delete myPCB;
	unlock
}

void Thread::start(){

	lock

	if (myPCB->threadStatus == PCB::RUNNING){

		unlock
		return;
	}

	myPCB->threadStatus = PCB::RUNNING;
	Scheduler::put(myPCB);

	unlock

	// myPCB->start();
}

void Thread::waitToComplete(){

	if (myPCB->threadStatus == PCB::FINISHED)
		return;

	lock
	runningThread->threadStatus = PCB::BLOCKED;
	myPCB->waitingThreads->add(runningThread);
	unlock
	
	dispatch();

	// myPCB->waitToComplete();
}

void Thread::sleep(Time timeToSleep){

	lock

	if ((runningThread->threadStatus != PCB::FINISHED) || (runningThread->threadStatus != PCB::NEW) || (timeToSleep > 0)) {

		runningThread->threadStatus = PCB::BLOCKED;
		sleepingThreads->sleepAdd(runningThread, timeToSleep);

		unlock
		dispatch();
	}

	unlock

	// runningThread->sleep(timeToSleep);

}

PCB* Thread::getPCB(){

	return myPCB;
}

ID Thread::getID(){

	return myPCB->myID;
}

void dispatch(){

	lock

	lockFlag = 0;

	context_switch_requested = 1;

	lockFlag = 1;

	unlock

	timer();
}