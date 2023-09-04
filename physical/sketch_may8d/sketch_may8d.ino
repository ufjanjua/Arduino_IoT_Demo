//all of the libraries needed for the physical layer side
#include <dht.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <string.h>
#include <Servo.h>

//motor object
Servo motor;

//temperature sensor object and pin
dht DHT;
#define TempsPin A3

//using pins 9 and 10 for transmitting and receiving
RF24 radio(9, 10); // CE, CSN

//this channel will be used for reading all incoming data
const byte ReadChannel[6] = "00004";
//this channel will be used for writing all outgoing data
const byte WriteChannel[6] = "00002";

//this will attach pin A4 as an integer to read the light values
int photocell = A4;

//this is the array and integer that will allow for the light values to be stored
int AverageLightLevelArray[10];
int LightArrayCounter = 0;

//this is the array and integer that will allow for the temperature values to be stored
int AverageTempsArray[10];
int TempArrayCounter = 0;

//this function allows for the edge to get light values and average lighting
void LightLevelFunction(int y)
{
  int LightLevel = analogRead(photocell);
  radio.stopListening();
  //if edge wants the light level
  if (y == 0)
  {
    //send light level to edge
    radio.write(&LightLevel, sizeof(LightLevel));
    Serial.println(LightLevel);
    //add light level to array
    AverageLightLevelArray[LightArrayCounter] = LightLevel;
    LightArrayCounter = (LightArrayCounter + 1);

  }
  //if edge wants average light level
  else if (y == 1)
  {
    //calculate average
    int SumOfLightLevel = 0;
    for (int i = 0; i < LightArrayCounter; i++)
    {
      SumOfLightLevel += AverageLightLevelArray[i];
    }
    //float because of decimal possibility
    float AverageLightLevel = (float) SumOfLightLevel / LightArrayCounter;
    //send data
    radio.write(&AverageLightLevel, sizeof(AverageLightLevel));
    Serial.println(AverageLightLevel);
  }
  //back to receive mode
  radio.startListening();
}


//this function will allow for the physical layer to send temperature values and average temperature
void TemperatureFunction(int x)
{
  //reads the value from the sensor
  int temps = DHT.temperature;
  //stop listening to return requested data
  radio.stopListening();
  //if edge wants the temperature
  if(x == 0)
  {
    //sent edge the current temperature
    radio.write(&temps, sizeof(temps));
    //print temp
    Serial.println(temps);
    //add the temperature to the array
    AverageTempsArray[TempArrayCounter] = temps;
    TempArrayCounter = (TempArrayCounter + 1);

    Serial.println();
  }
  //if edge wants the average temperature
  else if(x == 1)
  {
    //calculate the average temperature from the array
    int SumOfTemps = 0;
    for (int i = 0; i < TempArrayCounter; i++)
    {
      SumOfTemps += AverageTempsArray[i];
    }
    //float to deal with possibility of decimal value
    float AverageTemp = (float) SumOfTemps / TempArrayCounter;
    //send data to edge
    radio.write(&AverageTemp, sizeof(AverageTemp));
    Serial.println(AverageTemp);
  }
  //back to listening mode
  radio.startListening();
  
}



void setup()
{
  //read data from photoresistor
  pinMode(photocell, INPUT);
  //use pin A5 for the motor
  motor.attach(A5);
  //start radio
  radio.begin();
  radio.openReadingPipe(0, ReadChannel);
  radio.openWritingPipe(WriteChannel);
  radio.setPALevel(RF24_PA_MIN);
  //start in listening mode
  radio.startListening();
  //view edge input
  Serial.begin(9600);
}


void loop()
{   
  //read data
  char EdgeData[] = "NoCommand";
  //convert temperature reading to int
  int TempData = DHT.read11(TempsPin);

  //listen for any edge commands
  if(radio.available())
  {
    //read and print the data coming from the edge
    radio.read(&EdgeData, sizeof(EdgeData));
    Serial.println(EdgeData);
    //if the command is to get the lighting level
    if (strcmp(EdgeData, "GetLight") == 0)
    {
        LightLevelFunction(0);
    }
    //if the command is to get the average light level
    else if (strcmp(EdgeData, "GetAvgL") == 0)
    {
        LightLevelFunction(1);
    }
    //if the command is to clear the array
    else if (strcmp(EdgeData, "ClearL") == 0)
    {
        //clears the array
        for (int i = 0; i < LightArrayCounter; i++)
        {
          AverageLightLevelArray[i] = 0;
        }
        LightArrayCounter = 0;
        char Response[] = "Light Levels Cleared";
        radio.stopListening();
        radio.write(&Response, sizeof(Response));
        radio.startListening();
    }
    //if the command is to get the temperature
    else if(strcmp(EdgeData, "GetTemp") == 0)
    {
     TemperatureFunction(0);
    }
    //if the comand is to get the average temperature
    else if (strcmp(EdgeData, "GetAvgT") == 0)
    {
        TemperatureFunction(1);
    }
    //if the command is to clear the array
    else if (strcmp(EdgeData, "ClearTemps") == 0)
    {
        //clear the array
        for (int i = 0; i < TempArrayCounter; i++)
        {
          AverageTempsArray[i] = 0;
        }
        TempArrayCounter = 0;
        //response
        char Response[] = "Temps have been cleared";
        radio.stopListening();
        radio.write(&Response, sizeof(Response));
        radio.startListening();
    }
    //if the command is to rotate the motor
    else if (strcmp(EdgeData, "rotate") == 0)
    {
        int pos = 0;
        for (pos = 0; pos <= 180; pos ++)
        {
        motor.write(pos);
        delay(15);
        }    
    }
  }
}

