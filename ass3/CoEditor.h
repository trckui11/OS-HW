#ifndef COEDITOR_H
#define COEDITOR_H

#include <string>
#include <unistd.h>
#include <iostream>
#include "UnboundedBuffer.h"

using namespace std;

class CoEditor {
    public:
        CoEditor();
        void edit(UnboundedBuffer *messageBuffer, UnboundedBuffer *screenBuffer);
};

#endif