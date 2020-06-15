/*
* Zusi TCP Client to control pressure gauges
* Using two VID28 stepper motors with the SwitecX25 lib
*/

#include <Arduino.h>
#include "Zusi3Schnittstelle.h"
#include "SwitecX25.h"

/*	Enbable Test Mode for debugging
*		Simple test mode to debug motors, amount of steps and needle position
*		The test-mode will no connect to Zusi, it will just move the motors and test the LED
*		You need to define the maximum steps for the destined motor configuration like in production use.
*/
//#define TestMode

// Active the destined motor configuration

// Either the "big" gauge
#define Hauptluft

// or the small gauge
//#define Bremsdruckzylinder

// standard X25.168 range 315 degrees at 1/3 degree steps
#if !(defined(Hauptluft) && defined(Bremsdruckzylinder))

#if defined(Hauptluft)
// Defined Steps for the BIG gauge meter
#define STEP_RED (870)
#define STEP_YELLOW (880)

#else if defined(Bremsdruckzylinder)
// Defined Steps for the SMALL gauge meter
#define STEP_RED (810)
#define STEP_YELLOW (810)

#endif
#endif
//#define STEP_SMALL (355)
// For motors connected to digital pins 4,5,6,7
SwitecX25 motor_yellow(STEP_YELLOW, 16, 17, 18, 19);
SwitecX25 motor_red(STEP_RED, 32, 33, 26, 27);
float Steps;
const int ledPin = 5;
//Bitte die #define der Zusi3Schnittstelle.h nutzen
#if defined(ESP8266_Wifi) || defined(ESP32_Wifi)

const char *ssid = "WIFI-NAME";
const char *password = "SuperSecretPassword";
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
	pinMode(ledPin, OUTPUT);
#if defined(TestMode)
	Serial.print("Starting Test-Mode");
#else

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
#if defined(Hauptluft)
	zusi = new Zusi3Schnittstelle("192.168.0.2", 1436, "ESP32-HLL-HLB", 5);
#else if defined(Bremsdruckzylinder)
	zusi = new Zusi3Schnittstelle("192.168.0.2", 1436, "ESP32-Bremsdruckzylinder");
#endif
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
#endif
}

void loop()
{
#if defined(TestMode)
	Serial.println("Set needle to zero and disable LED");
	digitalWrite(ledPin, LOW); // turn on the LED
	motor_yellow.zero();
	motor_red.zero();
	delay(500);
	Serial.println("Set needle to half of maximum steps and enable LED");
	digitalWrite(ledPin, HIGH); // turn on the LED
	motor_red.setPosition(STEP_RED / 2);
	motor_red.updateBlocking();
	motor_yellow.setPosition(STEP_YELLOW / 2);
	motor_yellow.updateBlocking();
	delay(500);
#else
	digitalWrite(ledPin, HIGH); // turn on the LED
	Node *node = zusi->update();
	if (node != NULL)
	{
		for (int i = 0; i < node->getNodes()->size(); i++)
		{
			Node *subNode = node->getNodes()->get(i);
			if (subNode->getIDAsInt() == 0x0A)
			{
#if defined(Hauptluft)
				for (int j = 0; j < subNode->getAttribute()->size(); j++)
				{
					Attribute *attr = subNode->getAttribute()->get(j);
					if (attr->getIDAsInt() == Druck_Hauptluftleitung)
					{
						motor_yellow.setPosition((int)(attr->getDATAAsFloat() * STEP_YELLOW) / 12);
						motor_yellow.updateBlocking();
						Steps = ((attr->getDATAAsFloat() * STEP_YELLOW) / 12);
						Serial.print("Druck_Hauptluftleitung: ");
						Serial.print((attr->getDATAAsFloat()));
						Serial.println(" bar");
						Serial.print("Steps: ");
						Serial.println(Steps);
					}
				}
				for (int j = 0; j < subNode->getAttribute()->size(); j++)
				{
					Attribute *attr = subNode->getAttribute()->get(j);
					if (attr->getIDAsInt() == Druck_Hauptluftbehaelter)
					{
						Steps = ((attr->getDATAAsFloat() * STEP_RED) / 12);
						motor_red.setPosition((int)(attr->getDATAAsFloat() * STEP_RED) / 12);
						motor_red.updateBlocking();
						Serial.print("Druck_Hauptluftbehaelter: ");
						Serial.print(attr->getDATAAsFloat());
						Serial.println(" bar");
						Serial.print("Steps: ");
						Serial.println(Steps);
					}
				}
#else if defined(Bremsdruckzylinder)
				for (int j = 0; j < subNode->getAttribute()->size(); j++)
				{
					Attribute *attr = subNode->getAttribute()->get(j);
					if (attr->getIDAsInt() == Druck_Bremszylinder)
					{
						motor_yellow.setPosition((int)(attr->getDATAAsFloat() * STEP_YELLOW) / 10);
						motor_red.setPosition((int)(attr->getDATAAsFloat() * STEP_RED) / 10);
						motor_yellow.updateBlocking();
						motor_red.updateBlocking();
						Steps = ((attr->getDATAAsFloat() * STEP_RED) / 10);
						Serial.print("Druck_Bremszylinder: ");
						Serial.print((attr->getDATAAsFloat()));
						Serial.println(" bar");
						Serial.print("Steps: ");
						Serial.println(Steps);
					}
				}
#endif
			}
		}
	}
#endif
}
