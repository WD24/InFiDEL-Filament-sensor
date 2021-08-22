// Master Reader Calibration
/*
  *** Credits ***
  Original firmware by Thomas Sanladerer
  https://www.prusaprinters.org/prints/57154-infidel-inline-filament-diameter-estimator-lowcost

  TinyWireS library by Nadav Matalon
  https://github.com/nadavmatalon/TinyWireS

  Arduino analog smoothing example

  created 22 Apr 2007
  by David A. Mellis  <dam@mellis.org>
  modified 9 Apr 2012
  by Tom Igoe

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/Smoothing
*/



#include <Wire.h>

int ADC_RAW = 0;


byte a, b;

void setup()
{
  Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output
  Serial.println("Hello");

  Serial.println("Use rods with a known diameter such as the shank on a drill bit.");
  Serial.println("Create a spredsheet with the analog out put along with the measured diameters.");
  Serial.println("Use you spreadsheet program to create a graph showing the formula");
  Serial.println("Input that formula in to the main sketch for the InFiDEL");

}

void loop()
{

  //Wire.requestFrom(45, 4);    // request 4 bytes from slave device #45 Arduino Mega
  Wire.requestFrom(43, 2);    // request 2 bytes from slave device #43 ATtiny
  a = Wire.read();
  b = Wire.read();


  ADC_RAW = a;
  ADC_RAW = ADC_RAW << 8 | b;

  Serial.print("RAW = ");
  Serial.print(ADC_RAW);

  Serial.print("\n");


  delay(1000);

}
