#include <ESP8266WiFi.h>
#include "ESPAsyncUDP.h"

// const char *ssid = "server";
const char *ssid = "client";
const char *password = "12345678";

AsyncUDP udp;
/**/
#define Static_IP
/**/
#ifdef Static_IP
// Set your Static IP address
IPAddress local_IP(192, 168, 10, 100);
// Set your Gateway IP address
IPAddress gateway(192, 168, 1, 1);
/**/
IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(8, 8, 8, 8);   //optional
IPAddress secondaryDNS(8, 8, 4, 4); //optional
#endif
void setup()
{
  Serial.begin(9600);

#ifdef Static_IP
  WiFi.mode(WIFI_STA);
  // Configures static IP address
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS))
  {
    Serial.println("STA Failed to configure");
  }
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.println("WiFi Failed");
    while (1)
    {
      delay(1000);
    }
  }
#else
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password); //Create Access point
#endif

  if (udp.listen(1234))
  {
    Serial.print("UDP Listening on IP: ");
    Serial.println(WiFi.localIP());
    udp.onPacket([](AsyncUDPPacket packet) {
      Serial.print("UDP Packet Type: ");
      Serial.print(packet.isBroadcast() ? "Broadcast" : packet.isMulticast() ? "Multicast" : "Unicast");
      Serial.print(", From: ");
      Serial.print(packet.remoteIP());
      Serial.print(":");
      Serial.print(packet.remotePort());
      Serial.print(", To: ");
      Serial.print(packet.localIP());
      Serial.print(":");
      Serial.print(packet.localPort());
      Serial.print(", Length: ");
      Serial.print(packet.length());
      Serial.print(", Data: ");
      Serial.write(packet.data(), packet.length());
      Serial.println();
      //reply to the client
      packet.printf("Got %u bytes of data", packet.length());
    });
  }
}

void loop()
{
  delay(1000);
  //Send broadcast
  udp.broadcast("Anyone here?");
}
