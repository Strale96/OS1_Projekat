#include "SSem.h"
#include "Lists.h"
#include "PCB.h"
#include "System.h"
#include "SCHEDULE.h"

extern PCB* runningThread;

KernelSem::KernelSem(int init) {

	lock

	myValue = init;
	blockedList = new Lists();

	unlock
}

KernelSem::~KernelSem(){

	lock

	myValue = 0;

	if (blockedList->head != 0)
		blockedList->unblockAllThreads();

	delete blockedList;

	unlock
}

int KernelSem::wait(int toBlock){

	lock

	if (toBlock != 0){

		// Ukoliko toBlock nema vrednost 0 operacija se izvrsava na standardan nacin
		myValue--;
		if (myValue >= 0){

			unlock
			return 0;
		} else {

			blockedList->add(runningThread);

			runningThread->threadStatus = PCB::BLOCKED;

			unlock
			dispatch();
			return 1;
		}
	} else {

		// Ako toBlock ima vrednost nula:
		if (myValue <= 0){

			// Ako je myValue <= 0 pozivajuca nit se nece blokirati, semafor nece menjati vrednost a funkcija ce vratiti -1
			unlock
			return -1;
		} else {

			// Ako je myValue > 0 (tj myValue = 1) operacija se izvrsava na standardan nacin, tj semafor se umanjuje a
			// Ova funkcija vraca 0 posto se nit ne blokira

			myValue--;
			unlock
			return 0;
		}
	}
}

void KernelSem::signal(){

	lock

	myValue++;

	if (myValue <= 0){

		blockedList->remove();
	}

	unlock
}

int KernelSem::getVal(){

	return myValue;
}