/*
  Copyright (c) 2019 Andy England
  CustomLitt Laptop
*/

#include <ArtnetWifi.h>/**Artnet settings**/
#include <WiFi.h>
#include <WiFiUdp.h>

ArtnetWifi artnet;
WiFiUDP UdpSend;

int startUniverse = 0;
int endUniverse = 0;
int startSlot = 0;
bool sendFrame = 1;
int previousDataLength = 0;

//Wifi settings
String ssid = "dummy";
String password = "dummy";
IPAddress local_IP(192, 168, 1, 4);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(8, 8, 8, 8);   //optional
IPAddress secondaryDNS(8, 8, 4, 4);
#define NUM_CHANNELS NUM_LEDS * 3 // Total number of channels you want to receive (1 led = 3 channels)
#define NUM_UNIVERSES NUM_LEDS / 170

void initArtnet()
{
  bool connectionState = connectWifi();
  if (connectionState == true)
  {
    artnet.setArtDmxCallback(onDmxFrame);
    artnet.begin();
  }
}

void artRead ()
{
  artnet.read();
}

bool connectWifi(void) //Sets our ESP32 device up as an access point
{
  boolean state = false;
  //WiFi.mode(WIFI_AP_STA);
  //state = WiFi.softAP(ssid, password);
  //Comment out the above two lines and uncomment the below line to connect to an existing network specified on lines 8 and 9
  /*if (state)
    {
    ssid = getSSID();
    password = getPassword();
    }*/

  ssid = getSSID();
  password = getPassword();
  Serial.print("SSID: ");
  Serial.println(ssid);
  Serial.print("Password: ");
  Serial.println(password);
  state = WiFi.begin(ssid.c_str(), "figureitoutdude");
  Serial.println(WiFi.localIP());
  state = getWiFiOkay();
  return state;
}

void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data)
{
  sendFrame = 1;
  int ledOffset = universe * 170;
  for (int led = ledOffset; led < ledOffset + (length / 3); led++)
  {
    {
      int offset = ((led - ledOffset) * 3) + startSlot;
      leds[led] = CRGB(data[offset], data[offset + 1], data[offset + 2]);
    }
  }
  if (universe == endUniverse) //Display our data if we have received all of our universes, prevents incomplete frames when more universes are concerned.
  {
    FastLED.show();
    UdpSend.flush();
  }
}
