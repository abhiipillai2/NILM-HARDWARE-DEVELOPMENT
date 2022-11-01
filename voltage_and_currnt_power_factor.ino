//
//    FILE: ACS712_20_AC.ino
//  AUTHOR: Rob Tillaart
// PURPOSE: demo AC measurement with point to point
//     URL: https://github.com/RobTillaart/ACS712


#include "ACS712.h"
#include "EmonLib.h" 
#include "math.h" // Include Emon Library
EnergyMonitor emon1;             // Create an instance



//  Arduino UNO has 5.0 volt with a max ADC value of 1023 steps
//  ACS712 5A  uses 185 mV per A
//  ACS712 20A uses 100 mV per A
//  ACS712 30A uses  66 mV per A

int pin = 33;
unsigned long pulseWidth;
float phase = 0;
float power_Factor =0;

ACS712  ACS(33,5.0, 4095, 100);

//  ESP 32 example (might requires resistors to step down the logic voltage)
//  ACS712  ACS(25, 3.3, 4095, 66);


void setup()
{
  Serial.begin(115200);
  pinMode(pin, INPUT);
  while (!Serial);
  Serial.println(__FILE__);
  Serial.print("ACS712_LIB_VERSION: ");
  Serial.println(ACS712_LIB_VERSION);

  ACS.autoMidPoint();
  Serial.print("MidPoint: ");
  Serial.print(ACS.getMidPoint());
  Serial.print(". Noise mV: ");
  Serial.println(ACS.getNoisemV());

  emon1.voltage(35, 234.26, 1.7);
}


void loop()
{

  pulseWidth = pulseIn(pin, HIGH);
  phase = (2 * 50 * 180 * (pulseWidth/1000000));
  power_Factor = cos(phase * (3.14/180));
  
  
  emon1.calcVI(20,2000);         // Calculate all. No.of half wavelengths (crossings), time-out
  //emon1.serialprint();           // Print out all variables (realpower, apparent power, Vrms, Irms, power factor)
  
  emon1.calcVI(20,2000);         // Calculate all. No.of half wavelengths (crossings), time-out
  //emon1.serialprint();           // Print out all variables (realpower, apparent power, Vrms, Irms, power factor)
  
  float supplyVoltage   = emon1.Vrms;             //extract Vrms into Variable
  float mA = ACS.mA_AC();
  float A = (mA/1000);
  float EA =(A -.3);
  if (EA > 0){

    Serial.print("current(RMS): ");
    Serial.print(EA);
    Serial.print("A");
    Serial.println("  ");
    Serial.print("Voltage(RMS): ");
    Serial.println(supplyVoltage);
    Serial.println("V");
    Serial.print("power factor: ");
    Serial.println(power_Factor);
    
  }else{
    Serial.println("current: 0");
    Serial.println(EA);
    Serial.print("Voltage(RMS): ");
    Serial.println(supplyVoltage);
    Serial.println(power_Factor);
  }
  
  delay(500);
}


// -- END OF FILE --
