#ifndef Semaphore_h
#define Semaphore_h

#include <stdio.h>
#include <list>
#include <string>
#include <pthread.h>


class Semaphore {
		pthread_mutex_t mu;
		int counter;
		pthread_cond_t cond = PTHREAD_COND_INITIALIZER; 

	public: 
		Semaphore();
		Semaphore(int num);
		~Semaphore();
		void P();
		void V();
};

#endif /* Semaphore_ */
