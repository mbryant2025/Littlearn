import React, { useEffect } from 'react';
import { useBluetooth } from '../BluetoothContext';
import './styles/Console.css';

function BluetoothConsole() {
    const { bluetoothDevice, outputText } = useBluetooth();

    useEffect(() => {
        if (bluetoothDevice) {
            console.log('Connected to:', bluetoothDevice.name);
        } else {
            console.log('Not connected to any Bluetooth device.');
        }
    }, [bluetoothDevice]);

    return (
        <div className="console">
            <div className="title">Console</div>
            <div>{outputText}</div>
        </div>
    );
}

export default BluetoothConsole;