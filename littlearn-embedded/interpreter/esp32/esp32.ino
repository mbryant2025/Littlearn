#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <EEPROM.h>

#include <iostream>
#include <string>

#include "ast.hpp"
#include "error.hpp"
#include "interpreter.hpp"
#include "outputStream.hpp"
#include "tokenizer.hpp"

#define MAX_STRING_LENGTH 500  // Maximum length of the string you want to store
#define PAGE_SIZE 32           // EEPROM page size (AT24C256 has 32-byte pages)

#define SERVICE_UUID "00001101-0000-1000-8000-00805f9b34fb"
#define CHARACTERISTIC_UUID "00001102-0000-1000-8000-00805f9b34fb"
#define LED_PIN 2

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

                // Send data to client
                std::string dataToSend = "__SCRIPTSENT__";
                pCharacteristic->setValue(dataToSend);
                pCharacteristic->notify();

                // Reset the stopExecution flag
                errorHandler->resetStopExecution();
            }
        }
    }
};

class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer *pServer) {
        deviceConnected = true;
        Serial.println("Device connected");
        digitalWrite(LED_PIN, HIGH);
    }

    void onDisconnect(BLEServer *pServer) {
        deviceConnected = false;
        Serial.println("Device disconnected");
        digitalWrite(LED_PIN, LOW);
    }
};

void setup() {
    // Initialize all pins as outputs such that we can use them as ground pins until we need them
    // Pins 32 33 25 26 27 14
    pinMode(32, OUTPUT);
    pinMode(33, OUTPUT);
    pinMode(25, OUTPUT);
    pinMode(26, OUTPUT);
    pinMode(27, OUTPUT);
    pinMode(14, OUTPUT);

    Serial.begin(115200);

    Serial.println(blocklyCode.c_str());

    // BLE connected indicator
    pinMode(LED_PIN, OUTPUT);

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

void loop() {
    if (blocklyCode != "") {
        Tokenizer tokenizer(blocklyCode);

        // Tokenize the source code
        std::vector<Token> tokens = tokenizer.tokenize();

        // Create a Parser object
        Parser parser(tokens, outputStream);

        BlockNode *block = parser.parseProgram();

        // Create an Interpreter object
        Interpreter interpreter(block, outputStream);

        // Interpret the AST
        interpreter.interpret();

        // If anywhere we failed, reset the blocklyCode variable
        if (errorHandler->shouldStopExecution()) {
            blocklyCode = "";
        }
    }
}