#ifndef _thread_h_
#define _thread_h_


typedef unsigned long StackSize;
const StackSize defaultStackSize = 4096;

typedef unsigned int Time; // time, x 55ms
const Time defaultTimeSlice = 2; // default = 2*55ms

typedef int ID;

class PCB; // Kernel's implementation of a user's thread

class Thread{
	
	public:

		PCB* getPCB();
		ID getID();

		void start();
		void waitToComplete();
		virtual ~Thread();

		static void sleep(Time timeToSleep);
		Thread (StackSize stackSize = defaultStackSize, Time timeSlice = defaultTimeSlice);
		
	protected:

		friend class PCB;
		virtual void run(){};

		PCB* myPCB;		
};

void dispatch ();
#endif