// InFiDEL Master (arduino) Reader
/*
 *** Inline Filament Diameter by estimation Lowcost InFiDEL ***
  *** Credits ***
  Based on the original work from the following: Thank you

  Original firmware and hardware by Thomas Sanladerer
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

int Dia_IN = 0; // Diameter incomming from serial
float diameter = 0.00;

byte a, b;

void setup()
{
  Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output
  Serial.println("Hello");
}

void loop()
{

  //Wire.requestFrom(45, 4);    // request 4 bytes from slave device #45 Arduino Mega
  Wire.requestFrom(43, 2);    // request 2 bytes from slave device #43 ATtiny
  a = Wire.read();
  b = Wire.read();

  //store incomming data as an interger
  Dia_IN = a;
  Dia_IN = Dia_IN << 8 | b;

  //conert incomming data in to a float
  diameter = Dia_IN;
  diameter = diameter / 100;

  Serial.print("Diameter = ");
  Serial.print(diameter);

  Serial.println("mm");


  delay(2000);
}
