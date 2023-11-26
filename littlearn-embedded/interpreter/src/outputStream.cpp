#include "outputStream.hpp"

#if __EMBEDDED__
#include <Arduino.h>

void SerialOutputStream::write(const std::string& message) {
    Serial.println(message.c_str());
}





#else
#include <iostream>

void StandardOutputStream::write(const std::string& message) {
    std::cout << message;
}
#endif
