#ifndef PRODUCER
#define PRODUCER

#include <semaphore.h>
#include <string>
#include <queue>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include "BoundedBuffer.h"

/*
sports - 1
news - 2
weather - 3
*/ 

using namespace std;

class Producer {
    private:
        int id;
        int numProducts;
        int sports, weather, news;
    public:
        Producer(int id = 0, int numProducts = 0);
        int getID();
        string makeProduct();
        void produce(BoundedBuffer *buffer);
};

#endif