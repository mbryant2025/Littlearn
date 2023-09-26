import React, { useState, useEffect } from 'react';
import { useBluetooth } from '../BluetoothContext';

function BluetoothConsole() {
    const { bluetoothDevice, outputText, connectToDevice, disconnectDevice, sendBluetoothData } = useBluetooth();
    const [inputData, setInputData] = useState('');

    useEffect(() => {
        if (bluetoothDevice) {
            console.log('Connected to:', bluetoothDevice.name);
        } else {
            console.log('Not connected to any Bluetooth device.');
        }
    }, [bluetoothDevice]);

    return (
        <div>
            <h3>Received Data:</h3>
            <div>{outputText}</div>
        </div>
    );
}

export default BluetoothConsole;