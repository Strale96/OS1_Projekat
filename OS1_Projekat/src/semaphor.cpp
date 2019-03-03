#include "semaphor.h"
#include "SSem.h"
#include "System.h"

Semaphore::Semaphore(int init) {

	lock
	myImpl = new KernelSem(init);
	unlock

}

Semaphore::~Semaphore() {

	delete myImpl;
}

int Semaphore::wait(int toBlock){

	lock
	int myRetValue = myImpl->wait(toBlock);
	unlock

	return myRetValue;
}

void Semaphore::signal(){

	lock

	myImpl->signal();

	unlock
}

int Semaphore::val() const{

	return myImpl->getVal();
}