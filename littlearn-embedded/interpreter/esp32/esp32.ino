#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <Preferences.h>

#include <iostream>
#include <string>

#include "ast.hpp"
#include "error.hpp"
#include "interpreter.hpp"
#include "outputStream.hpp"
#include "tokenizer.hpp"

#define SERVICE_UUID "00001101-0000-1000-8000-00805f9b34fb"
#define CHARACTERISTIC_UUID "00001102-0000-1000-8000-00805f9b34fb"
#define LED_PIN 2

Preferences preferences;

BLEServer *pServer;
BLEService *pService;
BLECharacteristic *pCharacteristic;
bool deviceConnected = false;

std::string blocklyCode = "";

class BLEOutputStream : public OutputStream {
   public:
    void write(const std::string &message) override {
        if (deviceConnected) {
            pCharacteristic->setValue(message);
            pCharacteristic->notify();
        }
    }
};

OutputStream *outputStream = new BLEOutputStream;
ErrorHandler *errorHandler = new ErrorHandler(outputStream);

class MyCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
        std::string value = pCharacteristic->getValue();
        Serial.print("Callback found: ");
        Serial.println(value.c_str());

        if (value.length() > 0) {
            if (value.find("__SENDSCRIPT__") != std::string::npos) {
                // Halt the current execution
                errorHandler->triggerStopExecution();

                // Clear the value
                pCharacteristic->setValue("");

                // Get the code
                std::string code = value.substr(14, value.length() - 14 - 14);

                // Set the global blocklyCode variable
                blocklyCode = code;

                // Store the code in the preferences
                preferences.putString("code", code.c_str());

                // Send data to client
                pCharacteristic->setValue("__SCRIPTSENT__");
                pCharacteristic->notify();

                // Reset the stopExecution flag
                errorHandler->resetStopExecution();
            }
        }
    }
};

// Forward declaration
class MyServerCallbacks;

class MyServerCallbacks : public BLEServerCallbacks {
public:
    void onConnect(BLEServer *pServer);

    void onDisconnect(BLEServer *pServer);
};

void startBLE() {
    BLEDevice::init("Littlearn");
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    pService = pServer->createService(SERVICE_UUID);

    pCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_READ);
    pCharacteristic->setCallbacks(new MyCallbacks);
    pCharacteristic->addDescriptor(new BLE2902());
    pCharacteristic->setNotifyProperty(true);

    pService->start();
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(false);
    pAdvertising->start();
}

void stopBLE() {
    pServer->getAdvertising()->stop();  // Stop advertising
    pServer->removeService(pService);   // Remove the service from the server
    delete pService;                    // Delete the service object
    delete pCharacteristic;             // Delete the characteristic object
    BLEDevice::deinit(false);           // Deinitialize the Bluetooth device
}

void MyServerCallbacks::onConnect(BLEServer *pServer) {
    deviceConnected = true;
    Serial.println("Device connected");
    digitalWrite(LED_PIN, HIGH);
}

void MyServerCallbacks::onDisconnect(BLEServer *pServer) {
    deviceConnected = false;
    Serial.println("Device disconnected");
    digitalWrite(LED_PIN, LOW);

    stopBLE();

    startBLE();
}

void setup() {
    Serial.begin(115200);

    preferences.begin("littlearn", false);

    // If port 6 is high during this part, we will ignore the stored code
    pinMode(PORT_6, INPUT);

    if (digitalRead(PORT_6) != HIGH) {
        // If there is anything stored in the preferences, load it into the blocklyCode variable
        String tempCode = preferences.getString("code", "");

        if (tempCode != "") {
            blocklyCode = tempCode.c_str();
        }

        Serial.println("Code loaded from preferences:");
        Serial.println(blocklyCode.c_str());
    } else {
        Serial.println("Ignoring stored code");
    }

    // Initialize all pins as inputs
    pinMode(PORT_1, INPUT);
    pinMode(PORT_2, INPUT);
    pinMode(PORT_3, INPUT);
    pinMode(PORT_4, INPUT);
    pinMode(PORT_5, INPUT);
    pinMode(PORT_6, INPUT);

    // BLE connected indicator
    pinMode(LED_PIN, OUTPUT);

    startBLE();
}

void loop() {
    if (blocklyCode != "") {
        Tokenizer tokenizer(blocklyCode);

        // Tokenize the source code
        std::vector<Token> tokens = tokenizer.tokenize();

        // Create a Parser object
        Parser parser(tokens, outputStream, errorHandler);

        BlockNode *block = parser.parseProgram();

        // Create an Interpreter object
        Interpreter interpreter(block, outputStream, errorHandler);

        // Interpret the AST
        interpreter.interpret();

        // If anywhere we failed, reset the blocklyCode variable
        if (errorHandler->shouldStopExecution()) {
            blocklyCode = "";
        }
    }
}