#include <Arduino.h>
#include "BLEDevice.h"
//#include "BLEScan.h"


// The remote service we wish to connect to.
static BLEUUID    serviceUUID("ba10");

// The characteristic of the remote service we are interested in.
static BLEUUID    checkUUID("ba52");

static BLEClient*  pClient;

static BLEScan* pBLEScan;

static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = true;

static BLEAdvertisedDevice* myDevice;

static BLERemoteCharacteristic* Check;

const byte RelayPin = 22;
const byte LEDPin = 21;

static void HODOR(){
  Serial.println("Opened");
  digitalWrite(RelayPin, HIGH);
  digitalWrite(LEDPin, HIGH);
  delay(7000);
  digitalWrite(LEDPin, LOW);
  digitalWrite(RelayPin, LOW);
  ESP.restart();
}


class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {
  }

  void onDisconnect(BLEClient* pclient) {
    Serial.println("Disconnected");
    ESP.restart();
  }
};

bool connectToServer() {

    Serial.println("Forming a connection to ");

    BLEClient*  pClient  = BLEDevice::createClient();

    Serial.println(" - Created client");


    // Connect to the remove BLE Server.
    pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
    Serial.println(" - Connected to server");

    // Obtain a reference to the service we are after in the remote BLE server.
    BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
    if (pRemoteService == nullptr) {
      Serial.print("Failed to find our service UUID: ");
      Serial.println(serviceUUID.toString().c_str());
      ESP.restart();
    }
    Serial.println(" - Found our service");


    // Obtain a reference to the characteristic in the service of the remote BLE server.
    Check = pRemoteService->getCharacteristic(checkUUID);

    if (Check == nullptr) {
      Serial.print("Failed to find our characteristic UUID: ");
      Serial.println(checkUUID.toString().c_str());
      ESP.restart();
    }
    Serial.println(" - Found our characteristic");


    Serial.println("Writing value of MAC to server");
    delay(200);
    Check->writeValue(BLEDevice::getAddress().toString().c_str(), BLEDevice::getAddress().toString().length());
    // Check->writeValue(BLEDevice::getAddress().toString().c_str());
    Serial.println("Done");
    HODOR();
    ESP.restart();
    doConnect = false;
}


/**
 * Scan for BLE servers and find the first one that advertises the service we are looking for.
 */
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
 /**
   * Called for each advertising BLE server.
   */
  void onResult(BLEAdvertisedDevice advertisedDevice) {

    Serial.print("BLE Advertised Device found: ");
    Serial.println(advertisedDevice.toString().c_str());
    // We have found a device, let us now see if it contains the service we are looking for.
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {

          int RSSIL = advertisedDevice.getRSSI();
          Serial.print("RSSI is: ");
          Serial.println(RSSIL);
          if (RSSIL >= -90) {
            Serial.println("Our device is near");
            BLEDevice::getScan()->stop();
            myDevice = new BLEAdvertisedDevice(advertisedDevice);
            doConnect = true;
            doScan = false;
            }
    } // Found our server
  } // onResult
}; // MyAdvertisedDeviceCallbacks


void setup() {
  Serial.begin(115200);

  pinMode(RelayPin, OUTPUT);
  pinMode(LEDPin, OUTPUT);

  digitalWrite(LEDPin, LOW);
  digitalWrite(RelayPin, LOW);
  
  Serial.println("Starting Arduino BLE Client application...");
  BLEDevice::init("");
} // End of setup.

// This is the Arduino main loop function.
void loop() {
  if (doConnect == true) {
    if (connectToServer()) {
      Serial.println("We are now connected to the BLE Server.");
    } else {
      Serial.println("We have failed to connect to the server; there is nothin more we will do.");
      ESP.restart();
    }
  }

  if (doScan == true){
    // Retrieve a Scanner and set the callback we want to use to be informed when we
    // have detected a new device.  Specify that we want active scanning and start the
    // scan to run for 1 second.
    BLEScan* pBLEScan = BLEDevice::getScan();
    pBLEScan->setInterval(1349);
    pBLEScan->setWindow(449);
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(true);
    pBLEScan->start(3, false);
  }
} // End of loop
