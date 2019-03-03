#include "System.h"
#include "PCB.h"
#include "SCHEDULE.h"
#include "SEvent.h"
#include <dos.h>
#include <iostream.h>

extern int userMain(int, char**);
extern void tick();

PCB* runningThread = 0;
PCB* idleThread = 0;
PCB* userThread = 0;

Lists* allThreads = new Lists();
Lists* sleepingThreads = new Lists();

IVTEntry* myEntries[256] = {0};

volatile int lockFlag = 1;
volatile int context_switch_requested = 0;

volatile int timeLeft = 1;

void interrupt (*oldInter)(...);

unsigned tsp;
unsigned tss;
unsigned tbp;

unsigned oldsp;
unsigned oldss;
unsigned oldbp;

void interrupt timer(...){

	lock

	int runOldInter = 0;

	if (!context_switch_requested){

		runOldInter = 1;

		tick();

		sleepingThreads->tickAll();

		if (timeLeft > 0)
			timeLeft--;
	}

	if ((timeLeft == 0) || context_switch_requested){

		if (lockFlag){

			context_switch_requested = 0;

			asm{
				mov tsp, sp
				mov tss, ss
				mov tbp, bp
			}

			runningThread->mySP = tsp;
			runningThread->mySS = tss;
			runningThread->myBP = tbp;

			if ((runningThread->threadStatus != PCB::SYSTEM) && (runningThread->threadStatus != PCB::FINISHED) && (runningThread->threadStatus != PCB::BLOCKED))
				Scheduler::put((PCB*) runningThread);

			runningThread = Scheduler::get();

			if (runningThread == 0) 
				runningThread = idleThread;

			if (runningThread->myTime == 0)
				timeLeft = -1;
			else 
				timeLeft = runningThread->myTime;

			tsp = runningThread->mySP;
			tss = runningThread->mySS;
			tbp = runningThread->myBP;

			asm{
				mov sp, tsp
				mov ss, tss
				mov bp, tbp
			}
		} else context_switch_requested = 1;
	}

	if (runOldInter)
		(*oldInter)();

	unlock
}

void inic(){

	lock

	oldInter = getvect(0x08);
	setvect(0x08, timer);
	setvect(0x60, oldInter);

	unlock
}

void restore(){

	lock

	setvect(0x08, oldInter);

	unlock
}

void interrupt runUserThread(){

	asm{
		mov oldsp, sp
		mov oldss, ss
		mov oldbp, bp
	}

	userThread->threadStatus = PCB::RUNNING;
	runningThread = userThread;

	tsp = runningThread->mySP;
	tss = runningThread->mySS;
	tbp = runningThread->myBP;

	asm{
		mov sp, tsp
		mov ss, tss
		mov bp, tbp
	}
}

void interrupt returnToSystem(){

	asm{
		mov sp, oldsp
		mov ss, oldss
		mov bp, oldbp
	}
}

extern int userMain(int, char**);

int main(int argc, char** argv){

	lock

	UserThreadClass* userRes = new UserThreadClass(argc, argv);
	new IdleThreadClass();

	inic();
	
	unlock

	runUserThread();

	int myRes = userRes->myResult;

	restore();

	return myRes;
}