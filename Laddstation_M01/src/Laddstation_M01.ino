/*
 * Project Laddstation_M01
 * Description:
 * Author: Lars Lindmark
 * Date:
 */
// This #include statement was automatically added by the Particle IDE.
#include <MQTT.h>
#include "PollingTimer.h"
//#include "Particle.h"
PRODUCT_ID(1180);
PRODUCT_VERSION(2);

/* Function prototypes -------------------------------------------------------*/
int tinkerDigitalRead(String pin);
int tinkerDigitalWrite(String command);
int tinkerAnalogRead(String pin);
int tinkerAnalogWrite(String command);
int analogvalue;
bool laddare1, laddare2, laddare3;
float temp;
//float temp5;
float batteryVoltage;
int offSet; // Amp sensor Offset the value from the middle of register
int sensorValue; // (A1) anolog Amp sensor
const float Vpp = 0.001221; // 5v/1023 = Vpp = 0.00488758553
float voltage; //Voltage reading at sensor
float chargeAmp; //Ampere reading
float solarPower; // Power (W) from solarcell
int aValue = 0; //Node-red test av integration reading (A0)
int bValue = 0; //Node-red test av integration reading (A1)

int cykelId;

void callback(char *topic, byte *payload, unsigned int length);
MQTT client("skinny.skycharts.net", 1883, callback);

PollingTimer batteryTimer(360000);
volatile int forceReading = 1;

void mqttPublish(char *event, String msg) {
    if (!client.isConnected()) {
         client.connect("x042_" + String(Time.now()));
    }
    if (client.isConnected()) {
        client.publish(event, msg);
    }
}

// MQTT recieve message (not used right now but include if needed)
void callback(char *topic, byte *payload, unsigned int length)
{
  char p[length + 1];
  memcpy(p, payload, length);
  p[length] = NULL;

//  Serial.print("MQTT rx:");
//  Serial.println(p);
//  setMessage(p);
    forceReading = 1;
}

// setup() runs once, when the device is first turned on.
void setup() {
  // Put initialization like pinMode and begin functions here.
    pinMode(D7, OUTPUT);
    Particle.function("doLED", doLED);
    Particle.variable("aValue", aValue);
    Particle.variable("bValue", bValue);
	
	//Setup the Tinker application here

	//Register all the Tinker functions
	Particle.function("digitalread", tinkerDigitalRead);
	Particle.function("digitalwrite", tinkerDigitalWrite);
	Particle.function("analogread", tinkerAnalogRead);
	Particle.function("analogwrite", tinkerAnalogWrite);
    //Set up serial communication with Particle on Pin Rx/Tx
    Serial1.begin(9600);      // open the serial port at 9600 bps:
    //Testar en Cloudvariable för att se hur den kan användas och var den syns
    Particle.publish("my-event","The internet just got smarter!");

    pinMode(D0, INPUT_PULLUP);
    pinMode(D1, INPUT_PULLUP);
    pinMode(D2, INPUT_PULLUP);
    pinMode(D3, OUTPUT);
    pinMode(A0, INPUT);
    
    // connect to the server(unique id by Time.now())
    client.connect("x042_" + String(Time.now()));
    if (client.isConnected()) {
        client.subscribe("updateStats");
    }
    mqttPublish("my-event", "MQTT connected");

    batteryTimer.start();
}


// loop() runs over and over again, as quickly as it can execute.
void loop() {
  // The core of your code will likely live here.

}