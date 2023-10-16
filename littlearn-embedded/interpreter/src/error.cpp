#include "error.hpp"

void handleError(const std::string& errorMessage) {

    // If running on an embedded device, use Serial.print to print errors
    #if __EMBEDDED__
        Serial.println(("__ERROR__" + errorMessage + "__ERROR__").c_str());
    #else
        std::cout << errorMessage << std::endl;
        exit(1);
    #endif
}