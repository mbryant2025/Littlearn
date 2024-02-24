#ifndef OUTPUTSTREAM_HPP
#define OUTPUTSTREAM_HPP

// 1 if running on an embedded device, 0 if building an executable for desktop
#define __EMBEDDED__ 0

#include <string>

// Setup for dependency injection of the output stream.

class OutputStream {
public:
    virtual void write(const std::string& message) = 0;
    virtual ~OutputStream() = default;
};


// For PC environment, use std::cout
class StandardOutputStream : public OutputStream {
public:
    void write(const std::string& message) override;
};



#endif // OUTPUTSTREAM_HPP