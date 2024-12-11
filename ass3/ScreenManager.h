#ifndef SCREENMANAGER_H
#define SCREENMANAGER_H

#include <string>
#include <iostream>
#include "BoundedBuffer.h"
#include "UnboundedBuffer.h"

using namespace std;

class ScreenManager {
    public:
        ScreenManager();
        void display(UnboundedBuffer *screenBuffer);
};

#endif