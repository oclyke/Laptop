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

void deinitArtnet()
{
  WiFi.disconnect();
}

void artRead ()
{
  artnet.read();
}

void WiFiEvent(WiFiEvent_t event){
  IPAddress null_ip;
  null_ip.fromString(String("0.0.0.0"));
  
  switch(event) {
    case SYSTEM_EVENT_STA_GOT_IP:
      Serial.println("WiFi event: got ip!");
      Serial.println(WiFi.localIP());
      
      wifiStatus = true;
      storeWiFiStatus(wifiStatus);
      
      BLE.setIPAddress(WiFi.localIP());  
      BLE.setNetworkStatus(wifiStatus);
      break;
      
    case SYSTEM_EVENT_STA_DISCONNECTED:
      Serial.println("WiFi event: disconnected");
      
      wifiStatus = false;
      storeWiFiStatus(wifiStatus);
      BLE.setNetworkStatus(wifiStatus);
      BLE.setIPAddress(null_ip); 
      break;
      
    default:
      Serial.print("Unknown WiFi Event: (");
      Serial.print(event);
      Serial.println(")");
      break;
  }
}

bool connectWifi(void) //Sets our ESP32 device up as an access point
{
  boolean state = false;

  Serial.println("Connecting to WiFi: ");
  Serial.print("SSID: ");
  Serial.println(ssid);
  Serial.print("Password: ");
  Serial.println(password);

  const unsigned int c_ssid_len = 33;
  char c_ssid[c_ssid_len];
  ssid.toCharArray(c_ssid, c_ssid_len);

  const unsigned int c_pass_len = 33;
  char c_pass[c_pass_len];
  password.toCharArray(c_pass, c_pass_len);
  
  WiFi.onEvent(WiFiEvent);
  state = WiFi.begin(c_ssid, c_pass);
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
