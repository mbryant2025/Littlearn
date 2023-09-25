import React, { createContext, useContext, useState, useCallback, ReactNode } from 'react';

interface BluetoothContextProps {
  bluetoothDevice: BluetoothDevice | null;
  outputText: string;
  connectToDevice: () => Promise<void>;
  disconnectDevice: () => Promise<void>;
  sendBluetoothData: (data: string) => Promise<void>;
}

const BluetoothContext = createContext<BluetoothContextProps | undefined>(undefined);

interface BluetoothProviderProps {
  children: ReactNode;
}

export const BluetoothProvider: React.FC<BluetoothProviderProps> = ({ children }) => {
  const [bluetoothDevice, setBluetoothDevice] = useState<BluetoothDevice | null>(null);
  const [outputText, setOutputText] = useState<string>('');

  const connectToDevice = useCallback(async () => {
    try {
      const device = await navigator.bluetooth.requestDevice({
        filters: [{ services: ['00001101-0000-1000-8000-00805f9b34fb'] }],
      });

      console.log('Connected to:', device.name);

      const server = await device.gatt?.connect();
      console.log('Connected to GATT server');

      const service = await server?.getPrimaryService('00001101-0000-1000-8000-00805f9b34fb');
      console.log('Service discovered');

      const char = await service?.getCharacteristic('00001102-0000-1000-8000-00805f9b34fb');
      console.log('Characteristic discovered');

      console.log(char);

      if (char) {
        // Enable notifications for the characteristic to receive data
        await char.startNotifications();
        console.log('Notifications started');

        // Listen for data notifications
        char.addEventListener('characteristicvaluechanged', (event: any) => {
          const value = event.target?.value;
          if (value) {
            const textDecoder = new TextDecoder('utf-8');
            const decodedValue = textDecoder.decode(value);
            setOutputText((prevOutput) => prevOutput + decodedValue);
          }
        });

        setBluetoothDevice(device);
      } else {
        // Handle the case when the characteristic is not found
        setBluetoothDevice(null);
      }
    } catch (error) {
      console.error('Bluetooth error:', error);
    }


  }, []);

  const disconnectDevice = useCallback(async () => {
    if (bluetoothDevice && bluetoothDevice.gatt?.connected) {
      bluetoothDevice.gatt?.disconnect();
      console.log('Disconnected');
    }

    console.log('Disconnected from:', bluetoothDevice?.name);

    setBluetoothDevice(null);
    setOutputText('');
  }, [bluetoothDevice]);

  const sendBluetoothData = useCallback(async (data: string) => {
    if (bluetoothDevice) {
      const textEncoder = new TextEncoder();
      const encodedData = textEncoder.encode(data);
      const service = await bluetoothDevice.gatt?.getPrimaryService('00001101-0000-1000-8000-00805f9b34fb');
      const char = await service?.getCharacteristic('00001101-0000-1000-8000-00805f9b34fb');

      console.log('Sending data:', data);

      if (char) {
        await char.writeValue(encodedData);
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
