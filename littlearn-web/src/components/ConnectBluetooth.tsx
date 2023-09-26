import React, { useState, useEffect } from 'react';
import { useBluetooth } from '../BluetoothContext';

function ConnectBluetooth() {
  const { bluetoothDevice, outputText, connectToDevice, disconnectDevice, sendBluetoothData } = useBluetooth();
  const [inputData, setInputData] = useState('');

  const handleConnect = async () => {
    try {
      await connectToDevice();
    } catch (error) {
      console.error('Error connecting to Bluetooth device:', error);
    }
  };

  const handleDisconnect = async () => {
    try {
      await disconnectDevice();
    } catch (error) {
      console.error('Error disconnecting from Bluetooth device:', error);
    }
  };

  const handleSendData = async () => {
    try {
      if (inputData) {
        await sendBluetoothData(inputData);
        setInputData('');
      }
    } catch (error) {
      console.error('Error sending data to Bluetooth device:', error);
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
      <button onClick={handleConnect} disabled={bluetoothDevice !== null}>
        Connect to Bluetooth Device
      </button>
    </div>
  );
}

export default ConnectBluetooth;
