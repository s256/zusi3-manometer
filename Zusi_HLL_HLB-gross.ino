/*
* Author:  Sebastian Wolf
* Created:  August 2018
*/

#include <Arduino.h>
#include "Zusi3Schnittstelle.h"
#include "SwitecX25.h"

// standard X25.168 range 315 degrees at 1/3 degree steps
#define STEP_BIG (880) // BIG GAUGE METER

//#define STEP_SMALL (355)
// For motors connected to digital pins 4,5,6,7
SwitecX25 motor_yellow(STEP_BIG, 16, 17, 18, 19); // SMALL GAUGE METER
SwitecX25 motor_red(STEP_BIG, 32, 33, 26, 27);	  // SMALL GAUGE METER
float foo;
//Bitte die #define der Zusi3Schnittstelle.h nutzen
#if defined(ESP8266_Wifi) || defined(ESP32_Wifi)

const char *ssid = "InternetOfShit";
const char *password = "Atlas2012";
#endif
#ifdef ESP32_Ethernet
//nothing
#endif
#ifdef Ethernet_Shield //Arduino Uno hat zu wenig RAM für Datenpakete
byte *mac = new byte[6]{0x8c, 0x16, 0x45, 0x82, 0xfc, 0x72};
#endif
#ifdef AVR_Wifi
const char *ssid = "SSID";
const char *password = "PASSWORD";
#endif

Zusi3Schnittstelle *zusi;

void setup()
{
	Serial.begin(115200);
	Serial.println("Go!");

	motor_yellow.zero();
	motor_red.zero();

#if defined(ESP8266_Wifi) || defined(ESP32_Wifi)
	Serial.print("Verbinde mit ");
	Serial.println(ssid);

	WiFi.mode(WIFI_STA);
	WiFi.begin(ssid, password);

	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
		Serial.print(".");
	}

	Serial.println("\nVerbunden");
	Serial.print("IP-Adresse: ");
	Serial.println(WiFi.localIP());
#endif

	zusi = new Zusi3Schnittstelle("192.168.0.2", 1436, "ESP32-HLL-HLB");
	zusi->reqFstAnz(Druck_Hauptluftleitung);
	zusi->reqFstAnz(Druck_Hauptluftbehaelter);
	zusi->requestFuehrerstandsbedienung(false);
	zusi->requestProgrammdaten(false);
	uint32_t i = 0;
	while (!zusi->connect())
	{
		Serial.print("Verbindung zu Zusi fehlgeschlagen (");
		Serial.print(++i);
		Serial.println(")");
		delay(1000);
	}
	Serial.println("Verbunden mit Zusi");
}

void loop()
{

	Node *node = zusi->update();
	if (node != NULL)
	{
		for (int i = 0; i < node->getNodes()->size(); i++)
		{
			Node *subNode = node->getNodes()->get(i);
			if (subNode->getIDAsInt() == 0x0A)
			{
				for (int j = 0; j < subNode->getAttribute()->size(); j++)
				{
					Attribute *attr = subNode->getAttribute()->get(j);
					if (attr->getIDAsInt() == Druck_Hauptluftleitung)
					{
						motor_yellow.setPosition((int)(attr->getDATAAsFloat() * STEP_BIG) / 12);
						motor_yellow.updateBlocking();
						foo = ((attr->getDATAAsFloat() * STEP_BIG) / 12);
						Serial.print("Druck_Hauptluftleitung: ");
						Serial.print((attr->getDATAAsFloat()));
						Serial.println(" bar");
						Serial.print("Steps: ");
						Serial.println(foo);
					}
				}
				for (int j = 0; j < subNode->getAttribute()->size(); j++)
				{
					Attribute *attr = subNode->getAttribute()->get(j);
					if (attr->getIDAsInt() == Druck_Hauptluftbehaelter)
					{
						foo = ((attr->getDATAAsFloat() * STEP_BIG) / 12);
						motor_red.setPosition((int)(attr->getDATAAsFloat() * STEP_BIG) / 12);
						motor_red.updateBlocking();
						Serial.print("Druck_Hauptluftbehaelter: ");
						Serial.print(attr->getDATAAsFloat());
						Serial.println(" bar");
						Serial.print("Steps: ");
						Serial.println(foo);
					}
				}
			}
		}
	}
}
