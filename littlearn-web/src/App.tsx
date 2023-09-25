import React from 'react';
import { BluetoothProvider } from './BluetoothContext';
import ConnectBluetooth from './components/ConnectBluetooth';

const App: React.FC = () => {
  return (
    <div>
    <BluetoothProvider>
      <h1>ESP32 Bluetooth Communication</h1>
      <ConnectBluetooth />
    </BluetoothProvider>
    </div>

  );
}

export default App;