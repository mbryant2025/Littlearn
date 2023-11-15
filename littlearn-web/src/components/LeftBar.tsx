import React, { useEffect } from 'react';
import { useBluetooth } from '../BluetoothContext';
import { SendScript } from '../SendScript';
import './styles/LeftBar.css';
import { useGeneratedCode } from '../GeneratedCodeContext';

interface LeftBarProps {
    toggleConsoleVisibility: () => void;
    toggleTextCodeVisibility: () => void;
}

const LeftBar: React.FC<LeftBarProps> = ({ toggleConsoleVisibility, toggleTextCodeVisibility }) => {

    const { bluetoothDevice, connectToDevice, disconnectDevice, writeToOutput } = useBluetooth();
    const { generatedCode } = useGeneratedCode();

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
            writeToOutput('Disconnected from Bluetooth device.\n');
        } catch (error) {
            console.error('Error disconnecting from Bluetooth device:', error);
        }
    };

    const sendScript = SendScript();
    const uploadBlockly = async () => {
        try {
            const code = generatedCode.replace(/&lt;/g, '<').replace(/&gt;/g, '>').replace(/<br>/g, '\n');
            const script = "__SENDSCRIPT__{" + code + "}__SENDSCRIPT__"; // wrap in curly braces to make it a script per the interpreter
            const response = await sendScript.sendData(script);
            writeToOutput('Uploading code.\n');

            console.log('Upload Blockly success:', response);
        } catch (error) {
            console.error('Error during code upload:', error);
            writeToOutput('Error during code upload.\n');
        }
    };

    useEffect(() => {
        if (bluetoothDevice) {
            console.log('Connected to:', bluetoothDevice.name);
        } else {
            console.log('Not connected to any Bluetooth device.');
        }
    }, [bluetoothDevice, writeToOutput]);

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
                <img src="./upload.png" alt="Upload" className="icon" />
                Upload Code
            </div>

            <div className="button-container" onClick={toggleConsoleVisibility}>
                <img src="./console.png" alt="Console" className="icon" />
                Show Console
            </div>

            <div className="button-container" onClick={toggleTextCodeVisibility}>
                <img src="./text.png" alt="Console" className="icon" />
                Show Text Code
            </div>




        </div>
    );
}

export default LeftBar;









