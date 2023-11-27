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
            {/* This is to display output text while keeping newlines */}
            <div className="output">
                {outputText.split('\n').map((line, i) => {
                    const className = line.startsWith('Error') ? 'error' : '';
                    return <div key={i} className={className}>{line}</div>
                })}
            </div>

        </div>
    );
}

export default BluetoothConsole;