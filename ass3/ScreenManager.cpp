#include "ScreenManager.h"

ScreenManager::ScreenManager() {}

void ScreenManager::display(UnboundedBuffer *screenBuffer) {
    int doneCount = 0;
    string message;

    while (doneCount < 3) {
        message = screenBuffer->remove();
        if (message.compare("DONE") == 0) {
            doneCount++;
            continue;
        }

        cout << message << "\n";
    }
    cout << "DONE\n";
}