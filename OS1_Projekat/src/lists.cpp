#include "lists.h"
#include "PCB.h"
#include "System.h"


Lists::Lists(){

	lock

	head = 0;
	tail = 0;

	unlock
}

Lists::~Lists(){

	lock

	if (head == 0) 
		return;
	
	Node* tempNode = 0;
	while (tempNode != 0){
		
		tempNode = head;
		head = head->next;

		delete tempNode;
	}

	unlock
}

void Lists::add(PCB* newPCB){

	// U obicnoj funkciji za dodavanje nema potrebe koristiti Node->prev clan
	// Sva dodavanja u uklanjanja su preko head i tail

	lock

	Node* newNode = new Node(newPCB);

	if (head == 0)
		head = tail = newNode;
	else {

		tail->next = newNode;
		tail = tail->next;
	}

	unlock
}

void Lists::remove(){

	lock

	if (head == 0) {

		unlock
		return;
	}

	Node* tempNode = head;
	head = head->next;
	
	if (head == 0) 
		tail = 0;	
	
	head->prev = 0;	// Ovo je nebitno kada se uklanjaju niti iz obicne liste
					// Bitno je za listu uspavanih niti
	tempNode->myPCB->unblock();
	delete tempNode;

	unlock
}

void Lists::sleepAdd(PCB* newPCB, Time timeToSleep){

	lock

	Node* newNode = new Node(newPCB, timeToSleep);

	newNode->next = 0;
	newNode->prev = 0;

	if (head == 0){

		head = tail = newNode;
	} else {

		Node* tempNode = head;

		int change = 1;		// Flag koji mi govori da li je unutar while petlje izvrseno ubacivanje u listu
							// Tj, nova nit ima isto vreme spavanja kao vec postojeca nit
	
		while((tempNode != 0) && (tempNode->timeLeft <= newNode->timeLeft) && (change == 1)){

			newNode->timeLeft -= tempNode->timeLeft;

			if (newNode->timeLeft == 0){

				// Dolazi se do poslednjeg timeLeft = 0
				if (tempNode->next != 0)
					if (tempNode->next->timeLeft == 0)
					while(tempNode->next->timeLeft != 0){

						tempNode = tempNode->next;
						if (tempNode->next == 0) break;
					}

				change = 0;
				// newNode->sleep(x) == existingNode->sleep(x)
				if (tempNode->next == 0){

					tempNode->next = newNode;

					newNode->prev = tail;
					tail = newNode;

				} else {

					newNode->next = tempNode->next;
					newNode->prev = tempNode;

					tempNode->next->prev = newNode;
					tempNode->next = newNode;

				}
			} else tempNode = tempNode->next;
		}

		// Ovde se obradjuje drugi slucaj - nova uspavana nit je ugnjezdena izmedju dve vec postojece uspavane niti
		// Dakle, newNode->sleep(x), existingNode1->sleep(x-a), existingNode2->sleep(x+b)
		if ((tempNode != 0) && (change)){

			if (tempNode->prev == 0){

				// Slucaj kada nova uspavana nit ima najmanju sleep vrednost
				tempNode->prev = newNode;
				newNode->next = tempNode;

				head = newNode;

			} else {

				newNode->prev = tempNode->prev;
				newNode->next = tempNode;

				tempNode->prev->next = newNode;
				tempNode->prev = newNode;
			}

			tempNode->timeLeft -= newNode->timeLeft;
		}

		// Obrada poslednjeg slucaja - newNode ima najvecu sleep vrednost
		if (tempNode == 0){

			newNode->prev = tail;

			tail->next = newNode;
			tail = newNode;

		}
	}

	unlock
}

void Lists::tickAll(){

	Node* tempNode = head;

	if (tempNode != 0){

		tempNode->timeLeft--;

		if (tempNode->timeLeft == 0){

			while (tempNode->timeLeft == 0){

				tempNode = tempNode->next;

				remove();

				if (tempNode == 0) break;
			}
		}
	}
}

void Lists::unblockAllThreads(){

	Node* tempNode = head;

	while(tempNode != 0){

		head->myPCB->unblock();
		head = head->next;
		
		delete tempNode;
		tempNode = head;
	}
}