// All the different tile types
// Important: to adapt for potentially new tile types, we will define the types by if
// they are sources or sinks and by the type of data they handle

#ifndef TILE_TYPES_H
#define TILE_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    SOURCE_BOOL,    // Ex. a button
    SOURCE_INT,     // Ex. a temperature sensor with only integer precision
    SOURCE_FLOAT,   // Ex. a temperature sensor with decimal precision
    SOURCE_STRING,  // Ex. a keypad or speech recognition

    SINK_BOOL,      // Ex. a light  
    SINK_INT,       // Ex. a LED bar graph, index into possible sounds for a speaker
    SINK_FLOAT,     // Ex. a motor with speed control
    SINK_STRING,    // Ex. a character display
} TileType;

// For communication over radio
const char* tile_type_to_string(TileType type) {
    switch(type) {
        case SOURCE_BOOL:
            return "SB";
        case SOURCE_INT:
            return "SI";
        case SOURCE_FLOAT:
            return "SF";
        case SOURCE_STRING:
            return "SS";
        case SINK_BOOL:
            return "KB";
        case SINK_INT:
            return "KI";
        case SINK_FLOAT:
            return "KF";
        case SINK_STRING:
            return "KS";
    }

    return "UNKNOWN"; // Should never happen
}

TileType string_to_tile_type(const char* type) {
    // For efficiency, since we cannot use a switch statement with strings, order them
    // roughly from most likely to least likely
    if(strcmp(type, "KB") == 0) {
        return SINK_BOOL;
    } else if(strcmp(type, "KI") == 0) {
        return SINK_INT;
    } else if(strcmp(type, "SB") == 0) {
        return SOURCE_BOOL;
    } else if(strcmp(type, "SI") == 0) {
        return SOURCE_INT;
    } else if(strcmp(type, "SF") == 0) {
        return SOURCE_FLOAT;
    } else if(strcmp(type, "KF") == 0) {
        return SINK_FLOAT;
    } else if(strcmp(type, "KS") == 0) {
        return SINK_STRING;
    } else if(strcmp(type, "SS") == 0) {
        return SOURCE_STRING;
    }

    return (TileType) -1; // Should never happen
}

#ifdef __cplusplus
}
#endif

#endif  // TILE_TYPES_H