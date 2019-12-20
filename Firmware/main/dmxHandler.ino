#include <ArtnetWifi.h>/**Artnet settings**/
#include <WiFi.h>
#include <WiFiUdp.h>

ArtnetWifi artnet;
WiFiUDP UdpSend;

#define NUM_CHANNELS NUM_LEDS * 3 // Total number of channels you want to receive (1 led = 3 channels)
#define NUM_UNIVERSES NUM_LEDS / 170

char ssid[32] = "Dummy";
char password[32] = "dummypassword";

int startUniverse = 0;
int startSlot = 0;
int endUniverse = startUniverse + ((startSlot + NUM_CHANNELS) / 512);
bool sendFrame = 1;
int previousDataLength = 0;

void initArtnet()
{
  artnet.setArtDmxCallback(onDmxFrame);
  artnet.begin();
}

void artRead ()
{
  artnet.read();
}

bool connectWifi(void) //Sets our ESP32 device up as an access point
{
  boolean state = false;
  state = getWiFiOkay();
  if (state)
  {
    getSSID(ssid);
    getPassword(password);
    Serial.print("SSID: ");
    Serial.print(ssid);
    Serial.println(".");
    Serial.print("Password: ");
    Serial.print(password);
    Serial.println(".");
  }
  char finalSSID[32] = "Black Panther";
  char finalPassword[32] = "figureitoutdude";
  Serial.print("finalSSID: ");
  Serial.print(finalSSID);
  Serial.println(".");
  Serial.print("finalPassword: ");
  Serial.print(finalPassword);
  Serial.println(".");
  if (strcmp(finalSSID, ssid) == 0)
  {
    Serial.println("SSID Same");
  }
  else
  {
    Serial.println("SSID different");
  }
  if (strcmp(finalPassword, password) == 0)
  {
    Serial.println("Password Same");
  }
  else
  {
    Serial.println("Password different");
  }
  WiFi.begin(finalSSID, finalPassword);
  uint8_t counter = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    state = true;
    counter++;
    delay(250);
    Serial.print(".");
    if (counter > 20)
    {
      state = false;
      return state;
    }
  }
  Serial.println(WiFi.localIP());
  return state;
}

void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data)
{
  sendFrame = 1;
  //Read universe and put into the right part of the display buffer
  //DMX data should be sent with the first LED in the string on channel 0 of Universe 0
  int ledOffset = universe * 170;
  for (int led = ledOffset; led < ledOffset + (length / 3); led++)
  {
    int offset = ((led - ledOffset) * 3) + startSlot;
    leds[led] = CRGB(data[offset], data[offset + 1], data[offset + 2]);
  }
  previousDataLength = length;
  if (universe == endUniverse) //Display our data if we have received all of our universes, prevents incomplete frames when more universes are concerned.
  {
    FastLED.show();
    UdpSend.flush();
  }
}
