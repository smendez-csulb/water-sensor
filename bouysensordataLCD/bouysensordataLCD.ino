#include <LiquidCrystal.h>
int rs=7;
int en=8;
int d4=9;
int d5=10;
int d6=11;
int d7=12;

LiquidCrystal lcd(rs,en,d4,d5,d6,d7);


// Thermometer libraries
#include <Wire.h>

#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 6

OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature sensors(&oneWire);

float Celsius = 0;
float Fahrenheit = 0;

// Turbidity Sensor
#define TurbiditySensor A1

/* BEGIN TDS Sensor */
#define TdsSensorPin A2
#define VREF 5.0      // analog reference voltage(Volt) of the ADC
#define SCOUNT  30           // sum of sample point
int analogBuffer[SCOUNT];    // store the analog value in the array, read from ADC
int analogBufferTemp[SCOUNT];
int analogBufferIndex = 0, copyIndex = 0;
float averageVoltage = 0, tdsValue = 0, temperature = 25;
/* END TDS Sensor */

void setup() {
// Begin LCD
lcd.begin(16,2); // liquid column has 16 columns and 2 rows.

// Begin Thermometer Sensor
sensors.begin();

// Begin TDS Sensor
pinMode(TdsSensorPin, INPUT);

Serial.begin(9600);
}

void loop() {

  // Run thermoCouple function
  lcd.setCursor(0, 0);//lcd.setCursor(column,row)
  thermoCouple();

  // Run TDS
  total_Dissolved_Solids();
  
  // Run Turbidity
  lcd.setCursor(0, 1);
  turbidity();

}

void turbidity()
{
  // read the input on analog pin 0:
  int sensorValue = analogRead(TurbiditySensor);
  // print out the value you read:
  Serial.print("Turbidity: ");
  Serial.println(sensorValue);
  lcd.print("Turbidity: ");
  lcd.print(sensorValue);
  delay(100);        // delay in between reads for stability
}

void thermoCouple()
{
  sensors.requestTemperatures();

  Celsius = sensors.getTempCByIndex(0);
  Fahrenheit = sensors.toFahrenheit(Celsius);

  
  lcd.print(Celsius);

  lcd.setCursor(4, 0);
  lcd.print(" C. ");

  // lcd.setCursor(8, 0);
  //lcd.print(Fahrenheit);

  // lcd.setCursor(12, 0);
  //lcd.print("F");

  //Troubleshoot with Serial print
  Serial.print(Celsius);
  Serial.print(" C  ");
  Serial.print(Fahrenheit);
  Serial.println(" F");
  delay(100);
}

void total_Dissolved_Solids()
{
  static unsigned long analogSampleTimepoint = millis();
  if (millis() - analogSampleTimepoint > 40U)  //every 40 milliseconds,read the analog value from the ADC
  {
    analogSampleTimepoint = millis();
    analogBuffer[analogBufferIndex] = analogRead(TdsSensorPin);    //read the analog value and store into the buffer
    analogBufferIndex++;
    if (analogBufferIndex == SCOUNT)
      analogBufferIndex = 0;
  }
  static unsigned long printTimepoint = millis();
  if (millis() - printTimepoint > 800U)
  {
    printTimepoint = millis();
    for (copyIndex = 0; copyIndex < SCOUNT; copyIndex++)
      analogBufferTemp[copyIndex] = analogBuffer[copyIndex];
    averageVoltage = getMedianNum(analogBufferTemp, SCOUNT) * (float)VREF / 1024.0; // read the analog value more stable by the median filtering algorithm, and convert to voltage value
    float compensationCoefficient = 1.0 + 0.02 * (temperature - 25.0); //temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
    float compensationVolatge = averageVoltage / compensationCoefficient; //temperature compensation
    tdsValue = (133.42 * compensationVolatge * compensationVolatge * compensationVolatge - 255.86 * compensationVolatge * compensationVolatge + 857.39 * compensationVolatge) * 0.5; //convert voltage value to tds value
    //Serial.print("voltage:");
    //Serial.print(averageVoltage,2);
    //Serial.print("V   ");
    Serial.print("TDS----Value:");
    lcd.print("TDS:");

    Serial.print(tdsValue, 0);
    lcd.print(tdsValue,0);

    if(tdsValue < 10)
    {
      lcd.setCursor(13,0);
      lcd.print(" ");
      lcd.setCursor(14, 0);
      lcd.print(" ");
    }
    else if(tdsValue >= 10 && tdsValue <100)// this else catch clears LCD for any trailing digits from hundredths place 
    {
      lcd.setCursor(14, 0);
      lcd.print(" ");
    }

    Serial.println("ppm");
    // lcd.print("ppm");
  }
}

int getMedianNum(int bArray[], int iFilterLen)
{
  int bTab[iFilterLen];
  for (byte i = 0; i < iFilterLen; i++)
    bTab[i] = bArray[i];
  int i, j, bTemp;
  for (j = 0; j < iFilterLen - 1; j++)
  {
    for (i = 0; i < iFilterLen - j - 1; i++)
    {
      if (bTab[i] > bTab[i + 1])
      {
        bTemp = bTab[i];
        bTab[i] = bTab[i + 1];
        bTab[i + 1] = bTemp;
      }
    }
  }
  if ((iFilterLen & 1) > 0)
    bTemp = bTab[(iFilterLen - 1) / 2];
  else
    bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
  return bTemp;
}