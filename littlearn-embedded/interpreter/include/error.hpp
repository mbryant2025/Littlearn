// Handles errors and exceptions
// Directs them to the appropriate place
// Ex. if run in a terminal, prints to terminal
// Ex. if run in on embedded device, prints to serial

#ifndef ERROR_HANDLING_HPP
#define ERROR_HANDLING_HPP

// 0 if running on an embedded device, 1 if building an executable for desktop
#define __EMBEDDED__ 0

// Conditional import of Arduino.h/iostream
#if __EMBEDDED__
#include <Arduino.h>
#else
#include <iostream>
#endif // __EMBEDDED__

// Function to handle errors and exceptions
void handleError(const std::string& errorMessage);

#endif // ERROR_HANDLING_HPP
