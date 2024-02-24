# React/ESP32 Callbacks (over BLE)

For sending code from the React app to the ESP32:

`__SENDSCRIPT__<CODE>__SENDSCRIPT__`

The ESP32 will respond with:

`__SCRIPTSENT__`

For stdout from the ESP32 (console printing):

`__PRINT__<TEXT>__PRINT__`

For errors from the ESP32:

`__ERROR__<TEXT>__ERROR__`