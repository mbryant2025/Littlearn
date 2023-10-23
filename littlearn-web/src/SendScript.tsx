import { useBluetooth } from './BluetoothContext';

export function SendScript() {
    const { sendBluetoothData } = useBluetooth();

    // Hardcoded script header and footer
    const scriptHeader = "";
    const scriptFooter = "";

    const sendData = async (dataToSend: string) => {
        try {
            await sendBluetoothData(scriptHeader + dataToSend + scriptFooter);
        } catch (error) {
            console.error('Error sending data to Bluetooth device:', error);
        }
    };

    return {
        sendData,
    };
}
