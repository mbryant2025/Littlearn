#include "error.hpp"
#include "poll.h"


// Initialize stopExecution flag to false
// Only set back to true when reuploading code TODO
bool stopExecution = false;

void handleError(const std::string& errorMessage) {

    // Raise stopExecution flag
    stopExecution = true;

    // If running on an embedded device, use Serial.print to print errors
    #if __EMBEDDED__
        // TODO transition to Bluetooth
        Serial.println(("__ERROR__" + errorMessage + "__ERROR__").c_str());
    #else
        std::cout << errorMessage << std::endl;
        // exit(1);
    #endif
}

bool shouldStopExecution() {
    return stopExecution;
}
