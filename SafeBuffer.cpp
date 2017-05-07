//
//  SafeBuffer.cpp
//
//
//  Created by Joshua Higginbotham on 11/4/15.
//
//

#include "SafeBuffer.h"

SafeBuffer::SafeBuffer(int max_size) {
	pthread_mutex_init(&mutex, NULL);
	fullSlots = Semaphore(0);
	emptySlots = Semaphore(max_size);
}

SafeBuffer::~SafeBuffer() {
	pthread_mutex_destroy(&mutex);
}

int SafeBuffer::size() {
	pthread_mutex_lock(&mutex);
    return buffer.size();
	pthread_mutex_unlock(&mutex);
}

void SafeBuffer::push_back(std::string str) {
	emptySlots.P();
	pthread_mutex_lock(&mutex);
	buffer.push_back(str);
	pthread_mutex_unlock(&mutex);
	fullSlots.V();
}

std::string SafeBuffer::retrieve_front() {
	fullSlots.P();
	pthread_mutex_lock(&mutex);
	std::string front = buffer.front();
	buffer.pop_front();
	pthread_mutex_unlock(&mutex);
	emptySlots.V();
	return front;
}
