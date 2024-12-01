#include <Arduino.h>
#include <FastLED.h>
#include <ArduinoBLE.h>

#define NUM_LEDS 16
#define DATA_PIN 14

bool Running = false;
CRGB leds[NUM_LEDS];

BLEService Service("19B10000-E8F2-537E-4F6C-D104768A1214"); // Bluetooth® Low Energy LED Service

// Bluetooth® Low Energy LED Switch Characteristic - custom 128-bit UUID, read and writable by central
BLEByteCharacteristic ToggleChara("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite | BLENotify | BLEIndicate);
BLEByteCharacteristic ModeChara("d02a2828-2b77-4685-89f0-2b9f0ba38da5", BLERead | BLEWrite);

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
  BLEDescriptor desc("2901", "Enabled Status");
  ToggleChara.addDescriptor(desc);
  Service.addCharacteristic(ToggleChara);
  Service.addCharacteristic(ModeChara);

  // add service
  BLE.addService(Service);

  // set the initial value for the characeristic:
  ToggleChara.writeValue(0);
  ModeChara.writeValue(0);

  // start advertising
  BLE.advertise();

  Serial.println("BLE LED Peripheral");
}

void ToggleValue(byte value)
{
  Serial.print("value ");
  if (value)
  {
    Serial.println("(on)");
    leds[0] = CRGB::White;
  }
  else
  {
    Serial.println("(off)");
    leds[0] = CRGB::Black;
  }
  FastLED.show();
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
      if (ToggleChara.written())
      {
        Serial.print("Toggle: ");
        ToggleValue(ToggleChara.value());
      }

      if (ModeChara.written())
      {
        Serial.print("Mode: ");
        Serial.println(ModeChara.value());
      }

      if (Serial.available())
      {
        byte buffer[32] = {};
        Serial.readBytes(buffer, 32);
        byte v = buffer[0];
        ToggleChara.writeValue(v);
        ToggleValue(v);
      }
    }

    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }
}