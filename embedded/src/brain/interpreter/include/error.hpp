// Handles errors and exceptions
// Directs them to the appropriate place
// Ex. if run in a terminal, prints to terminal
// Ex. if run in on embedded device, prints to serial

#ifndef ERROR_HANDLING_HPP
#define ERROR_HANDLING_HPP

#include "outputStream.hpp"

// Global flag for the interpreter to stop execution
// Can be an error or an interrupt from the GUI
extern bool stopExecution;

class ErrorHandler {
   public:
    ErrorHandler(OutputStream& outputStream);
    ~ErrorHandler();
    // Function to handle errors and exceptions
    void handleError(const std::string& errorMessage);

    bool shouldStopExecution();

    void triggerStopExecution();

    void resetStopExecution();

   private:
    OutputStream& outputStream;
};

#endif  // ERROR_HANDLING_HPP
