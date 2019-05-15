#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

BLEServer *pServer = NULL;
BLECharacteristic * pTxCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint8_t txValue = 0;
bool hodor = false;

const byte RelayPin = 22;
const byte LEDPin = 21;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID           "BA10" // UART service UUID
#define CHARACTERISTIC_UUID_RX "BA52"


void HODOR(){
  Serial.println("Opened");
  digitalWrite(RelayPin, HIGH);
  digitalWrite(LEDPin, HIGH);
  delay(7000);
  digitalWrite(LEDPin, LOW);
  digitalWrite(RelayPin, LOW);
  ESP.restart();
}


class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      ESP.restart();
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValue = pCharacteristic->getValue();

      if (rxValue.length() > 0) {
        hodor = true;
      }
    }
};


void setup() {
  Serial.begin(115200);

  pinMode(RelayPin, OUTPUT);
  pinMode(LEDPin, OUTPUT);

  digitalWrite(LEDPin, LOW);
  digitalWrite(RelayPin, LOW);

  // Create the BLE Device
  BLEDevice::init("BuyQaw");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);


  BLECharacteristic * pRxCharacteristic = pService->createCharacteristic(
											 CHARACTERISTIC_UUID_RX,
											BLECharacteristic::PROPERTY_WRITE
										);

  pRxCharacteristic->setCallbacks(new MyCallbacks());

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");
}

void loop() {
  delay(100);
  if(hodor){
    HODOR();
  }
}
