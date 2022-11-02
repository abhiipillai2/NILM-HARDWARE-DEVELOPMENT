//this code is property of Atom developers || for the project of elektron v.1.0 beta
//
//lib for API
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

//lib for sensors
#include "ACS712.h"
#include "EmonLib.h"
#include "math.h" // Include Emon Library
EnergyMonitor emon1;             // Create an instance

ACS712  ACS(33,5.0, 4095, 100);


const char* ssid = "KERALAVISION";
const char* password = "12345678";
//json object
char jsonOutput[128];
bool clint_flag = false;

float current_value = 2;
float voltage_value =210;
float power_factor = .8;


void setup() {

  Serial.begin(115200);
  Serial.println(__FILE__);
  Serial.print("ACS712_LIB_VERSION: ");
  Serial.println(ACS712_LIB_VERSION);

  ACS.autoMidPoint();
  Serial.print("MidPoint: ");
  Serial.print(ACS.getMidPoint());
  Serial.print(". Noise mV: ");
  Serial.println(ACS.getNoisemV());

  emon1.voltage(35, 234.26, 1.7);
  
  WiFi.begin(ssid, password); 
  
  while (WiFi.status() != WL_CONNECTED) { //Check for the connection
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  
  Serial.println("Connected to the WiFi network");
  delay(1000);
}
  
void loop() {

powerMeter();
delay(1000);
}

void vipDirectPushNode(float current_1,float voltage_1 ,float power){

   HTTPClient http;   

   //Specify destination for HTTP request
   http.begin("http://18.220.121.4:5000/dataPush");
   //Specify content-type header
   http.addHeader("Content-Type", "application/json");            

  // allocate the memory for the document
  const size_t CAPACITY = JSON_OBJECT_SIZE(10);
  StaticJsonDocument<CAPACITY> doc;

  // create an JSON object
   JsonObject object = doc.to<JsonObject>();
   object["hardware_id"] = "dcmain001";
   object[ "data"]["current"] = current_1;
   object[ "data"]["voltage"] = voltage_1;
   object[ "data"]["power"] = power;
  
  // serialize the object and send the result to Serial
  serializeJson(doc, jsonOutput);

    //Send the actual POST request
   int httpResponseCode = http.POST(String(jsonOutput));  
   Serial.println(jsonOutput);
  
   if(httpResponseCode>0){

    //Get the response to the request
    String response = http.getString();                       

    //Print return code
    Serial.println(httpResponseCode);
    //Print request answer
    Serial.println(response);           
  
   }else{
  
    Serial.print("Error on sending POST: ");
    Serial.println(httpResponseCode);
  
   }
   //Free resources  
   http.end();  
   
  delay(1000); 
}


void powerMeter(){

  emon1.calcVI(20,2000);         // Calculate all. No.of half wavelengths (crossings), time-out
  //emon1.serialprint();           // Print out all variables (realpower, apparent power, Vrms, Irms, power factor)
  
  emon1.calcVI(20,2000);         // Calculate all. No.of half wavelengths (crossings), time-out
  //emon1.serialprint();           // Print out all variables (realpower, apparent power, Vrms, Irms, power factor)
  
  float supplyVoltage   = emon1.Vrms;             //extract Vrms into Variable
  float mA = ACS.mA_AC();
  float A = (mA/1000);
  float EA =(A -.3);
    float power = (EA * supplyVoltage);
    vipDirectPushNode(EA,supplyVoltage,power);
  delay(500);
  
}
