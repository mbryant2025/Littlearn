#include "outputStream.hpp"

#include <iostream>

void StandardOutputStream::write(const std::string& message) {
    std::cout << message;
}

