#ifndef BOUNDEDBUFFER_H
#define BOUNDEDBUFFER_H

#include <semaphore.h>
#include <string>
#include <queue>
#include <iostream>

using namespace std;

class BoundedBuffer {
    private:
        queue<string> buffer;
        int size;
        sem_t mutex, empty, full;
    
    public:
        BoundedBuffer(int size = 0);
        void insert(string s);
        string remove();
        int getSize();
};

#endif