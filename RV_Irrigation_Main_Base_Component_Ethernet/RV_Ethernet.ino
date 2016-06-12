#include <SPI.h>
#include <Ethernet.h>

byte mac[] = {
   0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02
};

IPAddress ip( 192, 168, 0, 11 );
IPAddress gateway( 192, 168, 0, 1 );
IPAddress subnet( 255, 255, 255, 0 );
EthernetServer server(80);

//ethernet shield connection init
void initializeEthernet()
{
   Ethernet.begin(mac, ip);
   server.begin();
   //Serial.print("server is at ");
   //Serial.println(Ethernet.localIP());
}
