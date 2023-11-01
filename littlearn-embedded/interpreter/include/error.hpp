// Handles errors and exceptions
// Directs them to the appropriate place
// Ex. if run in a terminal, prints to terminal
// Ex. if run in on embedded device, prints to serial

#ifndef ERROR_HANDLING_HPP
#define ERROR_HANDLING_HPP

// Global flag for the interpreter to stop execution
// Can be an error or an interrupt from the GUI
extern bool stopExecution;

// 1 if running on an embedded device, 0 if building an executable for desktop
#define __EMBEDDED__ 0

// Conditional import of Arduino.h/iostream
#if __EMBEDDED__
#include <Arduino.h>
#include <string>
#else
#include <iostream>
#endif // __EMBEDDED__

// Function to handle errors and exceptions
void handleError(const std::string& errorMessage);

bool shouldStopExecution();

void triggerStopExecution();

void resetStopExecution();

#endif // ERROR_HANDLING_HPP
