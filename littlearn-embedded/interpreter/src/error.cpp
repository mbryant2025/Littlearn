#include "error.hpp"

// Initialize stopExecution flag to false
// Only set back to true when reuploading code TODO
bool stopExecution = false;


ErrorHandler::ErrorHandler(OutputStream* outputStream) : outputStream(outputStream) {}

void ErrorHandler::handleError(const std::string& errorMessage) {
    // Raise stopExecution flag
    stopExecution = true;
    // Print error message
    outputStream->write(errorMessage + "\n");
}

bool ErrorHandler::shouldStopExecution() {
    return stopExecution;
}

void ErrorHandler::triggerStopExecution() {
    stopExecution = true;
}

void ErrorHandler::resetStopExecution() {
    stopExecution = false;
}
