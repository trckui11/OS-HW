#include "BoundedBuffer.h"

BoundedBuffer::BoundedBuffer(int size) {
    this->size = size;
    sem_init(&mutex, 0, 1);
    sem_init(&empty, 0, size);
    sem_init(&full, 0, 0);
}

int BoundedBuffer::getSize() {
    return buffer.size();
}

void BoundedBuffer::insert(string s) {
    // entry section
    sem_wait(&empty);
    sem_wait(&mutex);
    // critical section
    this->buffer.push(s);
    // exit section
    sem_post(&mutex);
    sem_post(&full);
}

string BoundedBuffer::remove() {
    // entry section
    sem_wait(&full);
    sem_wait(&mutex);
    // critical section
    string temp = this->buffer.front();
    this->buffer.pop();
    // exit section
    sem_post(&mutex);
    sem_post(&empty);
    // remainder section
    return temp;
}
