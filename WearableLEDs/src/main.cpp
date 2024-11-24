#include <Arduino.h>
#include <FastLED.h>
#include <ArduinoBLE.h>

#define NUM_LEDS 16
#define DATA_PIN 14

bool Running = false;
CRGB leds[NUM_LEDS];

BLEService Service("19B10000-E8F2-537E-4F6C-D104768A1214"); // Bluetooth® Low Energy LED Service

// Bluetooth® Low Energy LED Switch Characteristic - custom 128-bit UUID, read and writable by central
BLEByteCharacteristic Chara("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
BLEByteCharacteristic Chara1("d02a2828-2b77-4685-89f0-2b9f0ba38da5", BLERead | BLEWrite);
BLEDescriptor Descript("2901", "NameTest");

void setup()
{
  Serial.begin(115200);
  while (!Serial)
    ;

  // set LED pin to output mode
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);

  // begin initialization
  if (!BLE.begin())
  {
    Serial.println("starting Bluetooth® Low Energy module failed!");

    while (1)
      ;
  }

  // set advertised local name and service UUID:
  BLE.setLocalName("LED Control");
  BLE.setAdvertisedService(Service);

  // add the characteristic to the service
  Chara.addDescriptor(Descript);
  Service.addCharacteristic(Chara);
  Service.addCharacteristic(Chara1);

  // add service
  BLE.addService(Service);

  // set the initial value for the characeristic:
  Chara.writeValue(0);
  Chara1.writeValue(0);

  // start advertising
  BLE.advertise();

  Serial.println("BLE LED Peripheral");
}

void loop()
{
  // listen for Bluetooth® Low Energy peripherals to connect:
  BLEDevice central = BLE.central();

  // if a central is connected to peripheral:
  if (central)
  {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());

    // while the central is still connected to peripheral:
    while (central.connected())
    {
      // if the remote device wrote to the characteristic,
      // use the value to control the LED:
      if (Chara.written())
      {
        Serial.print("Option: ");
        Serial.println(Chara.value());

        if (Chara.value())
        { 
          // any value other than 0
          Serial.println(" (on)");
          leds[0] = CRGB::White;
          FastLED.show();
        }
        else
        { 
          // a 0 value
          Serial.println(" (off)");
          leds[0] = CRGB::Black;
          FastLED.show();
        }
      }

      if (Chara1.written())
      {
        Serial.print("Option: ");
        Serial.println(Chara.value());
      }
    }

    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }
}