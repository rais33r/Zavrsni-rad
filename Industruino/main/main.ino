#include <Indio.h>
#include <Wire.h>
#include <UC1701.h>
#include <SPI.h>
#include <SD.h>
#include <Ethernet2.h>
#include <MCP7940.h>
#include <SimpleModbusMaster.h>
#include "Datalogger.h"
#include "Menu.h"
#include "Sensors.h"
#include "Webserver.h"


#define READING_DELAY 30 // seconds

Menu menu;
int rtc[7];

const int backlightPinNumber   = 26;
const int upButtonPinNumber    = 25;
const int enterButtonPinNumber = 24;
const int downButtonPinNumber  = 23;
const int EthernetPinNumber    = 10;
const int FRAMPinNumber        = 6;
const int SDPinNumber          = 4;

const int heaterPinNumber         = 1;
const int coolerPinNumber         = 2;
const int humidifierMorePinNumber = 3;
const int humidifierLessPinNumber = 4;

// Boolean values from heater, cooler, humidifierMore, humidifierLess state
bool outputs[4];

// Wanting temperature, humidity, temperature hysteresis and humidity hysteresis data
int settings[4];

float temperatureAverage;
float humidityAverage;

const float temperatureLimitLow = -40.0;
const float temperatureLimitHigh = 60.0;
const float humidityLimitLow = 0.0;
const float humidityLimitHigh = 80.0;

void setup() {
  pinMode(backlightPinNumber, OUTPUT);
  pinMode(upButtonPinNumber, INPUT);
  pinMode(enterButtonPinNumber, INPUT);
  pinMode(downButtonPinNumber, INPUT);
  pinMode(EthernetPinNumber, OUTPUT);
  pinMode(FRAMPinNumber, OUTPUT);
  pinMode(SDPinNumber, OUTPUT);

  digitalWrite(EthernetPinNumber, HIGH);
  digitalWrite(FRAMPinNumber, HIGH);
  digitalWrite(SDPinNumber, HIGH);

  Indio.digitalMode(heaterPinNumber, OUTPUT);
  Indio.digitalMode(coolerPinNumber, OUTPUT);
  Indio.digitalMode(humidifierMorePinNumber, OUTPUT);
  Indio.digitalMode(humidifierLessPinNumber, OUTPUT);

  Indio.digitalWrite(heaterPinNumber, LOW);
  outputs[0] = false;
  
  Indio.digitalWrite(coolerPinNumber, LOW);
  outputs[1] = false;
  
  Indio.digitalWrite(humidifierMorePinNumber, LOW);
  outputs[2] = false;
  
  Indio.digitalWrite(humidifierLessPinNumber, LOW);
  outputs[3] = false;

  //default user settings
  settings[0] = 200; // temperature (20.0 C)
  settings[1] = 300; // humidity (30.0%)
  settings[2] = 10;  // temperature hysteresis (1.0)
  settings[3] = 10;  // humidity hysteresis (1.0)

  analogWrite(backlightPinNumber, 255);

  Wire.begin();
  SerialUSB.begin(9600);
  while(!SerialUSB){
    //waiting for serialUSB to load
  }

  if (!SD.begin(SDPinNumber)) {
    SerialUSB.println("SD Card initialization failed!");
    while(1);
  }

  if( !createFile(fileName) )
    SerialUSB.println("Coudn't create file or file already exists: " + fileName);
  
  lcd.begin();

  // Modbus Setup - Temperature and Humidity for Sensor 1
  modbus_construct(&packets[PACKET1], 1, READ_INPUT_REGISTERS, 0x0001, 2, 0);
  
  // Modbus Setup - Temperature and Humidity for Sensor 2
  modbus_construct(&packets[PACKET2], 2, READ_INPUT_REGISTERS, 0x0001, 2, 2);

  // Modbus Setup - Temperature and Humidity for Sensor 3
  modbus_construct(&packets[PACKET3], 3, READ_INPUT_REGISTERS, 0x0001, 2, 4);

  // Modbus setup - Modbus configuration
  modbus_configure(&Serial, baud, SERIAL_8N2, timeout_time, polling, retry_count, TxEnablePin, packets, TOTAL_NO_OF_PACKETS, unsignedData);

  RTCind.get(rtc, true); 

  RTCind.set(MCP7940_SEC, 0);
  RTCind.set(MCP7940_MIN, 0);
  RTCind.set(MCP7940_HR, 12);
  RTCind.set(MCP7940_DOW, 3);
  RTCind.set(MCP7940_DATE,3);
  RTCind.set(MCP7940_MTH, 6);
  RTCind.set(MCP7940_YR, 20);
  
  RTCind.start(true);

  Ethernet.begin(mac, ip);
  server.begin();
  SerialUSB.print("SERVER IS AT: ");
  SerialUSB.println(Ethernet.localIP());

  modbus_update();
  delay(200);
}

void loop() {
  modbus_update();
  delay(100);
  
  RTCind.get(rtc, true);
  rtc[5] -= 20; // Bug in a clock library (when year 2020, month is plus 20)

  digitalWrite(EthernetPinNumber, HIGH);
  digitalWrite(SDPinNumber, LOW);
  delay(20);

  getLastEntry(fileName);
  
  if( firstEntry || timeForNextReading() ) {
    if( getTHFromSensors() && databaseFilter() ) {
      transformDataForWriting();
      writeDataToFile(fileName, dataForWriting);
      getAllEntries(fileName);
    }
  }

  regulateOutputs();

  int buttonPressed = readButtons();
  menu.displayMenu(buttonPressed, data, outputs, settings);

  digitalWrite(SDPinNumber, HIGH);
  digitalWrite(EthernetPinNumber, LOW);
  delay(20);

  webserver(outputs, settings);

  delay(100);
}

bool timeForNextReading() {
  long rtcTime;
  long lastEntryTime;

  if(rtc[6] > lastEntry[5] || rtc[5] > lastEntry[4] || rtc[4] > lastEntry[3]) {
    SerialUSB.println("Next day reading");
    return true;
  }

  rtcTime = rtc[0] + rtc[1] * 60 + rtc[2] * 3600;
  lastEntryTime = lastEntry[0] + lastEntry[1] * 60 + lastEntry[2] * 3600;

  if(rtcTime > (lastEntryTime + READING_DELAY)) {
    SerialUSB.println("New reading");
    return true;
  }

  return false;
}

int getTHFromSensors() {
  int attempts = 0;
  int i = 0;
  
  transformSensorsData(unsignedData);

  if(data[0] != 0)
    i++;
  if(data[2] != 0)
    i++;
  if(data[4] != 0)
    i++;

  temperatureAverage = i ? (data[0] + data[2] + data[4]) / (10.0 * i) : 0.0;

  i = 0;
  if(data[1] != 0)
    i++;
  if(data[3] != 0)
    i++;
  if(data[5] != 0)
    i++;

  humidityAverage = i ? (data[1] + data[3] + data[5]) / (10.0 * i) : 0.0;

  while( (temperatureAverage <= temperatureLimitLow)  ||
         (temperatureAverage >= temperatureLimitHigh) ||
         (humidityAverage <= humidityLimitLow)        ||
         (humidityAverage >= humidityLimitHigh) ) {
        modbus_update();
        delay(200);
        transformSensorsData(unsignedData);
        attempts++;
        if(attempts > 5){
          SerialUSB.println("Sensors reading is invalid!");
          return 0;
        }
  }

  return 1;
}

void transformSensorsData(unsigned int unsignedData[]) {
  for(int i=0;i<TOTAL_NO_OF_REGISTERS;i++) {
    if(unsignedData[i] < 32768)
      data[i] = (int) unsignedData[i];
    else
      data[i] = unsignedData[i] - 65536;
  }
}

int databaseFilter() {
  if(firstEntry)
    return 1;

  if(!getFilter(fileName))
    return 0;

  // temperatureAverage and humidityAverage are averages from three sensors
  // temperatureFilter and humidityFilter are averages from last three records (or less) from database
  if( (temperatureAverage + 10.0 > temperatureFilter) ||
      (temperatureAverage - 10.0 < temperatureFilter) ||
      (humidityAverage + 10.0 > humidityFilter) || 
      (humidityAverage - 10.0 < humidityFilter) )
    return 0;
  
  return 1;
}

void transformDataForWriting() {
  // DD-MM-YY-HH-MM-SS-STTT-HHH
  float temp;
  String newData = "";
  
  if(rtc[4]<10)
    newData += "0";
  newData += rtc[4];
  
  if(rtc[5]<10)
    newData += "0";
  newData += rtc[5];

  newData += (rtc[6] - 2000);

  if(rtc[2]<10)
    newData += "0";
  newData += rtc[2];

  if(rtc[1]<10)
    newData += "0";
  newData += rtc[1];

  if(rtc[0]<10)
    newData += "0";
  newData += rtc[0];

  temp = temperatureAverage * 10.0;
  newData += (temp > 0.0) ? "+" : "-";
  if(temp < 100.0 && temp > -100.0)
    newData += "0";
  if(temp < 10.0 && temp > -10.0)
    newData += "0";
  newData += (temp > 0.0) ? (int)temp : (int)temp * (-1);

  temp = humidityAverage * 10;
  if(temp<100.0)
    newData += "0";
  if(temp<10.0)
    newData += "0";
  newData += (int)temp;

  dataForWriting = newData;
}

void regulateOutputs() {
  Indio.digitalWrite(heaterPinNumber, LOW);
  outputs[0] = false;
  Indio.digitalWrite(coolerPinNumber, LOW);
  outputs[1] = false;
  Indio.digitalWrite(humidifierMorePinNumber, LOW);
  outputs[2] = false;
  Indio.digitalWrite(humidifierLessPinNumber, LOW);
  outputs[3] = false;

  if(temperatureAverage > (settings[0] / 10.0)) {
    Indio.digitalWrite(heaterPinNumber, LOW);
    outputs[0] = false;
    if(temperatureAverage > (settings[0] / 10.0 + settings[2] / 10.0)){
      Indio.digitalWrite(coolerPinNumber, HIGH);
      outputs[1] = true;
    }
  }

  if(temperatureAverage < (settings[0] / 10.0)) {
    Indio.digitalWrite(coolerPinNumber, LOW);
    outputs[1] = false;
    if(temperatureAverage < (settings[0] / 10.0 - settings[2] / 10.0)){
      Indio.digitalWrite(heaterPinNumber, HIGH);
      outputs[0] = true;
    }
  }

  if(humidityAverage > (settings[1] / 10.0)) {
    Indio.digitalWrite(humidifierMorePinNumber, LOW);
    outputs[2] = false;
    if(humidityAverage > (settings[1] / 10.0 + settings[3] / 10.0)) {
      Indio.digitalWrite(humidifierLessPinNumber, HIGH);
      outputs[3] = true;
    }
  }

  if(humidityAverage < (settings[1] / 10.0)) {
    Indio.digitalWrite(humidifierLessPinNumber, LOW);
    outputs[3] = false;
    if(humidityAverage < (settings[1] / 10.0 + settings[3] / 10.0)) {
      Indio.digitalWrite(humidifierMorePinNumber, HIGH);
      outputs[2] = true;
    }
  }
}

int readButtons() {
  int enterButtonState = digitalRead(enterButtonPinNumber);
  int upButtonState = digitalRead(upButtonPinNumber);
  int downButtonState = digitalRead(downButtonPinNumber);
  
  if (enterButtonState == LOW)
    return enterButtonPinNumber;
  else if (upButtonState == LOW)
    return upButtonPinNumber;
  else if (downButtonState == LOW)
    return downButtonPinNumber;
  else 
    return 0;
}
