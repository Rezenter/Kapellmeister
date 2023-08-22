/*
nano v3 328P

dependency:
StepperDriver by Laurentiu Badea https://github.com/laurb9/StepperDriver

To-do:
get to required position always from the left
*/
#include <SPI.h>
#include <Ethernet.h>

//ethernet
const byte mac[] = {0xA4, 0x33, 0x44, 0x92, 0x9F, 0x9B};
IPAddress ip(192, 168, 10, 70);
#define RESET 9
EthernetUDP Udp;
#define localPort 8888      // local port to listen on
char currentBuffer[6]; //buffer to hold incoming packet,
int packetSize = 0;
char totalBuffer[6];


void setup() {
  pinMode(RESET, OUTPUT);
  digitalWrite(RESET, LOW);
  delay(1000);
  digitalWrite(RESET, HIGH);
  delay(1000);
  Ethernet.init(10);
  Ethernet.begin(mac, ip);
  Udp.begin(localPort);
}

void loop() {
    // rotate 360 degrees. That's it.
    //axis1.rotate(360);

  Udp.parsePacket();
  packetSize = Udp.available();
  if(packetSize){
    // read the packet into packetBuffer
    Udp.read(currentBuffer, 6);
    //Serial.println(packetBuffer);
    //axis1.rotate(360);

    /*
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());

    Udp.write("hello");

    Udp.endPacket();
    */
 }
}
