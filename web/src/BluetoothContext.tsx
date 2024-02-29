import React, { createContext, useContext, useState, useCallback, ReactNode, useEffect } from 'react';

interface BluetoothContextProps {
  bluetoothDevice: BluetoothDevice | null;
  outputText: string;
  connectToDevice: () => Promise<void>;
  disconnectDevice: () => Promise<void>;
  sendBluetoothData: (data: string) => Promise<void>;
  writeToOutput: (data: string) => void;
  clearOutput: () => void;
}

const BluetoothContext = createContext<BluetoothContextProps | undefined>(undefined);

const PrintFlag = '__P__';
const PrintFlagLength = PrintFlag.length;

const ErrorFlag = '__ER__';
const ErrorFlagLength = ErrorFlag.length;

const ScriptSentFlag = '__SS__';

function uuid_bytes_to_string(uuid: number[]): string {
  // Assumes LSB first
  const uuid_str = uuid.reverse().map(byte => byte.toString(16).padStart(2, '0')).join('');
  return [
    uuid_str.slice(0, 8),
    uuid_str.slice(8, 12),
    uuid_str.slice(12, 16),
    uuid_str.slice(16, 20),
    uuid_str.slice(20),
  ].join('-');
}

const SERVICE_UUID = uuid_bytes_to_string([
  0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80,
  0x00, 0x10, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
]);

const CHARACTERISTIC_UUID = '0000ff01-0000-1000-8000-00805f9b34fb';

interface BluetoothProviderProps {
  children: ReactNode;
}

export const BluetoothProvider: React.FC<BluetoothProviderProps> = ({ children }) => {
  const [bluetoothDevice, setBluetoothDevice] = useState<BluetoothDevice | null>(null);
  const [outputText, setOutputText] = useState<string>('');

  // For writing to the output text
  const writeToOutput = useCallback((data: string) => {
    setOutputText((prevOutput) => prevOutput + '\n' + data);
    console.log("Output: " + data);
  }, [setOutputText]);

  const clearOutput = useCallback(() => {
    setOutputText('');
  }, [setOutputText]);

  const handleDisconnected = useCallback(() => {
    console.log('Disconnected from GATT server');
    const message = 'Disconnected from Bluetooth device.\n';
    // If the outputText does not end with the message, append it
    if (!outputText.endsWith(message)) {
      writeToOutput(message);
    }
    
    setBluetoothDevice(null);
  }, [writeToOutput, outputText]);

  const connectToDevice = useCallback(async () => {

    console.log('Requesting Bluetooth Device...');

    try {

      if (!navigator.bluetooth) {
        console.error('Web Bluetooth API is not available.\n');
        return;
      }

      // If the device is already connected, disconnect it
      if (bluetoothDevice) { return; }

      const device = await navigator.bluetooth.requestDevice({
        filters: [{ services: [SERVICE_UUID] }],
      });

      console.log('Connected to:', device.name);

      const server = await device.gatt?.connect();
      console.log('Connected to GATT server');

      // Event listener for disconnection
      device.addEventListener('gattserverdisconnected', handleDisconnected);

      const service = await server?.getPrimaryService(SERVICE_UUID);
      console.log('Service discovered');

      const char = await service?.getCharacteristic(CHARACTERISTIC_UUID);
      console.log('Read Characteristic discovered');

      console.log(char);

      if (char) {
        // Enable notifications for the characteristic to receive data
        await char.startNotifications();
        console.log('Notifications started for read characteristic');

        // Listen for data notifications
        char.addEventListener('characteristicvaluechanged', (event: any) => {
          console.log('Received data:', event.target?.value);
          const value = event.target?.value;
          if (value) {
            const textDecoder = new TextDecoder('utf-8');
            const decodedValue = textDecoder.decode(value);

            // Decode the callbacks per callbacks.md

            // __SS__
            if (decodedValue === ScriptSentFlag) {
              writeToOutput('Script uploaded.\n');
            }

            // __P__<TEXT>__P__
            else if (decodedValue.startsWith(PrintFlag) && decodedValue.endsWith(PrintFlag)) {
              const text = decodedValue.slice(PrintFlagLength, decodedValue.length - PrintFlagLength);
              writeToOutput(text + '\n');
            }

            // __ER__<TEXT>__ER__
            else if (decodedValue.startsWith(ErrorFlag) && decodedValue.endsWith(ErrorFlag)) {
              const text = decodedValue.slice(ErrorFlagLength, decodedValue.length - ErrorFlagLength);
              writeToOutput('Error: ' + text + '\n');
            }
          }
        });

        setBluetoothDevice(device);
        writeToOutput('Connected to Bluetooth device.\n');
      } else {
        // Handle the case when the characteristic is not found
        setBluetoothDevice(null);
      }
    } catch (error) {
      console.error('Bluetooth error:', error);
    }
  }, [bluetoothDevice, handleDisconnected, writeToOutput]);

  const disconnectDevice = useCallback(async () => {
    if (bluetoothDevice && bluetoothDevice.gatt?.connected) {
      bluetoothDevice.gatt?.disconnect();
      console.log('Disconnected');
    }

    console.log('Disconnected from:', bluetoothDevice?.name);

    setBluetoothDevice(null);
  }, [bluetoothDevice]);

  const sendBluetoothData = useCallback(async (data: string) => {
    if (bluetoothDevice) {
        const textEncoder = new TextEncoder();
        const encodedData = textEncoder.encode(data);
        
        // Update the characteristic UUID to match the one used in your ESP32 code
        const service = await bluetoothDevice.gatt?.getPrimaryService(SERVICE_UUID);
        const writeChar = await service?.getCharacteristic(CHARACTERISTIC_UUID);

        console.log('Sending data:', data);

        if (writeChar) {
            try {
                await writeChar.writeValue(encodedData);
                console.log('Write successful');
            } catch (error) {
                console.error('Write error:', error);
            }
        }
    }
  }, [bluetoothDevice]);

  return (
    <BluetoothContext.Provider
      value={{
        bluetoothDevice,
        outputText,
        connectToDevice,
        disconnectDevice,
        sendBluetoothData,
        writeToOutput,
        clearOutput,
      }}
    >
      {children}
    </BluetoothContext.Provider>
  );
};

export const useBluetooth = (): BluetoothContextProps => {
  const context = useContext(BluetoothContext);
  if (context === undefined) {
    throw new Error('useBluetooth must be used within a BluetoothProvider');
  }
  return context;
};

