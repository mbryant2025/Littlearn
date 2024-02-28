import React, { useEffect } from 'react';
import { useBluetooth } from '../BluetoothContext';

function DisconnectBluetooth() {
  const { bluetoothDevice, outputText, connectToDevice, disconnectDevice, sendBluetoothData } = useBluetooth();

  const handleDisconnect = async () => {
    try {
      await disconnectDevice();
    } catch (error) {
      console.error('Error disconnecting from Bluetooth device:', error);
    }
  };

  useEffect(() => {
    if (bluetoothDevice) {
      console.log('Connected to:', bluetoothDevice.name);
    } else {
      console.log('Not connected to any Bluetooth device.');
    }
  }, [bluetoothDevice]);

  return (
    <div>
      <button onClick={handleDisconnect} disabled={bluetoothDevice === null}>
        Disconnect from Bluetooth Device
      </button>
    </div>
  );
}

export default DisconnectBluetooth;
