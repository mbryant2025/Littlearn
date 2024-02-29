import React, { useEffect } from 'react';
import { useBluetooth } from '../BluetoothContext';
import './styles/Console.css';

function BluetoothConsole() {
    const { bluetoothDevice, outputText, clearOutput } = useBluetooth();

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
            <div onClick={clearOutput} className="clear-button">
                Clear
            </div>
            {/* This is to display output text while keeping newlines */}
            <div>
                {outputText.split('\n').map((line, i) => {
                    const className = line.includes('Error') ? 'error' : line.includes('Bluetooth') ? 'ble' : '';
                    const formattedLine = line.replace(/&gt;/g, '>').replace(/&lt;/g, '<');
                    return <div key={i} className={className}>{formattedLine}</div>
                })}
            </div>

            


        </div>
    );
}

export default BluetoothConsole;