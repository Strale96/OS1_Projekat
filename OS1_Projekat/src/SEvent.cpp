#include "SEvent.h"
#include "System.h"
#include "SCHEDULE.h"
#include <dos.h>
#include <iostream.h>

extern PCB* runningThread;

extern IVTEntry* myEntries[256];

KernelEv::KernelEv(IVTNo ivtNo){

	lock

	this->myIVTNo = ivtNo;
	this->myPCB = runningThread;
	this->threadIsBlocked = 0;

	myValue = 1;

	if (myEntries[ivtNo] != 0){

		if (myEntries[ivtNo]->myEvent == 0){

			myEntries[ivtNo]->myEvent = this;
		} else {

			cout << endl << "ERROR! Entry has event!" << endl;
		}
	} else {

		cout << endl << "ERROR! IVTEntry not initialised" << endl;
	}

	unlock
}

KernelEv::~KernelEv(){

	myEvent = 0;
	myEntries[myIVTNo]->myEvent = 0;
}

void KernelEv::wait(){

	lock

	if (runningThread->myID == myPCB->myID){
		if (myValue == 1){

			myValue = 0;
		} else {

			threadIsBlocked = 1;
			myPCB->threadStatus = PCB::BLOCKED;

			unlock
			dispatch();
		}
	}

	unlock 
}

void KernelEv::signal(){

	lock

	if (threadIsBlocked == 0){

		myPCB->threadStatus = PCB::RUNNING;
		myValue = 1;
	} else {

		threadIsBlocked = 0;
		myPCB->threadStatus = PCB::RUNNING;
		Scheduler::put(myPCB);
	}

	myValue = 1;
	unlock
	dispatch();
}

// IVTEntry klasa
IVTEntry::IVTEntry(IVTNo ivtNo, Inter newInter){

	lock

	myNo = ivtNo;
	myEvent = 0;

	if (myEntries[myNo] == 0){
		myEntries[myNo] = this;
		//TODO: da li ovde ima greske?

		oldInter = getvect(myNo);
		setvect(myNo, newInter);

	} else {
		// Ovo ne bi smelo da se desi, znaci da je IVT ulaz vec promenjen
		// Ovde se moze ubaciti parce koda koje menja trenutni newInter sa novim newInter-om, dok cuva originalni oldInter
		// Time bi se ukljucila overwrite mogucnost
		// Tj deo koda bi izgledao kao iznad, samo bez oldInter = getvect(myNo); linije
		// *********************************
		// myEntries[myNo] = this;
		// setvect(myNo, newInter);
		//
		// *********************************
	}

	unlock

}

IVTEntry::~IVTEntry(){

	// TODO: da li ovde ima greske?
	setvect(myNo, oldInter);
}

void IVTEntry::signal(){

	lock

	if (myEvent != 0) myEvent->signal();

	unlock
}

void IVTEntry::runOldInter(){

	oldInter();
}