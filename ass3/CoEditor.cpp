#include "CoEditor.h"

CoEditor::CoEditor() {}

void CoEditor::edit(UnboundedBuffer *messageBuffer, UnboundedBuffer *screenBuffer) {
    string message;
    while (true) {
        message = messageBuffer->remove();
        if (message.compare("DONE") == 0) {
            screenBuffer->insert("DONE");
            break;
        }
        usleep(100);
        screenBuffer->insert(message);
    }
}