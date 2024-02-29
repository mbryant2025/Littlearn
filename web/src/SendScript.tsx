import { useBluetooth } from './BluetoothContext';

export function SendScript() {
    const { sendBluetoothData } = useBluetooth();

    const sendData = async (dataToSend: string) => {
        try {
            await sendBluetoothData(dataToSend);
        } catch (error) {
            console.error('Error sending data to Bluetooth device:', error);
        }
    };

    return {
        sendData,
    };
}
