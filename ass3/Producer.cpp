#include "Producer.h"

Producer::Producer(int id, int numProducts) {
    this->id = id;
    this->sports = 0;
    this->weather = 0;
    this->news = 0;
    this->numProducts = numProducts;
}

int Producer::getID() {
    return this->id;
}

string Producer::makeProduct() {
    int messageType = rand() % 3 + 1;
    int typeCounter;
    string messageTypeString;
    
    switch (messageType) {
    case 1:
        messageTypeString = "SPORTS";
        typeCounter = this->sports;
        this->sports++;
        break;
    
    case 2:
        messageTypeString = "NEWS";
        typeCounter = this->news;
        this->news++;
        break;

    case 3:
        messageTypeString = "WEATHER";
        typeCounter = this->weather;
        this->weather++;
        break;
    }

    return "Producer " + to_string(this->id) + " " + messageTypeString + " " + to_string(typeCounter);
}

void Producer::produce(BoundedBuffer *buffer) {
    for (int i = 0; i < numProducts; i++) {
        buffer->insert(makeProduct());
    }
    
    buffer->insert("DONE");
}