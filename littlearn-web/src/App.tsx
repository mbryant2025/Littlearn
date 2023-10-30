import React from 'react';
import { BluetoothProvider } from './BluetoothContext';
import WindowManager from './components/WindowManager';


const App: React.FC = () => {
  return (
    <div>
    <BluetoothProvider>
      <WindowManager />
    </BluetoothProvider>
    </div>
  );
}

export default App;