#ifndef SSEM_H_
#define SSEM_H_

class Lists;

class KernelSem {

public:

	KernelSem(int init = 1);
	~KernelSem();

	int wait(int toBlock);
	void signal();

	int getVal();

	int myValue;			// Vrednost na ovom semaforu
	Lists* blockedList;		// Lista niti koje su blokirane na ovom semaforu
};

#endif