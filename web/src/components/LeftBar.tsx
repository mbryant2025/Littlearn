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
            // writeToOutput('Disconnected from Bluetooth device.\n');
        } catch (error) {
            console.error('Error disconnecting from Bluetooth device:', error);
        }
    };

    const sendScript = SendScript();

    const uploadBlockly = async () => {
        try {
            const code = generatedCode.replace(/&lt;/g, '<').replace(/&gt;/g, '>').replace(/<br>/g, '\n');
            const script = "__SD__{" + code + "}__SD__"; // wrap in curly braces to make it a script per the interpreter
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

    // Prompt the user for text input
    // TEMP TODO REMOVE
    const uploadText = async () => {
        const text = prompt("Please enter your text");
        try {
            if (!text) {
                return;
            }
            const code = text.replace(/&lt;/g, '<').replace(/&gt;/g, '>').replace(/<br>/g, '\n');
            const script = "__SD__{" + code + "}__SD__"; // wrap in curly braces to make it a script per the interpreter
            const response = await sendScript.sendData(script);
            writeToOutput('Uploading code.\n');

            console.log('Upload Blockly success:', response);
        } catch (error) {
            console.error('Error during code upload:', error);
            writeToOutput('Error during code upload.\n');
        }
    }

    const buttons = [
        {
            imgSrc: './ble.png',
            alt: 'BLE',
            text: bluetoothDevice ? 'Disconnect' : 'Connect',
            onClick: bluetoothDevice ? handleDisconnect : handleConnect,
        },
        {
            imgSrc: './upload.png',
            alt: 'Upload',
            text: 'Upload Code',
            onClick: bluetoothDevice ? uploadBlockly : warnNeedConnect,
        },
        {
            imgSrc: './text.png',
            alt: 'Console',
            text: 'Show Text Code',
            onClick: toggleTextCodeVisibility,
        },
        {
            imgSrc: './console.png',
            alt: 'Console',
            text: 'Show Console',
            onClick: toggleConsoleVisibility,
        },
        {
            imgSrc: './upload.png',
            alt: 'Upload',
            text: 'TEMP',
            onClick: bluetoothDevice ? uploadText : warnNeedConnect,
        },
    ];


    return (

        <div>
            <div className="logo">Electro-Tiles</div>
            {buttons.map((button, index) => (
                <div key={index} className="button-container" onClick={button.onClick}>
                    <img src={button.imgSrc} alt={button.alt} className="icon" />
                    {button.text}
                </div>
            ))}
        </div>
    );
}

export default LeftBar;