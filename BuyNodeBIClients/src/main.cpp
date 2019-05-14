#include <Arduino.h>
#include "BLEDevice.h"
#include <WiFi.h>
#include <HTTPClient.h>
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


const char* ssid     = "cleverest.tech_5G";
const char* password = "Robotics1sTheBest";


static void HODOR(){
  Serial.println("Door is opened");
  delay(7000);
}

String convertCstr2String(std::string msg1){
  String output = "";
  for (int b = 0; b < msg1.length(); b++){
        output[b] = msg1[b];
      }
  return output;
}

void send_signal(std::string msg1){
  String msg = convertCstr2String(msg1);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
  HTTPClient http;
  Serial.print("Request is: ");
  Serial.println("http://35.204.205.60:7777/buynode/" + msg);
  http.begin("http://35.204.205.60:7777/buynode/" + msg); //Specify destination for HTTP request
  http.addHeader("Content-Type", "text/plain"); //Specify content-type header
  int httpResponseCode = http.GET(); //Send the actual POST request
  Serial.print("Responce is: ");
  Serial.println(httpResponseCode);
  http.end(); //Free resources
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
    std::string value;
    if(Check->canRead()) {
      value = Check->readValue();
      Serial.print("The characteristic value was: ");
      Serial.println(value.c_str());
    }

    send_signal(value);
    HODOR();

    Serial.println("Done");
    ESP.restart();
}



void setup() {
  Serial.begin(115200);
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
