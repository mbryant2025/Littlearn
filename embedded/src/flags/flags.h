// Prepended and appended to statements to define our communication protocol

#ifndef FLAGS_H
#define FLAGS_H

#ifdef __cplusplus
extern "C" {
#endif

#define PRINT_FLAG "__P__"         // Printing to web console
#define ERROR_FLAG "__ER__"        // Printing error to web console
#define SEND_SCRIPT_FLAG "__SD__"  // Sending script to ESP32
#define SENT_SCRIPT_FLAG "__SS__"  // Acknowledging that the script has been sent to the ESP32
#define QUERY_FLAG "__Q__"         // Brain is querying for peripherals
#define IDENTIFY_FLAG "__I__"      // Peripheral is identifying itself


#ifdef __cplusplus
}
#endif

#endif  // FLAGS_H