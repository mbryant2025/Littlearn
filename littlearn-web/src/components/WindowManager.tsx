import React, { useState } from 'react';
import BlocklyComponent from './Blockly';
import BluetoothConsole from './BluetoothConsole';
import TextCode from './TextCode';
import LeftBar from './LeftBar';
import './styles/WindowManager.css';


function WindowManager() {
    const [isConsoleVisible, setConsoleVisibility] = useState(false);
    const [isTextCodeVisible, setTextCodeVisibility] = useState(true);

    const toggleConsoleVisibility = () => {
        setConsoleVisibility(!isConsoleVisible);
    };

    const toggleTextCodeVisibility = () => {
        setTextCodeVisibility(!isTextCodeVisible);
    };

    return (
        <div>
            <div className="main-divider">
                <div className="left-bar">
                    <LeftBar toggleConsoleVisibility={toggleConsoleVisibility} toggleTextCodeVisibility={toggleTextCodeVisibility} />
                </div>
                <div className={`main-area ${(isConsoleVisible || isTextCodeVisible) ? 'show-right-column' : ''}`}>
                    <div className="blockly-area">
                        <BlocklyComponent />
                    </div>


                    {isConsoleVisible && isTextCodeVisible && <div className="split-vertically">

                        <TextCode />
                        <BluetoothConsole />

                    </div>}

                    {isConsoleVisible && !isTextCodeVisible && <BluetoothConsole />}

                    {!isConsoleVisible && isTextCodeVisible && <TextCode />}

                </div>
            </div>
        </div>
    );
}

export default WindowManager;


/*<div className="console-area">
                            <BluetoothConsole />
                        </div>*/
