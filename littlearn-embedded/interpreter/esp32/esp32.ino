#include <iostream>
#include <string>

#include "tokenizer.hpp"
#include "ast.hpp"
#include "interpreter.hpp"
#include "error.hpp"

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#include <Wire.h>
#include <EEPROM.h>

#define EEPROM_ADDRESS 0x50   // 24LC256 I2C address
#define MAX_STRING_LENGTH 500 // Maximum length of the string you want to store
#define PAGE_SIZE 32        // EEPROM page size (AT24C256 has 32-byte pages)

#define SERVICE_UUID "00001101-0000-1000-8000-00805f9b34fb"
#define CHARACTERISTIC_UUID "00001102-0000-1000-8000-00805f9b34fb"
#define LED_PIN 2

BLEServer *pServer;
BLEService *pService;
BLECharacteristic *pCharacteristic;
bool deviceConnected = false;

std::string blocklyCode = "";

//EEPROM address to write to
int eepromAddress = 0;

// Write a string to EEPROM
void writeString(int address, const std::string &str)
{

    Serial.print("Writing to EEPROM: ");
    Serial.println(str.c_str());

    String s = "{print_seven_segment(69);}";

    int length = str.length();

    for (int i = 0; i < length; i++)
    {
        EEPROM.write(address, s[i]);
        address++;
    }

    EEPROM.write(address, '\0'); // Null-terminate the string
}

std::string readString(int address)
{
    char readChar = '\0';
    std::string readStringResult = "";

    while (true)
    {
        readChar = EEPROM.read(address);

        if (readChar == '\0') // Null-terminator found
        {
            break;
        }

        readStringResult += readChar;
        address++;
    }

    return readStringResult;
}


class MyCallbacks : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *pCharacteristic)
  {
    std::string value = pCharacteristic->getValue();
    Serial.print("Callback found: ");
    Serial.println(value.c_str());

    if (value.length() > 0)
    {

      if (value.find("__SENDSCRIPT__") != std::string::npos)
      {

        // Halt the current execution
        triggerStopExecution();

        // Clear the value
        pCharacteristic->setValue("");

        // Get the code
        std::string code = value.substr(14, value.length() - 14 - 14);

        // Set the global blocklyCode variable
        blocklyCode = code;

        // Write the code to EEPROM
        writeString(eepromAddress, code);

        // Send data to client
        std::string dataToSend = "__SCRIPTSENT__";
        pCharacteristic->setValue(dataToSend);
        pCharacteristic->notify();

        // Reset the stopExecution flag
        resetStopExecution();
      }
    }
  }
};

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

  // Initialize all pins as outputs such that we can use them as ground pins until we need them 
  // Pins 32 33 25 26 27 14
  pinMode(32, OUTPUT);
  pinMode(33, OUTPUT);
  pinMode(25, OUTPUT);
  pinMode(26, OUTPUT);
  pinMode(27, OUTPUT);
  pinMode(14, OUTPUT);

  Serial.begin(115200);

  // Initialize EEPROM with SCL on pin 22 and SDA on pin 21
  Wire.begin(21, 22);
  blocklyCode = readString(eepromAddress);

  Serial.println("Read from EEPROM:");
  Serial.println(blocklyCode.c_str());

  // If the EEPROM does not start with { and end with }, replace with empty string
  // TODO try to tokenize the string and see if it is valid
  if (blocklyCode[0] != '{' || blocklyCode[blocklyCode.length() - 1] != '}')
  {
    blocklyCode = "";
    Serial.println("No valid code found in EEPROM");
  }
  else {
    Serial.println("Found valid code in EEPROM");
    Serial.println(blocklyCode.c_str());
  }

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

void loop()
{

  if (deviceConnected)
  {

    digitalWrite(LED_PIN, HIGH);

    if (blocklyCode != "")
    {

      Tokenizer tokenizer(blocklyCode);

      // Tokenize the source code
      std::vector<Token> tokens = tokenizer.tokenize();

      // Create a Parser object
      Parser parser(tokens);

      BlockNode *block = parser.parseProgram();

      // Create an Interpreter object
      Interpreter interpreter(block);

      // Interpret the AST
      interpreter.interpret();
    }
  }
  else
  {
    digitalWrite(LED_PIN, LOW);
  }
}