#include <iostream>
#include <string>

#include "tokenizer.hpp"
#include "ast.hpp"
#include "interpreter.hpp"

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define SERVICE_UUID "00001101-0000-1000-8000-00805f9b34fb"
#define CHARACTERISTIC_UUID "00001102-0000-1000-8000-00805f9b34fb"
#define LED_PIN 2

BLEServer *pServer;
BLEService *pService;
BLECharacteristic *pCharacteristic;
bool deviceConnected = false;

std::string rick_roll[] = {
    "Never",
    "gonna",
    "give",
    "you",
    "up",
    "Never",
    "gonna",
    "let",
    "you",
    "down"};

int rick_roll_index = 0;

class MyServerCallbacks : public BLEServerCallbacks
{
  void onConnect(BLEServer *pServer)
  {
    deviceConnected = true;
    Serial.println("Device connected");
  }

  void onDisconnect(BLEServer *pServer)
  {
    deviceConnected = false;
    Serial.println("Device disconnected");
  }
};

void setup()
{
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);

  BLEDevice::init("Littlearn");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  pService = pServer->createService(SERVICE_UUID);

  pCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_READ);
  pCharacteristic->addDescriptor(new BLE2902());
  pCharacteristic->setNotifyProperty(true);

  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->start();
}

void loop()
{

  if (deviceConnected)
  {
    digitalWrite(LED_PIN, HIGH);

    // Receive data from client
    std::string value = pCharacteristic->getValue();

    if (value.length() > 0)
    {
      Serial.print("Received Value: ");
      for (int i = 0; i < value.length(); i++)
      {
        Serial.print(value[i]);
      }
      Serial.println();

      Tokenizer tokenizer(value);

      // Tokenize the source code
      std::vector<Token> tokens = tokenizer.tokenize();

      // Create a Parser object
      Parser parser(tokens);

      BlockNode *block = parser.parseProgram();

      // Create an Interpreter object
      Interpreter interpreter(block);

      // Interpret the AST
      interpreter.interpret();

      // Send data to client
      std::string dataToSend = rick_roll[rick_roll_index++];
      pCharacteristic->setValue(dataToSend);
      pCharacteristic->notify();

      // Clear the value
      pCharacteristic->setValue("");
    }
  }
  else
  {
    digitalWrite(LED_PIN, LOW);
  }
  digitalWrite(LED_PIN, LOW);
}