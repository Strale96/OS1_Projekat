#ifndef LISTS_H_
#define LISTS_H_

#include "thread.h"

class PCB;

class Lists{

	public:

		struct Node{

			Node(PCB* newPCB, Time timeToSleep = -1){

				myPCB = newPCB;
				timeLeft = timeToSleep;
				
				next = prev = 0;
			};

			PCB* myPCB;
			Time timeLeft;

			Node* next;
			Node* prev;
		};

		Lists();
		~Lists();

		void add(PCB* newPCB);		// Funkcija koja dodaje clan na kraj liste
		void remove();				// Funkcija koja uklanja prvi clan iz liste				
		void sleepAdd(PCB* newPCB, Time timeToSleep);	// Funkcija koja dodaje nit u listu uspavanih niti
														// Koristi se samo u listi uspavanih niti
		void unblockAllThreads();	// Funkcija koja odblokira sve niti
		void tickAll();				// Funkcija koja se koristi u listi uspavanih niti, meri protok vremena u prekidnoj rutini

		Node* head;		// Pokazivac na prvi element liste
		Node* tail;		// Pokazivac na poslednji element liste
};

#endif