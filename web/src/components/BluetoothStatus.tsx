import React, { useState, useEffect } from 'react';
import { useBluetooth } from '../BluetoothContext';

function BluetoothStatus() {
  const { bluetoothDevice, outputText, connectToDevice, disconnectDevice, sendBluetoothData } = useBluetooth();


  useEffect(() => {
    if (bluetoothDevice) {
      console.log('Connected to:', bluetoothDevice.name);
    } else {
      console.log('Not connected to any Bluetooth device.');
    }
  }, [bluetoothDevice]);

  return (
    <div>
      <p>Bluetooth Status: {bluetoothDevice ? 'Connected to ' + bluetoothDevice.name : 'Disconnected'}</p>
    </div>
  );
}

export default BluetoothStatus;
