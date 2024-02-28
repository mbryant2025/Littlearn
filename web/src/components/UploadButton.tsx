import { SendScript } from '../SendScript';
import React from 'react';


function UploadButton() {

    //temp script
    const script = "Never gonna give you up, never gonna let you down, never gonna run around and desert you."

    const sendScript = SendScript();

    const handleClick = (e: React.MouseEvent<HTMLButtonElement, MouseEvent>) => {
        e.preventDefault();
        sendScript.sendData(script);
    }

    return (
        <div>
            <button onClick={handleClick}
                disabled={script === null}>
                Upload Script
            </button>
        </div>
    );
}

export default UploadButton;