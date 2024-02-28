import React, { useState } from 'react';
import BlocklyComponent from './Blockly';
import BluetoothConsole from './BluetoothConsole';
import TextCode from './TextCode';
import LeftBar from './LeftBar';
import './styles/WindowManager.css';


function WindowManager() {
    const [isConsoleVisible, setConsoleVisibility] = useState(true);
    const [isTextCodeVisible, setTextCodeVisibility] = useState(true);

    const [blocklyKey, setBlocklyKey] = useState(0); // Key to force reload BlocklyComponent

    const reloadBlockly = () => {
        setBlocklyKey((prevKey) => prevKey + 1); // Change the key value to force re-render BlocklyComponent
    };

    const toggleConsoleVisibility = () => {
        setConsoleVisibility(!isConsoleVisible);
        reloadBlockly();
    };

    const toggleTextCodeVisibility = () => {
        setTextCodeVisibility(!isTextCodeVisible);
        reloadBlockly();
    };

    return (
        <div>
            <div className="main-divider">
                <div className="left-bar">
                    <LeftBar toggleConsoleVisibility={toggleConsoleVisibility} toggleTextCodeVisibility={toggleTextCodeVisibility} />
                </div>
                <div className={`main-area ${(isConsoleVisible || isTextCodeVisible) ? 'show-right-column' : ''}`}>
                    <div className="blockly-area">
                        <BlocklyComponent key={blocklyKey} /> {/* Set key to force re-render */}
                    </div>


                    {isConsoleVisible && isTextCodeVisible && <div className="split-vertically">

                        <div className="text-code">
                            <TextCode />
                        </div>

                        <div className="bluetooth-console">
                            <BluetoothConsole />
                        </div>

                    </div>}

                    {isConsoleVisible && !isTextCodeVisible && <div className="bluetooth-console">
                        <BluetoothConsole />
                    </div>}

                    {!isConsoleVisible && isTextCodeVisible && <div className="text-code">
                        <TextCode />
                    </div>}

                </div>
            </div>
        </div>
    );
}

export default WindowManager;


/*<div className="console-area">
                            <BluetoothConsole />
                        </div>*/
