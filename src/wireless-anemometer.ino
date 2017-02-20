/*


*/

#include <Wire.h>
#include <Adafruit_WINC1500.h>
#include "network.h"

int devID = 201;
int RELAY = A1;
int main_indicator = 11;
int bpm_indicator = 12;

int stat = 0;
int main_stat = 0;
String post_add = "incoming/";
int onFlag = 0;
int offFlag = 0;

int sensorPin = A5;    // select the input pin for the potentiometer
int battPin = A4;
int ledPin = 13;      // select the pin for the LED

double analogVref = 3.272;
double chargeCutOff = 4.2;
double dischargeCutOff = 2.75;
double sensorValue = 0;  // variable to store the value coming from the sensor
double battVoltage = 0;
double battLevel = 0;
double Vout = 0;
double windSpeed = 0;

unsigned long lastConnectionTime = 0;            // last time you connected to the server, in milliseconds
const unsigned long gettingInterval = 1L * 1000L; // delay between updates, in milliseconds

void setup() {
  // declare the ledPin as an OUTPUT:
  pinMode(ledPin, OUTPUT);
  pinMode(sensorPin, INPUT);
  pinMode(battPin, INPUT);

#ifdef WINC_EN
  pinMode(WINC_EN, OUTPUT);
  digitalWrite(WINC_EN, HIGH);
#endif
  pinMode(bpm_indicator, OUTPUT);
  pinMode(main_indicator, OUTPUT);
  digitalWrite(bpm_indicator, LOW);
  digitalWrite(main_indicator, LOW);
  Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output
  pinMode(RELAY, OUTPUT);
  digitalWrite(RELAY, LOW);
  delay(2000);

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  // attempt to connect to Wifi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
  // you're connected now, so print out the status:
  printWifiStatus();
}

void loop() {
  // read the value from the sensor:
  sensorValue = analogRead(sensorPin);
  battVoltage = analogRead(battPin);
  battVoltage = (battVoltage / 1023) * analogVref;
  battVoltage = ((battVoltage - 0.012) * 2.0);
  battLevel = round((battVoltage - dischargeCutOff) * (100 / (chargeCutOff - dischargeCutOff)));
  if (battLevel > 100) {
    battLevel = 100;
  }
  Vout = (sensorValue / 1023.0) * analogVref;
  windSpeed = ((Vout - 0.4) / 1.6) * 32.4;
  Serial.println(String(battLevel) + ", " + String(battVoltage) + ", " + String(sensorValue) + ", " + String(Vout) + ", " + String(windSpeed));

  if ((millis() - lastConnectionTime > gettingInterval)) {
    long rssi = WiFi.RSSI();
    Serial.println("stat: " + String(stat) + "\n");
    httpRequestPOST(String(devID), String(windSpeed), String(rssi), String(battLevel), String(battVoltage), post_add);
    lastConnectionTime = millis();
  }
}
