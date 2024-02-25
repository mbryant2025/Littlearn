#ifndef RADIO_H
#define RADIO_H

#include "tile_types.h"

#ifdef __cplusplus
extern "C" {
#endif

void radio_init(void (*write_cb)(char*, uint16_t, uint8_t*));

void radio_broadcast(const char *data, uint16_t len);

void radio_send(const char *data, uint16_t len, uint8_t *addr);

#ifdef __cplusplus
}
#endif

#endif // RADIO_H
