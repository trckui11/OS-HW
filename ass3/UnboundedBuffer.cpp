#include "UnboundedBuffer.h"

UnboundedBuffer::UnboundedBuffer() {
    sem_init(&mutex, 0, 1);
    sem_init(&full, 0, 0);
}

void UnboundedBuffer::insert(string s) {
    // entry section
    sem_wait(&mutex);
    // critical section
    this->buffer.push(s);
    // exit section
    sem_post(&mutex);
    sem_post(&full);
}

string UnboundedBuffer::remove() {
    // entry section
    sem_wait(&full);
    sem_wait(&mutex);
    // critical section
    string temp = this->buffer.front();
    this->buffer.pop();
    // exit section
    sem_post(&mutex);
    // remainder section
    return temp;
}