import React from 'react';
import { BluetoothProvider } from './BluetoothContext';
import ConnectBluetooth from './components/ConnectBluetooth';
import BluetoothStatus from './components/BluetoothStatus';
import DisconnectBluetooth from './components/DisconnectBluetooth';
import SendRawBluetooth from './components/SendRawBluetooth';
import BluetoothConsole from './components/BluetoothConsole';
import UploadButton from './components/UploadButton';
import BlocklyComponent from './components/Blockly';

const App: React.FC = () => {
  return (
    <div>
    <BluetoothProvider>
      <h1>Littlearn</h1>
      <ConnectBluetooth />
      <DisconnectBluetooth />
      <BluetoothStatus />
      <SendRawBluetooth />
      <BluetoothConsole />
      <UploadButton />
      <BlocklyComponent />
    </BluetoothProvider>
    </div>

  );
}

export default App;