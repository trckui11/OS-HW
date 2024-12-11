#include <pthread.h>
#include <fstream>
#include "Producer.h"
#include "CoEditor.h"
#include "Dispatcher.h"
#include "ScreenManager.h"

BoundedBuffer *producerBuffers;
Producer *producers;

UnboundedBuffer sportsBuf;
UnboundedBuffer newsBuf;
UnboundedBuffer weatherBuf;
UnboundedBuffer screenBuf;

void *producerThread(void * arg) {
    int index = *((int *)arg);
    producers[index].produce(&producerBuffers[index]);
    return (void *)0;
}

void *dispatcherThread(void * arg) {
    int numProducers = *((int *)arg);
    Dispatcher dispatcher(numProducers);
    dispatcher.dispatch(producerBuffers, &sportsBuf, &newsBuf, &weatherBuf);
    return (void *)0;
}

void *sportsEditorThread(void * arg) {
    CoEditor coEditor;
    coEditor.edit(&sportsBuf, &screenBuf);
    return (void *)0;
}

void *newsEditorThread(void * arg) {
    CoEditor coEditor;
    coEditor.edit(&newsBuf, &screenBuf);
    return (void *)0;
}

void *weatherEditorThread(void * arg) {
    CoEditor coEditor;
    coEditor.edit(&weatherBuf, &screenBuf);
    return (void *)0;
}

void *screenManagerThread(void * arg) {
    ScreenManager manager;
    manager.display(&screenBuf);
    return (void *)0;
}

int main(int argc, char *argv[]) {
    // count lines
    ifstream configFile(argv[1]);
    int lineCount = count(istreambuf_iterator<char>(configFile), istreambuf_iterator<char>(), '\n') + 1;
    int numProducers = (lineCount - 1) / 4;
    configFile.seekg(0, configFile.beg);  // reset file offset to beginning

    producerBuffers = new BoundedBuffer[numProducers];
    producers = new Producer[numProducers];

    // parse file
    string line;
    int i = 0;
    while (getline(configFile, line)) {
        if (line[0] == 'C') {
            break;
        }
        int id = stoi(line.substr(9));
        
        getline(configFile, line); // [number of products]
        int numProducts = stoi(line);
        producers[i] = Producer(id, numProducts);

        getline(configFile, line); // queue size = [size]
        int queueSize = stoi(line.substr(13)); // Extract size
        producerBuffers[i] = BoundedBuffer(queueSize);

        getline(configFile, line); // newline
        i++;
    }
    configFile.close();

    CoEditor sportsEditor();
    CoEditor newsEditor();
    CoEditor weatherEditor();

    pthread_t producerTIDs[numProducers];
    int indices[numProducers];
    for (i = 0; i < numProducers; i++) {
        indices[i] = i;
        pthread_create(&producerTIDs[i], NULL, producerThread, &indices[i]);
    }

    pthread_t dispathcerTID;
    pthread_create(&dispathcerTID, NULL, dispatcherThread, &numProducers);
    
    pthread_t coEditorTIDs[3];
    pthread_create(&coEditorTIDs[0], NULL, sportsEditorThread, NULL);
    pthread_create(&coEditorTIDs[1], NULL, newsEditorThread, NULL);
    pthread_create(&coEditorTIDs[2], NULL, weatherEditorThread, NULL);

    pthread_t screenManagerTID;
    pthread_create(&screenManagerTID, NULL, screenManagerThread, NULL);

    // wait for all threads
    for (i = 0; i < numProducers; i++) {
        pthread_join(producerTIDs[i], NULL);
    }
    pthread_join(dispathcerTID, NULL);
    for (i = 0; i < 3; i++) {
        pthread_join(coEditorTIDs[i], NULL);
    }
    pthread_join(screenManagerTID, NULL);

    delete[] producers;
    delete[] producerBuffers;

    return 0;
}
