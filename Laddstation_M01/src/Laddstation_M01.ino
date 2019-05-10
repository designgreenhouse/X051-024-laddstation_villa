/*
 * Project #010 - Laddstation_M01
 * Support: X046-010
 * Description: Laddstation-villa
 * Author: Lars Lindmark
 * Date: 2019-05-10
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
  // assume you have an analog sensor attached to A0:
    
    aValue = analogRead(A0);
    bValue = analogRead(A1);

    // calculating power from sensor readings A0 and A1
    solarPower = batteryVoltage * chargeAmp; 
     
/*******************************************************************************
 * Function Name    :   
 * Description      :   Serie communikation med Nextion display, platshållare till värde skapas i Nextion editor
                        och publiceras till Nextion display med 
 * Pin Tx           :   blue         
 * Pin  Rx          :   yellow
 * Pin GND          :   black
 * Pin  VIN (5V)    :   red
 * Id               :   t0
 * attribut         :   txt
 ******************************************************************************/


    if (client.isConnected()) {
        client.loop();
    }

   int takeReading = forceReading || batteryTimer.interval();
   
   if (!takeReading) {
       return;
   }

  // clear flag
  if (forceReading > 0) {
    forceReading--;
  }

  measureVA();
  
  Serial1.print("t0.txt=\"25.4°C\"");
  Serial1.write(0xff);
  Serial1.write(0xff);
  Serial1.write(0xff);
    
  delay(1000);

  Serial1.print("t0.txt=");
  Serial1.write(0x22); 
    // Visar batterispänning med en deciamal ex 13,9
  Serial1.print(batteryVoltage,1); 
  Serial1.write(0x22); 

  Serial1.write(0xff);
  Serial1.write(0xff);
  Serial1.write(0xff);
  //delay(1000);


  
  // Publicera till particle cloud
  String temp2 = String(batteryVoltage,1); // store voltage in "batteryVoltage" string
  Particle.publish("batteryVoltage", temp2, PRIVATE); // publish to cloud
  String temp4 = String(chargeAmp,1); // store ampere in "chargeAmp" string
  Particle.publish("chargeAmp", temp4, PRIVATE); // publish to cloud
  String temp5 = String(solarPower,1); // store ampere in "chargeAmp" string
  Particle.publish("solarPower", temp5, PRIVATE); // publish to cloud
  
  mqttPublish("batteryVoltage", temp2);
  mqttPublish("chargeAmp" , temp4);
  mqttPublish("solarPower" , temp5);
 // Particle.publish("batteryVoltage", "13,9", PRIVATE); // publish to cloud

    String temp3 = String(cykelId); // store "CykelId" in string
    laddare1 = digitalRead(D0);
    laddare2 = digitalRead(D1);
    laddare3 = digitalRead(D2);
    if (laddare1) {
    }
    else {
       Particle.publish( "laddare1/cykelId", temp3, PRIVATE);
       mqttPublish("laddare1/cykelId", temp3);
    }
   if (laddare2) {
    }
    else {
       Particle.publish( "laddare2/cykelId", temp3, PRIVATE);
       mqttPublish("laddare2/cykelId", temp3);
    }
   if (laddare3) {
    }
    else {
       Particle.publish( "laddare3/cykelId", temp3, PRIVATE);
       mqttPublish("laddare3/cykelId", temp3);
    }
    
    cykelId = cykelId + 1;
//delay (3600);
//delay (360000); // 5 minute delay
 
 
}

void measureVA() {
//version 1 ChargeAmp
//float average = 0;
  //for(int i = 0; i < 1000; i++) {
     
    //average = average + (.044 * analogRead(A0) -3.78) / 1000;//this is 
    //chargeAmp = average;
 // }

//Version 3 (A1 - 550) * (5v/1023 = 0.00488758553 =Vpp) / 0.066 känslighet sensor
    sensorValue = analogRead(A1);
    offSet = sensorValue - 3070;
    voltage = offSet * Vpp;
    chargeAmp = voltage / 0.066;
 //Version 2 chargeAmp
 // check to see what the value of the A1 input is and store it in the int(heltal) variable sensorValue
  // chargeAmp är ett flyttal som visar decimaler. Formel : chargeAmp = 60 *(A1 - 512) /1023 
    //sensorValue = analogRead(A1);
    //temp5 = 514 - sensorValue;
    //chargeAmp = 75.76 * temp5 / 1023;

  // check to see what the value of the A0 input is and store it in the int(heltal) variable analogvalue
  // batteryVoltage är ett flyttal som visar decimaler. Formel : batteryVoltage = A0 * 2 / 112
    analogvalue = analogRead(A0);
    temp = analogvalue*2;
    batteryVoltage = temp/112;
// 3035=54,2v  3012=53,8V : 3007= 53,7V : 3001 = 53,6V :2996= 53,5 :  2938 = 52V : 2800 = 50v : 2700 = 48,21

   if (analogvalue<2700) {
   // if (batteryVoltage>53) {2968
        digitalWrite(D3,HIGH);
        Particle.publish("my-event","High");
        mqttPublish("my-event", "High");

    }
 else if (analogvalue>2938) {
//    else if (batteryVoltage<53.8) {3012
        digitalWrite(D3,LOW);
        Particle.publish("my-event","Low");
        mqttPublish("my-event", "Low");
   }
   else {

   }



}




/*******************************************************************************
 * Function Name  : tinkerDigitalRead
 * Description    : Reads the digital value of a given pin
 * Input          : Pin
 * Output         : None.
 * Return         : Value of the pin (0 or 1) in INT type
                    Returns a negative number on failure
 *******************************************************************************/
int tinkerDigitalRead(String pin)
{
	//convert ascii to integer
	int pinNumber = pin.charAt(1) - '0';
	//Sanity check to see if the pin numbers are within limits
	if (pinNumber< 0 || pinNumber >7) return -1;

	if(pin.startsWith("D"))
	{
		pinMode(pinNumber, INPUT_PULLDOWN);
		return digitalRead(pinNumber);
	}
	else if (pin.startsWith("A"))
	{
		pinMode(pinNumber+10, INPUT_PULLDOWN);
		return digitalRead(pinNumber+10);
	}
	return -2;
}

/*******************************************************************************
 * Function Name  : tinkerDigitalWrite
 * Description    : Sets the specified pin HIGH or LOW
 * Input          : Pin and value
 * Output         : None.
 * Return         : 1 on success and a negative number on failure
 *******************************************************************************/
int tinkerDigitalWrite(String command)
{
	bool value = 0;
	//convert ascii to integer
	int pinNumber = command.charAt(1) - '0';
	//Sanity check to see if the pin numbers are within limits
	if (pinNumber< 0 || pinNumber >7) return -1;

	if(command.substring(3,7) == "HIGH") value = 1;
	else if(command.substring(3,6) == "LOW") value = 0;
	else return -2;

	if(command.startsWith("D"))
	{
		pinMode(pinNumber, OUTPUT);
		digitalWrite(pinNumber, value);
		return 1;
	}
	else if(command.startsWith("A"))
	{
		pinMode(pinNumber+10, OUTPUT);
		digitalWrite(pinNumber+10, value);
		return 1;
	}
	else return -3;
}

/*******************************************************************************
 * Function Name  : tinkerAnalogRead
 * Description    : Reads the analog value of a pin
 * Input          : Pin
 * Output         : None.
 * Return         : Returns the analog value in INT type (0 to 4095)
                    Returns a negative number on failure
 *******************************************************************************/
int tinkerAnalogRead(String pin)
{
	//convert ascii to integer
	int pinNumber = pin.charAt(1) - '0';
	//Sanity check to see if the pin numbers are within limits
	if (pinNumber< 0 || pinNumber >7) return -1;

	if(pin.startsWith("D"))
	{
		return -3;
	}
	else if (pin.startsWith("A"))
	{
		return analogRead(pinNumber+10);
	}
	return -2;
}

/*******************************************************************************
 * Function Name  : tinkerAnalogWrite
 * Description    : Writes an analog value (PWM) to the specified pin
 * Input          : Pin and Value (0 to 255)
 * Output         : None.
 * Return         : 1 on success and a negative number on failure
 *******************************************************************************/
int tinkerAnalogWrite(String command)
{
	//convert ascii to integer
	int pinNumber = command.charAt(1) - '0';
	//Sanity check to see if the pin numbers are within limits
	if (pinNumber< 0 || pinNumber >7) return -1;

	String value = command.substring(3);

	if(command.startsWith("D"))
	{
		pinMode(pinNumber, OUTPUT);
		analogWrite(pinNumber, value.toInt());
		return 1;
	}
	else if(command.startsWith("A"))
	{
		pinMode(pinNumber+10, OUTPUT);
		analogWrite(pinNumber+10, value.toInt());
		return 1;
	}
	else return -2;
}
    // Integration from Node-red
   int doLED(String cmd) {
    bool ledState = atoi(cmd);
    digitalWrite(D7, ledState);
    }