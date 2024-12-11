#include "Dispatcher.h"

Dispatcher::Dispatcher(int numProducers) {
    this->numProducers = numProducers;
}

void Dispatcher::dispatch(BoundedBuffer *producerBuffers, UnboundedBuffer *sports, UnboundedBuffer *news, UnboundedBuffer *weather) {
    int doneCount = 0;
    string message;
    
    while (doneCount != this->numProducers) {
        // round robin reading
        for (int i = 0; i < this->numProducers; i++) {
            if (producerBuffers[i].getSize() == 0) {
                continue;
            }
            message = producerBuffers[i].remove();

            if (message.compare("DONE") == 0) {
                doneCount++;
            } else if (message.find("SPORTS") != string::npos) {
                sports->insert(message);
            } else if (message.find("NEWS") != string::npos) {
                news->insert(message);
            } else if (message.find("WEATHER") != string::npos) {
                weather->insert(message);
            }
        }
    }

    sports->insert("DONE");
    news->insert("DONE");
    weather->insert("DONE");
}