#ifndef UNBOUNDEDBUFFER_H
#define UNBOUNDEDBUFFER_H

#include <semaphore.h>
#include <string>
#include <iostream>
#include <queue>

using namespace std;

class UnboundedBuffer {
    private:
        queue<string> buffer;
        sem_t mutex, full;

    public:
        UnboundedBuffer();
        void insert(string s);
        string remove();
};

#endif