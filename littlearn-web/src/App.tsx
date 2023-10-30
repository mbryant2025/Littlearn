import React from 'react';
import { BluetoothProvider } from './BluetoothContext';
import WindowManager from './components/WindowManager';
import { GeneratedCodeProvider } from './GeneratedCodeContext';

const App: React.FC = () => {
  return (
    <div>
      <BluetoothProvider>
        <GeneratedCodeProvider>
          <WindowManager />
        </GeneratedCodeProvider>
      </BluetoothProvider>
    </div>
  );
}

export default App;