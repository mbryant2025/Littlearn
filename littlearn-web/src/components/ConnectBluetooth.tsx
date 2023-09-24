import React, { useState } from 'react';

function ConnectBluetooth() {
  const [bluetoothDevice, setBluetoothDevice] = useState<BluetoothDevice | null>(null);
  const [outputText, setOutputText] = useState<string>('');

  const handleConnect = async () => {
    try {
      const device = await navigator.bluetooth.requestDevice({
        filters: [{ services: ['00001101-0000-1000-8000-00805f9b34fb'] }],
      });

      console.log('Connected to:', device.name);

      const server = await device.gatt?.connect();
      console.log('Connected to GATT server');

      const service = await server?.getPrimaryService('00001101-0000-1000-8000-00805f9b34fb');
      console.log('Service discovered');

      const char = await service?.getCharacteristic('00001101-0000-1000-8000-00805f9b34fb');
      console.log('Characteristic discovered');

      if (char) {
        // Enable notifications for the characteristic to receive data
        await char.startNotifications();

        // Listen for data notifications
        char.addEventListener('characteristicvaluechanged', (event) => {
          const value = (event.target as BluetoothRemoteGATTCharacteristic)?.value;
          if (value) {
            const textDecoder = new TextDecoder('utf-8');
            const decodedValue = textDecoder.decode(value);
            setOutputText((prevOutput) => prevOutput + decodedValue);
          }
        });

        setBluetoothDevice(device);
      } else {
        // Handle the case when the characteristic is not found
        setBluetoothDevice(null);
      }
    } catch (error) {
      console.error('Bluetooth error:', error);
    }
  };

  const handleDisconnect = async () => {
    if (bluetoothDevice && bluetoothDevice.gatt?.connected) {
      await bluetoothDevice.gatt?.disconnect();
      console.log('Disconnected');
    }

    setBluetoothDevice(null);
    setOutputText('');
  };

  return (
    <div>
      <h1>ESP32 Bluetooth Communication</h1>
      <button onClick={handleConnect} disabled={bluetoothDevice !== null}>
        Connect to ESP32
      </button>
      <button onClick={handleDisconnect} disabled={!bluetoothDevice}>
        Disconnect
      </button>
      <div id="output">{outputText}</div>
    </div>
  );
}

export default ConnectBluetooth;
