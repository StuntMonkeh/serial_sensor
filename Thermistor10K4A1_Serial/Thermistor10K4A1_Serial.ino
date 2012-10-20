/*
 * Inputs ADC Value from Thermistor and outputs Temperature in Celsius
 *  requires: include <math.h>
 * Utilizes the Steinhart-Hart Thermistor Equation:
 *    Temperature in Kelvin = 1 / {A + B[ln(R)] + C[ln(R)]3}
 *    where A = 0.001129148, B = 0.000234125 and C = 8.76741E-08
 *
 * These coefficients seem to work fairly universally, which is a bit of a 
 * surprise. 
 *
 * Schematic:
 *   [Ground] -- [10k-pad-resistor] -- | -- [thermistor] --[Vcc (5 or 3.3v)]
 *                                     |
 *                                Analog Pin 0
 *
 * In case it isn't obvious (as it wasn't to me until I thought about it), the analog ports
 * measure the voltage between 0v -> Vcc which for an Arduino is a nominal 5v, but for (say) 
 * a JeeNode, is a nominal 3.3v.
 *
 * The resistance calculation uses the ratio of the two resistors, so the voltage
 * specified above is really only required for the debugging that is commented out below
 *
 * Resistance = (1024 * PadResistance/ADC) - PadResistor 
 *
 * I have used this successfully with some CH Pipe Sensors (http://www.atcsemitec.co.uk/pdfdocs/ch.pdf)
 * which be obtained from http://www.rapidonline.co.uk.
 *
 */

#include <math.h>

#define ThermistorPIN 0                 // Analog Pin 0

float vcc = 4.959;                       // only used for display purposes, if used
                                        // set to the measured Vcc.
float pad = 1000;                       // balance/pad resistor value, set this to
                                        // the measured resistance of your pad resistor
float thermr = 1400;                   // thermistor nominal resistance

float Thermistor(int RawADC) {
  long Resistance;  
  float Temp;  // Dual-Purpose variable to save space.

  Resistance=((1024 * pad / RawADC) - pad)-10; // the -10 at the end is calibration
  Temp = log(Resistance); // Saving the Log(resistance) so not to calculate  it 4 times later
  Temp = 1 / (0.001129148 + (0.000234125 * Temp) + (0.0000000876741 * Temp * Temp * Temp));
  Temp = Temp - 273.15;  // Convert Kelvin to Celsius                      

  return Temp;                                      // Return the Temperature
}

//Software smoothing
const int numReadings = 2;
int readings[numReadings];  // the readings from the sensor
int index = 0;              // the index of the current reading
float total = 0;              // the running total
float average = 0;            // the average

float temp;
float input = temp;

void setup() {
  Serial.begin(9600);
  for (int thisReading = 0; thisReading < numReadings; thisReading++)
    readings[thisReading] = 0;
}

void loop() {
  float temp;
  float tempAverage;
  temp=Thermistor(analogRead(ThermistorPIN));      //read ADC and  convert it to Celsius
  total= total - readings[index];                  //subtract the last reading
  readings[index] = temp;                          //read from the sensor
  total= total + readings[index];                  //add the reading to the total
  index = index + 1;                               //advance to the next position in the array
  
  if (index >= numReadings)                        //if we are at the end of the array
    index = 0;                                     //wrap back to the beginning

  tempAverage = total / numReadings;                //calculate the average
  //Serial.print("temperature:"); 
  Serial.print(tempAverage,1);                      // display Celsius
  Serial.println("");                                   
  delay(10000);                                     // Delay a bit...
}
