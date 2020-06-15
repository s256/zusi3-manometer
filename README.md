# ESP32 powered gauge display for Zusi3

This project is based on https://github.com/Basti189/Zusi3Schnittstelle and https://github.com/clearwater/SwitecX25.
The Libs and code is chaos right now, but it's working.

Tested hardware: ESP32  in combination with https://forum.zusi.de/viewtopic.php?f=25&t=13044&p=311390

Use a level converter and seperate power delivery to use the stepper motors.

Under *./resources/* you can find several documents, like datasheets or the official ZuSi documentation including the TCP API.

Under *./cad/* you can find a PCB for the ESP32 including level converter made with Fritzing.

## To-Do: Seperate power delivery for the stepper motors. 
The motors are currently powered by the ESP, putting seperate 5V Input to the level converter didn't change that for some reason. I didn't have the time to check for the reason. I expect the motors to destroy the level converter because of the inductive power from the coils going back into the level converter (and maybe the ESP!). So be warned and use with caution.



# ESP32 Manometer für Zusi 3

Basierend auf https://github.com/Basti189/Zusi3Schnittstelle and https://github.com/clearwater/SwitecX25.

Der Code und die Einbindung der Libs sind Chaos, aber es läuft.

Eingesetzte Hardware: ESP32  mit https://forum.zusi.de/viewtopic.php?f=25&t=13044&p=311390

Für den richtigen Pegel an den Motoren kommen Pegelwandler 3,3V <-> 5V zum Einsatz.

Unter *./resources/* finden sich Dokumente, wie Datenblätter und die ZUSI Dokumentation inkl. TCP API.

Unter *./CAD/* findet sich ein mit Fritzing erstelltes PCB.

## To-Do: Stromversorgung der Motoren vom ESP trennen. Aktuell werden die Motoren mittel Pegelwandler vom ESP versorgt. 
Vermutlich kommt es hier zu induktiven Strömen der Motoren, welche die Pegelwandler (N-Mosfet) zerstören können und eventuell sogar den ESP!
Eine separate Stromzufuhr über den HV Pin der Pegelwandler hat daran nichts geändert, ich hatte aber noch keine Zeit mit die Ursache hierfür anzuschauen.
Benutzung also auf eigene Gefahr. 


# Zusi3Schnittstelle
Eine Programmierschnittstelle für Zusi 3 in Java und C++ (Arduino)

## C++ (Arduino) - Released 23.08.2018

Vorraussetzungen:
  - Hardware:
    - RAM: > 20KB
    - Eine Netzwerkverbindung
  
  - Software:
    - IDE: Arduino, Visual Studio, ...
    - LinkedList von Ivan Seidel Version 1.2.3, kann im Bibliotheksverwalter heruntergeladen werden
    - Zusi 3: Für ein optimales Erlebnis empfiehlt es sich, den Sendeabstand auf 0,100 - 0,250 sek zu stellen

  - Erfolgreich getestete Boards/Chips:
    - ESP8266 -> NodeMCU 1.0 (ESP12E-Module) (WiFi)
    - ESP32   -> NodeMCU-32S (WiFi)
    - ESP32   -> Olimex ESP32-Gateway (Ethernet)


Den Ordner Arduino klonen und die Arduino.ino starten. Das richtige Board auswählen.
Standartmäßig ist in der Zusi3Schnittstelle.h das ESP8266 definiert! Bitte anpassen, oder den Code selber anpassen.
Im Sketch wird eine Verbindung zu Zusi aufgebaut, das Programm läuft nicht weiter, ehe eine Verbindung erfolgreich hergestellt wurde.

```c++
Zusi3Schnittstelle *zusi;

void setup() {
	/* Netzwerkverbindung muss selber aufgebaut werden */

	zusi = new Zusi3Schnittstelle("10.0.0.20", 1436, "ESP8266 Client");
	zusi->setDebugOutput(true);
	zusi->reqFstAnz(Geschwindigkeit); //0x01
	zusi->reqFstAnz(Status_Zugbeeinflussung); //0x65
	zusi->requestFuehrerstandsbedienung(false);
	zusi->requestProgrammdaten(true); //Pfad, Zugnummer, etc...
	int i = 0;
	while (!zusi->connect()) {
		Serial.print("Verbindung zu Zusi fehlgeschlagen (");
		Serial.print(++i);
		Serial.println(")");
		delay(5000);
	}
	Serial.println("Verbunden mit Zusi");
}
```

Die Verbinung zu Zusi 3 wurde erfolgreich hergestellt, dann geht es weiter in der Methode loop().
Wichtig ist, dass in der Loop immer zusi->update() aufgerufen wird, da sich dort die Klasse darum kümmert, die Daten abzurufen und in Knoten zu verpacken, dort festgestellt wird, ob noch eine Verbindung zu Zusi besteht und wenn nicht, ein neuer Verbindungsaufbau versucht wird (Blockiert nur solange, wie ein Verbindungsversuch unternommen wird ~5 sek.). Liegen keine Daten an, wird NULL zurückgegeben, ansonsten ein Zeiger auf die Klasse Node.
Weitere Informationen zu den Knoten und Attributen finden Sie in der Dokumentation zu Zusi 3.

```c++
void loop() {
	Node *node = zusi->update();
	if (node != NULL) {
		// Todo
	}
}
```
Im Beispiel Sketch Arduino.ino wird die Geschwindigkeit ausgegeben und die Leuchtmelder eines PZB-Leuchtmelderblocks angesteuert.
Da Zusi mehrere Knoten der gleichen ID in einem Paket mitsenden kann ist es wichtig, immer mit der Liste direkt zu arbeiten, um wirklich alle Daten auswerten zu können, z.B.:
```c++
void loop() {
	Node *node = zusi->update();
	if (node != NULL) {
		for (int i = 0; i < node->getNodes()->size(); i++) {
			Node *subNode = node->getNodes()->get(i);
			if (subNode->getIDAsInt() == 0x0A) {
				//usw ...
			}
		}
	}
}
```
Attribute können ihre Daten in verschiedenen Datentypen zurückgeben, siehe Zusi 3 Dokumentation.
Zur Auswahl stehen hier folgende Datentype:
  - byte* getDATA() : Rohe Daten - Benötigt zusätzlich getDATASize() da es sich immer einen Zeiger auf ein byte array handelt
  - String getDATAsString() : Zeichenkette für Texte
  - int getDATAAsInt() : Für Zahlen, wie die Zeit oder was auch immer
  - float getDATAAsFloat() : Für z.B. die Geschwindigkeit(m/s) (Wert * 3.6), oder HLL/HBL Druck, etc...
  - boolean getDATAAsBoolean() : Für z.B. ist die Taste gedrückt oder nicht
  
  Die Methoden und Klassen sind übereinstimmend mit der Java Schnittstelle, da ich diese auf C++ portiert habe und angepasst habe. Somit ist die in Java geschriebene Schnittstelle (Noch nicht veröffentlicht) genau so zu benutzen, bis auf einen kleinen Unterschied.

# Switec X25 library
==================

This is an Arduino library for driving Switec X25 miniature 
stepper motors. It was written specifically for the Switec X25.168,
and compatible steppers from other manufacturers including the 
VID29 and MCR1108.

For more information including [datasheets](http://guy.carpenter.id.au/gaugette/resources) see the [Gaugette blog](http://guy.carpenter.id.au/gaugette/).

## Usage Notes
-----------
This library assumes you are driving the X25 directly from
the Arduino IO lines, not through an intermediate controller circuit.
A motor can be connected to any 4 Arduino digital output lines.
The Arduino Uno has 14 I/O lines, limiting you to three motors.
The Mega boards have more.

The rate at which these miniature stepper motors 
can accelerate is dependent upon the inertia
of the needle you are using.  You may need to tune the acceleration
tables in the library for your needle.


This Library Is ASYNCHRONOUS NON-BLOCKING
-----------------------------------------
If you read nothing more, please read and understand this:

The ```update()``` call is _non-blocking_.  It does _NOT_ move
the motor to the target position, it advances the motor _at most
just one step_ toward the target position and returns.

The correct way to use this library asynchronously is:

- Add a call to ```update()``` inside your main loop.  It should be called as frequently as possible.  It will return quickly if there is nothing to be done.  This means for example if you are waiting for serial I/O, you should be calling ```update()``` while you are waiting.

- Call ```setPosition()``` whenever you need to change the target position of the motor.

The advantage of this over a blocking/synchronous library is that you can
control many motors simultanously, and you wont have long periods of inactivity
(and potentially missed I/O events) while your motor is moving.

If instead you require a blocking call which will complete the motor action before returning:

- Call ```setPosition()``` to to the target position of the motore

- Call ```updateBlocking()``` which will move the motor smoothy to the position set by ```setPosition()```.

## Using the Library
-----------------

```C++
#include "SwitecX25.h"

// 315 degrees of range = 315x3 steps = 945 steps
// declare motor1 with 945 steps on pins 4-7
SwitecX25 motor1(315*3, 4,5,6,7);

// declare motor2 with 945 steps on pins 8-11
SwitecX25 motor2(315*3, 8,9,10,11);

void setup(void) {
  Serial.begin(9600);
  Serial.println("Go!");

  // run both motors against stops to re-zero
  motor1.zero();   // this is a slow, blocking operation
  motor2.zero();  
  motor1.setPosition(427);
  motor2.setPosition(427);
}

void loop(void) {
  // update motors frequently to allow them to step
  motor1.update();
  motor2.update();

  // do stuff, call motorX.setPosition(step) to 
  // direct needle to new position.
}
