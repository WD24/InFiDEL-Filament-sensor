// ATTiny Calibration
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


#include <TinyWireS.h>

// Holder for analog readings
int RAW = 0;

// Smoothing data
const int numReadings = 10;
int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
int average = 0;                // the average


//pins as named on PCB
#define FAULT 4 //on-board LED and digital output pin
#define A_IN A3 //SS495 output and push-button input (for calibration procedure)
#define A_OUT 1 //analog (filtered PWM) output, voltage proportional to filament diameter

// This firmware does not support analog output. rev 1 boards are populated without the lowpass filter on A_OUT.

//I2C
#define I2C_ADDR 43 //iterate for additional sensors in same I2C bus




void setup()
{
  //setup pins
  pinMode(FAULT, OUTPUT);
  pinMode(A_IN, INPUT);
  pinMode(A_OUT, OUTPUT);

  //start I2C
  TinyWireS.begin(I2C_ADDR);
  TinyWireS.onRequest(requestEvent);

  for (int thisReading = 0; thisReading < numReadings; thisReading++)
  {
    readings[thisReading] = 0;
  }

  //blink to indicate sensor powered and ready
  digitalWrite(FAULT, HIGH);
  delay(500);
  digitalWrite(FAULT, LOW);
  delay(500);
  digitalWrite(FAULT, HIGH);
  delay(500);
  digitalWrite(FAULT, LOW);
}

void loop()
{
  // Collect and smooth analog data from the arduino smoothing example
  RAW = analogRead(A_IN);
  // subtract the last reading:
  total = total - readings[readIndex];
  // read from the sensor:
  readings[readIndex] = RAW;
  // add the reading to the total:
  total = total + readings[readIndex];
  // advance to the next position in the array:
  readIndex = readIndex + 1;

  // if we're at the end of the array...
  if (readIndex >= numReadings)
  {
    // ...wrap around to the beginning:
    readIndex = 0;
  }

  // calculate the average:
  average = total / numReadings;
  delay(1);


}

// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent()
{
  
  byte myArray[2];
  // average analog output
  byte b1 = (average >> 8) &  0xFF;
  byte b2 = average & 0xFF;
  
  TinyWireS.write(b1);
  TinyWireS.write(b2);

 
}
