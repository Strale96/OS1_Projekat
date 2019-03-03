#ifndef SEVENT_H_
#define SEVENT_H_

#include "event.h"
#include "PCB.h"

typedef unsigned char IVTNo;

typedef void interrupt(*Inter)(...);

class Event;

class KernelEv {

public:

	KernelEv(IVTNo ivtNo);
	virtual ~KernelEv();

	void wait();
	void signal();

	int myValue;
	int threadIsBlocked;
	IVTNo myIVTNo;
	Event* myEvent;
	PCB* myPCB;
};

class IVTEntry{

public:

	IVTEntry(IVTNo ivtNo, Inter newInter);
	~IVTEntry();

	Inter oldInter;
	KernelEv* myEvent;
	IVTNo myNo;

	void signal();
	void runOldInter();
};

// Definicija PREPAREENTRY makroa:
//
// Primer - entryNo = 9, oldInter = 0 :
// PREPAREENTRY(9, 0)
//
// void interrupt Inter_9(...);
// IVTEntry* Entry_9(9, Inter_9) = new IVTEntry(9, Inter_9);
// void interrupt Inter_9(...){
// 		Entry_9->signal();
//		if (0 != 0)
//  		Entry_9->runoldInter();
// }

#define PREPAREENTRY(entryNo,oldInter)\
	void interrupt Inter_##entryNo(...);\
	IVTEntry* Entry_##entryNo = new IVTEntry(entryNo, Inter_##entryNo);\
	void interrupt Inter_##entryNo(...){\
		Entry_##entryNo->signal();\
		if (oldInter != 0)\
			Entry_##entryNo->runOldInter();\
	}


#endif