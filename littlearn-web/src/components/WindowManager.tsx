import React, { useState } from 'react';
import BlocklyComponent from './Blockly';
import BluetoothConsole from './BluetoothConsole';
import LeftBar from './LeftBar';
import './styles/WindowManager.css';


function WindowManager() {
    const [isConsoleVisible, setConsoleVisibility] = useState(false);

    const toggleConsoleVisibility = () => {
        setConsoleVisibility(!isConsoleVisible);
    };

    return (
        <div>
            <div className="main-divider">
                <div className="left-bar">
                    <LeftBar toggleConsoleVisibility={toggleConsoleVisibility}/>
                </div>
                <div className={`main-area ${isConsoleVisible ? 'show-right-column' : ''}`}>
                    <div className="blockly-area">
                        <BlocklyComponent />
                    </div>
                    {isConsoleVisible &&
                        <div className="console-area">
                            <BluetoothConsole />
                        </div>
                    }
                </div>
            </div>
        </div>
    );
}

export default WindowManager;
