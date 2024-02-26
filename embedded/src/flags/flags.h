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

#define TILE_COMMAND_FLAG "__TC__" // Brain is sending a command (data) to a tile Ex. __TC__1__TC__ if the data is boolean true


#define TILE_REQUEST_FLAG "__TR__" // Brain is requesting a tile's data
#define TILE_DATA_FLAG "__TD__"    // Peripheral is sending tile data to the brain


#ifdef __cplusplus
}
#endif

#endif  // FLAGS_H