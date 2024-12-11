#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <string>
#include <iostream>
#include "BoundedBuffer.h"
#include "UnboundedBuffer.h"

using namespace std;

class Dispatcher {
    private:
        int numProducers;

    public:
        Dispatcher(int numProducers);
        void dispatch(BoundedBuffer *producerBuffers, UnboundedBuffer *sport, UnboundedBuffer *news, UnboundedBuffer *weather);
};

#endif