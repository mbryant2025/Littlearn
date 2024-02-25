# React/ESP32 Callbacks (over BLE)

For sending code from the React app to the ESP32:

`__SD__<CODE>__SD__`

The ESP32 will respond with:

`__SS__`

For stdout from the ESP32 (console printing):

`__P__<TEXT>__P__`

For errors from the ESP32:

`__ER__<TEXT>__ER__`


# Initial Setup of Peripheral Tiles

1. Brain will broadcast a QUERY message containing the mac address of itself
2. Tiles will recieve the QUERY message, storing the mac address of the brain. They will then respond with a TILE message containing their own mac address and TileType
3. Brain will recieve the TILE message, storing the mac address of the tile and the TileType in a map
   1. To prevent missed messages, the tile will send TILE messages (once every 100ms) until it recieves a targeted TILEACK message from the brain
   2. Will store the tiles as an ordered map (by mac address) to ensure that the tiles are always in the same order

This tile acknowledgment process will occur:
* Upon boot of brain
* Upon connection to BLE
* By pressing a dedicated button on the GUI