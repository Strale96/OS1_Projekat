#ifndef PCB_H_
#define PCB_H_

#include "thread.h"
#include "lists.h"

class PCB{

	public:
	
		enum Status{NEW, READY, RUNNING, BLOCKED, FINISHED, SYSTEM};	// Definicija za status niti

		// Konstruktor i destruktor
		PCB(Thread* myThread, StackSize myStackSize, Time myTimeSlice);
		~PCB();

		void unblock();						// Funkcija koja odblokira nit i stavlja je u Scheduler

		static void threadRunWrapper();		// Funkcija koja sluzi kao public omotac za Thread::run()

		// Pokazivaci
		unsigned mySP;				
		unsigned mySS;
		unsigned myBP;

		// Stek niti
		unsigned* stack;

		// Podatci neophodni za uspesno izvrsavanje
		Thread* myThread;
		ID myID;
		Time myTime;
		Lists* waitingThreads;
		Status threadStatus;

		// Globalna promenljiva koja "broji" niti
		static volatile ID GLOBAL_ID;
};

// Klasa koja prestavlja glavnu korisnicku nit
// Nasledjuje klasu Thread
class UserThreadClass: public Thread{

	public:

		UserThreadClass(int argc, char** argv);
		void run();

		int myResult;
		int argc;
		char** argv;
};

// Klasa koja prestavlja idleThread
class IdleThreadClass: public Thread{

	public:

		IdleThreadClass();
		void run();
};

#endif