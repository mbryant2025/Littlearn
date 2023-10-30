import React from 'react';
import { BluetoothProvider } from './BluetoothContext';
import BlocklyComponent from './components/Blockly';
import LeftBar from './components/LeftBar';


const App: React.FC = () => {
  return (
    <div>
    <BluetoothProvider>
      <div className="main-divider">
        <div className="left-bar">
          <LeftBar />
        </div>
        <div className="main-area">
          <BlocklyComponent />
        </div>
      </div>
    </BluetoothProvider>
    </div>

  );
}

export default App;