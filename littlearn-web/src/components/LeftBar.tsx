import React, { useEffect } from 'react';
import { useBluetooth } from '../BluetoothContext';
import { SendScript } from '../SendScript';


import './styles/LeftBar.css';

const LeftBar: React.FC = () => {

    const { bluetoothDevice, connectToDevice, disconnectDevice } = useBluetooth();

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

    const sendScript = SendScript();
    const uploadBlockly = async () => {
        try {
            // Add your asynchronous logic here
            const element = document.getElementById('generatedCode');
            const script = "{" + element?.innerHTML?.replace(/<br>/g, '').replace(/&gt;/g, '>') + "}";
            const response = await sendScript.sendData(script);
    
            // Handle the response if needed
            console.log('Upload Blockly success:', response);
        } catch (error) {
            console.error('Error during Blockly upload:', error);
        }
    };
    


    useEffect(() => {
        if (bluetoothDevice) {
            console.log('Connected to:', bluetoothDevice.name);
        } else {
            console.log('Not connected to any Bluetooth device.');
        }
    }, [bluetoothDevice]);

    const warnNeedConnect = () => {
        if (!bluetoothDevice) {
            alert("Please connect to device first!");
            return true;
        }
        return false;
    }




    return (
        <div>

            <div className="logo">
                Littlearn
            </div>

            <div className="button-container" onClick={bluetoothDevice ? handleDisconnect : handleConnect}>
                <img src="./ble.png" alt="BLE" className="icon" />
                {bluetoothDevice ? 'Disconnect' : 'Connect'}
            </div>

            <div className="button-container" onClick={bluetoothDevice ? uploadBlockly : warnNeedConnect}>
                <img src="./upload.png" alt="BLE" className="icon" />
                Upload Code
            </div>




        </div>
    );
}

export default LeftBar;









