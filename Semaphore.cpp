#include <cassert>
#include <cstring>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include "Semaphore.h"

Semaphore::Semaphore()
{
	counter = 0;
	int success = pthread_cond_init(&cond, NULL);
	pthread_mutex_init(&mu, NULL);
}

Semaphore::Semaphore(int num)
{
	counter = num;
	int success = pthread_cond_init(&cond, NULL);
	pthread_mutex_init(&mu, NULL);
}

Semaphore::~Semaphore()
{
	int ret = pthread_cond_destroy(&cond);
	pthread_mutex_destroy(&mu);
}

void Semaphore::P()
{
	pthread_mutex_lock(&mu);
	counter--;
	if(counter < 0)
	{
		pthread_cond_wait(&cond, &mu);
	}
	pthread_mutex_unlock(&mu);
}

void Semaphore::V()
{
	pthread_mutex_lock(&mu);
	counter++;
	if(counter <= 0)
	{
		pthread_cond_signal(&cond); 
	}
	pthread_mutex_unlock(&mu);
}