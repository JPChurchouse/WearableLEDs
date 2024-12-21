#include "main.hpp"

void setup()
{
  Serial.begin(115200);
  while (!Serial)
    ;

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
    LEDs[0] = CRGB::White;
  }
  else
  {
    Serial.println("(off)");
    LEDs[0] = CRGB::Black;
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