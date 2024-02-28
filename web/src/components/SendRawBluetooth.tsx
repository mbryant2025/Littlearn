import React, { useState, useEffect } from 'react';
import { useBluetooth } from '../BluetoothContext';

function SendRawBluetooth() {
    const { bluetoothDevice, sendBluetoothData } = useBluetooth();
    const [inputData, setInputData] = useState('');

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
            <input
                type="text"
                placeholder={bluetoothDevice ? 'Send data to Bluetooth device' : 'Connect to Bluetooth device first'}
                value={inputData}
                onChange={(e) => setInputData(e.target.value)}
            />
            <button onClick={handleSendData}>Send Data</button>
        </div>
    );
}

export default SendRawBluetooth;