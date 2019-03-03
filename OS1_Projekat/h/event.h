#ifndef EVENT_H_
#define EVENT_H_

#include "SEvent.h"

typedef unsigned char IVTNo;
class KernelEv;

class Event {

public:

	Event(IVTNo ivtNo);
	virtual ~Event();

	void wait();

protected:

	friend class KernelEv;
	void signal();	// Can call KernelEv

private:

	KernelEv* myImpl;
};

#endif