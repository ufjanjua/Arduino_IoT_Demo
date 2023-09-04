//all of the libraries needed on the edge side
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

//using pins 9 and 10 for transmission and recieiving
RF24 radio(9, 10);

//channel for reading incoming data
const byte ReadChannel[6] = "00002";
//channel for sending outgoing data
const byte WriteChannel[6] = "00004";
//string for reading user input
String UserData;

//function for getting light level
void GetLightLevel()
{
  //int to store the light level
  int LightLevel;
  if (radio.available())
  {
    //convert from char array to int
    radio.read(&LightLevel, sizeof(LightLevel));
    //print light level
    Serial.print("The light level is: ");
    Serial.println(LightLevel);
  }
}

//function for getting the average light level
void GetAvgLightValue()
{
  //float to store average
  float AverageLightValue;
  if (radio.available())
  {
    //convert from char array to float
    radio.read(&AverageLightValue, sizeof(AverageLightValue));
    //print average
    Serial.print("The average light level is: ");
    Serial.println(AverageLightValue);
  }
}

//function for temperature
void GetTemp()
{
  //int for temperature
  int temp;
  if (radio.available())
  {
    //convert char array to int
    radio.read(&temp, sizeof(temp));
    //print temperature
    Serial.print("The temperature is: ");
    Serial.println(temp);
    Serial.print("Celsius");

  }
}
//function to get average temperature
void GetAvgTemp()
{
  //float because of possible decimal vlue
  float AverageTemp;
  if (radio.available())
  {
    //covert char array to float
    radio.read(&AverageTemp, sizeof(AverageTemp));
    //print average temp
    Serial.print("The average temperature is: ");
    Serial.println(AverageTemp);
  }
}

void setup()
{
  //start radio
  radio.begin();
  radio.openWritingPipe(WriteChannel);
  radio.openReadingPipe(1, ReadChannel);
  radio.setPALevel(RF24_PA_MIN);
  //start in listening mode
  radio.startListening();
  //view physical layer responses and collect user input
  Serial.begin(9600);
}

void loop()
{
  Serial.println();
  Serial.println("Input a command");
  while (Serial.available() == 0);
  UserData = Serial.readString();
  UserData.trim();
  //switch to send mode
  radio.stopListening();
  //if user wants light level
  if (UserData == "get light level")
  {
    //send request
    radio.write("GetLight", sizeof("GetLight"));
    //listen and wait for response
    radio.startListening();
    delay(500);
    //execute function
    GetLightLevel();
  }
  //if user wants average light level
  else if (UserData == "get average light level")
  {
    //send request
    radio.write("GetAvgL", sizeof("GetAvgL"));
    //listen and wait for response
    radio.startListening();
    delay(500);
    //execute function
    GetAvgLightValue();
  }
  //if user wants to clear lighting values
  else if (UserData == "clear lighting levels")
  {
    //send request
    radio.write("ClearL", sizeof("ClearL"));
    //listen and wait for response
    radio.startListening();
    delay(500);
    //read and print response
    char response[32];
    radio.read(&response, sizeof(response));
    Serial.println(response);
  }  
  //if user wants temperature
  else if (UserData == "get temperature")
  {
    //send request
    radio.write("GetTemp", sizeof("GetTemp"));
    //listen and wait for response
    radio.startListening();
    delay(500);
    //execute function
    GetTemp();
  }
  //if user wants average temperature
  else if (UserData == "get the average temperature")
  {
    //send request
    radio.write("GetAvgT", sizeof("GetAvgT"));
    //listen and wait for response
    radio.startListening();
    delay(500);
    //execute function
    GetAvgTemp();
  }
  //if user wants to clear temps
  else if (UserData == "clear temperatures")
  {
    //send request
    radio.write("ClearTemps", sizeof("ClearTemps"));
    //listen and wait for response
    radio.startListening();
    delay(500);
    //convert and print response
    char response[32];
    radio.read(&response, sizeof(response));
    Serial.println(response);
  } 
  else if (UserData == "rotate motor")
  {
    radio.write("rotate", sizeof("rotate"));
  }
  else
  {
    Serial.println("That command is not recognized, please try again.");
  }
  //switches to listening mode
  radio.startListening();
}
