#include <Wire.h>
#include "INA219.h"

INA219 ina1(0x41);


void setup(void) 
{
  Serial.begin(115200);
  while (!Serial) {
      // will pause Zero, Leonardo, etc until serial console opens
      delay(1);
  }
  
  // Initialize the INA219.
  // By default the initialization will use the largest range (32V, 2A).  However
  // you can call a setCalibration function to change this range (see comments).
  ina1.begin();
  // To use a slightly lower 32V, 1A range (higher precision on amps):
  //ina219.setCalibration_32V_1A();
  // Or to use a lower 16V, 400mA range (higher precision on volts and amps):
  //ina219.setCalibration_16V_400mA();

  Serial.println("Measuring voltage and current with INA219 ...");
}

void loop(void) 
{
  float shuntvoltage1 = 0;
  float busvoltage1 = 0;
  float current_mA1 = 0;
  float loadvoltage1 = 0;
  float power_mW1 = 0;
  
  shuntvoltage1 = ina1.getShuntVoltage_mV();
  busvoltage1 = ina1.getBusVoltage_V();
  current_mA1 = ina1.getCurrent_mA();
  power_mW1 = ina1.getPower_mW();
  loadvoltage1 = busvoltage1 + (shuntvoltage1 / 1000);
  
  Serial.print("Bus Voltage:"); Serial.print(busvoltage1); Serial.print("V   ");
  Serial.print("Shunt Voltage:"); Serial.print(shuntvoltage1); Serial.print("mV   ");
  Serial.print("Load Voltage:"); Serial.print(loadvoltage1); Serial.print("V   ");
  Serial.print("Current:"); Serial.print(current_mA1); Serial.print("mA   ");
  Serial.print("Power:"); Serial.print(power_mW1); Serial.print("mW");

  Serial.println("");
  Serial.println("");
  delay(1000);
}
