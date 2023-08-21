/****************************************
 * Include Libraries
 ****************************************/
#include "UbidotsESPMQTT.h"
#include <DHT.h>
#include <Adafruit_BMP280.h>
#include <Wire.h>
#include "PubsubClient.h"

#define BMP280_I2C_ADDRESS  0x76

Adafruit_BMP280  bmp280;

#define DHTPIN 5     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(5, DHT22); //// Initialize DHT sensor for normal 16mhz Arduino
float calibration_value = 21.34 - 0.7;
int phval = 0;
unsigned long int avgval;
int buffer_arr[10],temp;

float ph_act;

int sense_Pin= 0; // Soil Sensor input at Analog PIN A0
int value= 0;

float hum;
float temp1;

#define TOKEN "BBFF-qT6AkbkCPeLdEWQUIQxfBG8Crrxo24" // Your Ubidots TOKEN
#define WIFINAME "V2037" //Your SSID
#define WIFIPASS "password1999" // Your Wifi Pass

Ubidots client(TOKEN);

/****************************************
 * Auxiliar Functions
 ****************************************/

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

/****************************************
 * Main Functions
 ****************************************/

void setup() {
   Serial.begin(115200);
  dht.begin();
  Wire.begin(4, 0);
  bmp280.begin(0x76);
  

 
  //client.ubidotsSetBroker("business.api.ubidots.com"); // Sets the broker properly for the business account
  client.setDebug(true); // Pass a true or false bool value to activate debug messages
  
  client.wifiConnection(WIFINAME, WIFIPASS);
  
  client.begin(callback);
  }

void loop() {
      //Read data and store it to variables hum and temp
    hum = dht.readHumidity();
    temp= dht.readTemperature();
   value= analogRead(sense_Pin);
   value=value/10;
     float pressure = bmp280.readPressure(); 
     float pressure1=pressure/100;
     for(int i=0;i<10;i++)
{
buffer_arr[i]=analogRead(A0);
delay(30);
}
for(int i=0;i<9;i++)
{
for(int j=i+1;j<10;j++)
{
if(buffer_arr[i]>buffer_arr[j])
{
temp1=buffer_arr[i];
buffer_arr[i]=buffer_arr[j];
buffer_arr[j]=temp1;
}
}
}
avgval=0;
for(int i=2;i<8;i++)
avgval+=buffer_arr[i];
float volt=(float)avgval*5.0/1024/6;
ph_act = -(-5.70 * volt + calibration_value);
Serial.println("pH Val:  ");
Serial.println(ph_act);

  Serial.print("Pressure: ");
  Serial.print(pressure);
  

  
   Serial.print("MOISTURE LEVEL : ");
   Serial.println(value);
    //Print temp and humidity values to serial monitor
    Serial.print("Humidity: ");
    Serial.print(hum);
    Serial.print(" %, Temp: ");
    Serial.print(temp);
    Serial.println(" Celsius");


    
  
  
    if(!client.connected()){
      client.reconnect();
      }
 
  client.add("temperature", temp);
  client.add("humidity", hum);
  client.add("pressure", pressure1);
  client.add("moisture", value);
  client.add("ph_value",ph_act);
  
  
  client.ubidotsPublish("nodemcu");
  client.loop();
   

  delay(1800000);
}
