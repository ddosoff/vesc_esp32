/**
 * A BLE client example that is rich in capabilities.
 * There is a lot new capabilities implemented.
 * author unknown
 * updated by chegewara
 */

#include "Arduino.h"
//#include "BLEDevice.h"
//#include "BluetoothSerial.h"
#include <WiFi.h>
#include <WiFiAP.h>
//#include <User_Setups/Setup25_TTGO_T_Display.h>
#include <TFT_eSPI.h>
#include <Button2.h>
#include <EBYTE.h>
#include <CircularBuffer.h>
//#include <WebServer.h>

// The remote service we wish to connect to.
/*
static BLEUUID serviceUUID("6e400001-b5a3-f393-e0a9-e50e24dcca9e");
// The characteristic of the remote service we are interested in.
static BLEUUID charUUID("6e400003-b5a3-f393-e0a9-e50e24dcca9e");
static BLEUUID rxUUID("6e400002-b5a3-f393-e0a9-e50e24dcca9e");
*/

/*
static boolean doBleConnect = false;
static boolean bleConnected = false;
static boolean doScan = false;
static BLERemoteCharacteristic *pRemoteCharacteristic;
static BLERemoteCharacteristic *rxRemoteCharacteristic;
static BLEAdvertisedDevice *myDevice;
*/
#define ADC_EN 14
#define ADC_PIN 34
#define BUTTON_1 35
#define BUTTON_2 0

// EByte module connection
#define EB_M0 21
#define EB_M1 22
#define EB_RX 17
#define EB_TX 32
#define EB_AUX 33

// Vesc serial connection
#define VESC_RX 26
#define VESC_TX 25

EBYTE eb(&Serial2, EB_M0, EB_M1, EB_AUX);

TFT_eSPI tft = TFT_eSPI(135, 240); // Invoke custom library

Button2 btn1(BUTTON_1);
Button2 btn2(BUTTON_2);

const char *ssid = "vesc_extender";
const char *password = "12345678";
const int vescDefaultPort = 65102;

static WiFiServer server(vescDefaultPort);
static WiFiClient client;

// VESC produces about 800 bytes of configs data on vesc_tool connection
// We need to store it, while sending over slow LoRa chanel
static CircularBuffer<uint8_t, 2048> loraToSend;
const size_t max_buf = 2048;
uint8_t buf[max_buf];

/*
static void notifyCallback(
    BLERemoteCharacteristic *pBLERemoteCharacteristic,
    uint8_t *pData,
    size_t length,
    bool isNotify)
{
  if (client.connected())
    client.write(pData, length);
  Serial.print(length);
  Serial.print('*');
}

class MyClientCallback : public BLEClientCallbacks
{
  void onConnect(BLEClient *pclient)
  {
    Serial.println("onConnect");
  }

  void onDisconnect(BLEClient *pclient)
  {
    bleConnected = false;
    //doScan = true;
    Serial.println("onDisconnect");
  }
};

bool connectToBleUartServer()
{
  Serial.print("Forming a connection to ");
  Serial.println(myDevice->getAddress().toString().c_str());

  BLEClient *bleClient = BLEDevice::createClient();
  Serial.println(" - Created client");

  bleClient->setClientCallbacks(new MyClientCallback());

  // Connect to the remove BLE Server.
  bleClient->connect(myDevice); // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
  Serial.println(" - Connected to server");
  BLEDevice::setPower(ESP_PWR_LVL_P9);

  // Obtain a reference to the service we are after in the remote BLE server.
  BLERemoteService *pRemoteService = bleClient->getService(serviceUUID);
  if (pRemoteService == nullptr)
  {
    Serial.print("Failed to find our service UUID: ");
    Serial.println(serviceUUID.toString().c_str());
    bleClient->disconnect();
    return false;
  }
  Serial.println(" - Found our service");

  // Check remote device has RX characteristic
  rxRemoteCharacteristic = pRemoteService->getCharacteristic(rxUUID);
  if (rxRemoteCharacteristic == nullptr)
  {
    Serial.print("Failed to find BLE rx characteristic UUID: ");
    Serial.println(rxUUID.toString().c_str());
    bleClient->disconnect();
    return false;
  }
  Serial.println(" - Found our rx characteristic");

  if (rxRemoteCharacteristic->canRead())
  {
    std::string value = rxRemoteCharacteristic->readValue();
    Serial.print("The rx canRead() value was: ");
    Serial.println(value.c_str());
  }

  if (rxRemoteCharacteristic->canNotify())
  {
    Serial.println("rxUUID can notify");
    //rxCharacteristic->registerForNotify(notifyCallback);
  }

  // Check remote device has TX characteristic
  pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
  if (pRemoteCharacteristic == nullptr)
  {
    Serial.print("Failed to find our characteristic UUID: ");
    Serial.println(charUUID.toString().c_str());
    bleClient->disconnect();
    return false;
  }
  Serial.println(" - Found our tx characteristic");

  // Read the value of the characteristic.
  if (pRemoteCharacteristic->canRead())
  {
    std::string value = pRemoteCharacteristic->readValue();
    Serial.print("The characteristic value was: ");
    Serial.println(value.c_str());
  }

  if (pRemoteCharacteristic->canNotify())
  {
    Serial.println("charUUID can notify");
    pRemoteCharacteristic->registerForNotify(notifyCallback);
  }

  BLERemoteDescriptor *d = pRemoteCharacteristic->getDescriptor(BLEUUID((uint16_t)0x2902));
  if (d)
  {
    // Allow receive data notifications from Vesc
    const uint8_t notificationOn[] = {0x1, 0x0};
    d->writeValue((uint8_t *)notificationOn, 2, true);
    Serial.println("Wrote TXON");
  }

  bleConnected = true;
  return true;
}

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
  void onResult(BLEAdvertisedDevice advertisedDevice)
  {
    Serial.print("BLE Advertised Device found: ");
    Serial.println(advertisedDevice.toString().c_str());

    // We have found a device, let us now see if it contains the service we are looking for.
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID))
    {

      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doBleConnect = true;
      doScan = false;
      Serial.println("doBleConnect");
    } // Found our server
  }   // onResult
};    // MyAdvertisedDeviceCallbacks
*/

void initTDisplay()
{
  tft.init();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.setFreeFont(&FreeMono9pt7b);
}

class DisplayOffMessage
{
  int countDown;

public:
  DisplayOffMessage(int countDownStartsFrom = 10) : countDown(countDownStartsFrom) {}
  void step()
  {
    if (countDown < 0)
      return;
    else if (countDown > 0)
    {
      String s = String("Display off - ") + countDown;
      // Clear bottom line
      tft.setTextDatum(BC_DATUM);
      tft.setTextPadding(tft.textWidth(s) + 20);
      tft.drawString(s, tft.width() / 2, tft.height());
    }
    else
    {
      tft.fillScreen(TFT_BLACK); // Clear to reduce blinking on reset
      digitalWrite(TFT_BL, !TFT_BACKLIGHT_ON);
    }

    countDown--;
  }
} displayOff;

void initEBYTE()
{
  // EBYTE module always use 9600 Baud to read/set configuration
  Serial2.begin(9600, SERIAL_8N1, EB_TX, EB_RX);

  eb.init();
  Serial.println("Read EBYTE parametes with 9600 serial");
  eb.PrintParameters();
  eb.SetUARTBaudRate(UDR_115200);
  eb.SetAirDataRate(ADR_4800);
  eb.SaveParameters();
  Serial2.updateBaudRate(115200);
}

void eatEBYTEshit()
{
  // Module sends packet after initialization, read it to prevent forwarding to vesc
  const int shitBytes = 6;
  uint8_t buf[shitBytes];

  int len = Serial2.readBytes(buf, shitBytes);

  if (len == shitBytes)
    Serial.printf("EBYTE unwanted reply %d bytes: %02x:%02x:%02x:%02x:%02x:%02x\n", len, buf[0], buf[1], buf[2], buf[3], buf[4], buf[5]);
  else
    Serial.printf("EBYTE unwanted reply %d bytes!\n", len);
}

void setup()
{
  Serial.begin(115200);
  Serial.println("Starting Arduino BLE Client application...");

  // This shit will send first packet with his parameters instead of data
  initEBYTE();

  initTDisplay();

  // Vesc serial
  Serial1.begin(115200, SERIAL_8N1, VESC_TX, VESC_RX); // Start Vesc serial

  // WiFi Access Point
  WiFi.softAP(ssid, password, 2, 0, 4);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.begin();

  tft.drawString(String("WiFi: ") + ssid, 0, 0);
  tft.drawString(String("Pass: ") + password, 0, 20);
  tft.drawString(myIP.toString() + String(':') + vescDefaultPort, 0, 40);

  eatEBYTEshit();

  /*
  BLEDevice::init("Vesc Extender");

  // Retrieve a Scanner and set the callback we want to use to be informed when we
  // have detected a new device.  Specify that we want active scanning and start the
  // scan to run for 5 seconds.
  BLEScan *pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);
  */
} // End of setup.

// This is the Arduino main loop function.

void appendToLora(uint8_t *buf, size_t len)
{
  if (loraToSend.available() >= len) // Otherwise append to large enough LoRa buf
  {
    for (int i = 0; i < len; i++)
      loraToSend.push(buf[i]);
  }
  else // Drop packet, not enough buffer space
    Serial.printf("loraToSend BUFFER OVERFLOW! %d bytes available in buffer, %d received\n", loraToSend.available(), len);
}

static int loopStep = 0;

void debugPacket(uint8_t *buf, int len)
{
  Serial.print('(');
  for (int i = 0; i < len; i++)
    i == len - 1 ? Serial.printf("%02x", buf[i]) : Serial.printf("%02x:", buf[i]);
  Serial.println(")");
}

void loop()
{
  // If the flag "doBleConnect" is true then we have scanned for and found the desired
  // BLE Server with which we wish to connect.  Now we connect to it.  Once we are
  // connected we set the connected flag to be true.
  /*
  if (doBleConnect == true)
  {
    if (connectToBleUartServer())
    {
      Serial.println("We are now connected to the BLE Server.");
    }
    else
    {
      Serial.println("We have failed to connect to the server; there is nothin more we will do.");
    }
    doBleConnect = false;
  }
  
  // If we are connected to a peer BLE Server, update the characteristic each time we are reached
  // with the current time since boot.
  if (doScan)
  {
    Serial.println("doScan");
    BLEDevice::init("Vesc Extender");
    BLEScan *pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setInterval(1349);
    pBLEScan->setWindow(449);
    pBLEScan->setActiveScan(true);
    pBLEScan->start(5); // this is just example to start scan after disconnect, most likely there is better way to do it in arduino
  }
  */

  if (!(loopStep % 200000))
    displayOff.step();

  loopStep++;

  if (!client)
  {
    client = server.available();

    if (client)
    {
      client.setNoDelay(true);
      Serial.println();
      Serial.print(client.remoteIP());
      Serial.println(" connected!");
    }
  }

  size_t len, avail;

  // Wifi client data available?
  avail = client.available();
  if (avail)
  {
    len = client.readBytes(buf, avail < max_buf ? avail : max_buf);

    Serial.print(len);
    Serial.print('W');
    size_t written = Serial1.write(buf, len); // Send to Vesc
    Serial.print(written);
    Serial.print("V ");
  }

  // Vesc data available?
  avail = Serial1.available();
  if (avail)
  {
    len = Serial1.readBytes(buf, avail < max_buf ? avail : max_buf);
    Serial.print(len);
    Serial.print("V");
    if (client) // Send to WiFi, if connected
    {
      size_t written = client.write(buf, len);
      Serial.print(written);
      Serial.print("W ");

      if (written != len) // WiFi client seems lost
      {
        Serial.println();
        Serial.println("WiFi client lost, repeating to LoRa!");
        client.stop();

        // Accidentially append to lora
        appendToLora(buf, len);
      }
    }
    else
      appendToLora(buf, len);
  }

  // LoRa data available?
  avail = Serial2.available();
  if (avail)
  {
    len = Serial2.readBytes(buf, avail < max_buf ? avail : max_buf);
    Serial.print(len);
    Serial.print('L');
    size_t written = Serial1.write(buf, len); // Send to Vesc
    Serial.print(written);
    Serial.print("V ");

    debugPacket(buf, len);
  }

  int loraAvailableForWrite = Serial2.availableForWrite();
  if (loraToSend.size() && loraAvailableForWrite >= 58)
  {
    //Serial.printf("loraToSend.size() %d, loraAvailableForWrite %d\n", loraToSend.size(), loraAvailableForWrite);
    len = loraToSend.size() < loraAvailableForWrite ? loraToSend.size() : loraAvailableForWrite;
    bool isLoraModuleReady = digitalRead(EB_AUX) == HIGH;
    //Serial.printf("isLoraModuleReady: %d\n", isLoraModuleReady);

    if (isLoraModuleReady)
    {
      for (int i = 0; i < len; i++)
        buf[i] = loraToSend.shift();

      size_t written = Serial2.write(buf, len); // Send to Vesc
      Serial.print(written);
      Serial.print("L");

      debugPacket(buf, len);
    }
  }
  //delay(10);
} // End of loop
