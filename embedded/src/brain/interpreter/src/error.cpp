#include <mutex>

#include "error.hpp"
#include "flags.h"

// Initialize stopExecution flag to false
// Only set back to true when reuploading code TODO
bool stopExecution = false;
std::mutex stopExecutionMutex;


ErrorHandler::ErrorHandler(OutputStream& outputStream) : outputStream(outputStream) {}

ErrorHandler::~ErrorHandler() {}

void ErrorHandler::handleError(const std::string& errorMessage) {
    // Raise stopExecution flag
    std::lock_guard<std::mutex> lock(stopExecutionMutex);
    stopExecution = true;
    // Print error message
    printf("Error: %s\n", errorMessage.c_str());
    outputStream.write(ERROR_FLAG + errorMessage + "\n" + ERROR_FLAG);
}

bool ErrorHandler::shouldStopExecution() {
    std::lock_guard<std::mutex> lock(stopExecutionMutex);
    printf("CALLED stopExecution: %d\n", stopExecution);
    return stopExecution;
}

void ErrorHandler::triggerStopExecution() {
    std::lock_guard<std::mutex> lock(stopExecutionMutex);
    stopExecution = true;
}

void ErrorHandler::resetStopExecution() {
    std::lock_guard<std::mutex> lock(stopExecutionMutex);
    stopExecution = false;
}
