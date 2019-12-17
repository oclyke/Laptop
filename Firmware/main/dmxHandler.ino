#include <WiFi.h>
#include <WiFiUdp.h>
#include <ArtnetWifi.h>/**Artnet settings**/

WiFiUDP UdpSend;
ArtnetWifi artnet;

int startUniverse = 0;
int startSlot = 0;
bool sendFrame = 1;
int previousDataLength = 0;

//Wifi settings
char *ssid;
char *password;
IPAddress local_IP(10, 0, 0, 2);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(8, 8, 8, 8);   //optional
IPAddress secondaryDNS(8, 8, 4, 4);
#define NUM_CHANNELS NUM_LEDS * 3 // Total number of channels you want to receive (1 led = 3 channels)
#define NUM_UNIVERSES NUM_LEDS / 170

void initArtnet()
{
  artnet.begin();
  artnet.setArtDmxCallback(onDmxFrame);
}

void artRead ()
{
  artnet.read();
}

boolean connectWiFi(void)
{
  boolean state = true;
  int i = 0;
  ssid = getSSID();
  password = getPassword();
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS))
  {
    Serial.println("STA Failed to configure");
  }

  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println("Connecting to WiFi");

  // Wait for connection
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    if (i > 20) {
      state = false;
      break;
    }
    i++;
  }
  return state;
}

void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data)
{
  sendFrame = 1;
  // read universe and put into the right part of the display buffer
  for (int i = 0; i < length / 3; i++)
  {
    int led = i + (universe - startUniverse) * (previousDataLength / 3);
    leds[led] = CRGB(data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
  }
  previousDataLength = length;
}
