#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define SERVICE_UUID        "00001101-0000-1000-8000-00805f9b34fb" // Replace with your service UUID
#define CHARACTERISTIC_UUID "00001102-0000-1000-8000-00805f9b34fb" // Replace with your characteristic UUID
#define LED_PIN             2 // GPIO pin connected to the LED

BLEServer* pServer;
BLEService* pService;
BLECharacteristic* pCharacteristic;
bool deviceConnected = false;

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
    Serial.println("Device connected");
  }

  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
    Serial.println("Device disconnected");
  }
};

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);

  BLEDevice::init("LightSwitch");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_WRITE
  );
  pCharacteristic->addDescriptor(new BLE2902());

  pService->start();
  BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->start();
  Serial.println("Bluetooth peripheral started. Waiting for connections...");
}

void loop() {
  if (deviceConnected) {
    // Turn on the LED when the device is connected
    digitalWrite(LED_PIN, HIGH);
  } else {
    // Turn off the LED when the device is disconnected
    digitalWrite(LED_PIN, LOW);
  }
  delay(1000);
}
