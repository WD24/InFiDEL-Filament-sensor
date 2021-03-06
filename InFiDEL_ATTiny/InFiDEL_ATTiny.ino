// InFiDEL ATTiny Sender
/*
 *** Inline Filament Diameter by estimation Lowcost InFiDEL ***
  *** Credits ***
  Based on the original work from the following:
  Thank you for sharing this with everyone.

  Original firmware and hardware  by Thomas Sanladerer
  https://www.prusaprinters.org/prints/57154-infidel-inline-filament-diameter-estimator-lowcost

  and further development by Daniel Smullen
  https://github.com/drspangle/infidel-sensor/development

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

//Outputs
int Dia;
int RAW = 0; // Holder for analog readings
float Diameter ;


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

//Formula
// Input formula generated by your spreadsheet here
// and comment out / uncomment the appropiate line in the the void loop()
// Logarithmic formula is active by default

// Map functions
// calculate diameter by mapping the ADC reading to the maximum and minimum radings / diameters
// Less accurate but works well if you just want to use the InFiDEL as a simple filament runout sensor

int Smallest_Diameter = 96;  // 0.96mm* 100
int Largest_Diameter = 200;  // 2.00mm* 100
int Lower_ADC = 183;
int Higher_ADC = 470;

/*
  A linear graph may produce better results if you have less data points in the calibration

    Linear diameter from Open Office
    f(x)=287.448x ??? 63.051


    Linear Diameter = (f(x)  + 63.051) / 287.448;
    LinearDia = (average  + 63.051) / 287.448;
    LinearDia = (average  + Linear_RAW_A) / Linear_RAW_B;
*/
#define Linear_RAW_A 63.051
#define Linear_RAW_B 287.448
/*
  A Logarithmic graph may produce better results if you have more data points in the calibration

   Logarithmic diameter from Open Office
   f(x)= 416.2165 In(x)+208.4720


   LogarithmicDia = exp((average - 208.472) / 416.2165)
   LogarithmicDia = exp((average - Logarithmic_RAW_A) / Logarithmic_RAW_B);
*/
#define Logarithmic_RAW_A 208.472
#define Logarithmic_RAW_B 416.2165


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

  // Calculate diameter
  // *****[ Uncomment the appropiate formula ]****

  // Approximate diameter from map function
  Dia = map(average, Lower_ADC, Higher_ADC, Smallest_Diameter, Largest_Diameter);

  // uncomment to use the approximate (mapped) diameter as serial output
  //Diameter = Dia/100;

  // Linear Output From Open office calculations
  //Diameter = (average + Linear_RAW_A) / Linear_RAW_B;

  // Logarithmic output from Open office calculations
  Diameter = exp((average - Logarithmic_RAW_A) / Logarithmic_RAW_B);


  //light LED and pull up FAULT if sensor saturated, button pressed or diameter low
  if (Dia > 250 or Dia  < 150)
  {
    digitalWrite(FAULT, HIGH);
  }
  else
  {
    digitalWrite(FAULT, LOW);
  }
}

// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent()
{
  int Diameter_out = Diameter * 100;


  byte myArray[2];
  // Raw analog output
  byte b1 = (Diameter_out >> 8) &  0xFF;
  byte b2 = Diameter_out & 0xFF;


  TinyWireS.write(b1);
  TinyWireS.write(b2);

}
