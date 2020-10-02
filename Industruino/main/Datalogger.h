#ifndef DATALOGGER_H
#define DATALOGGER_H

#include <SPI.h>
#include <SD.h>


File myFile;

const String fileName = "data004.txt";
String temperatureData;
String humidityData;
String timestamp;
String dataForWriting;
int lastEntry[6];
bool firstEntry;
float temperatureFilter;
float humidityFilter;

int createFile(String file) {
  if(SD.exists(file)) {
    SerialUSB.println("File " + file + " already exists");
    return 0;
  }

  myFile = SD.open(file, FILE_WRITE);
  myFile.close();

  if(!SD.exists(file)) {
    SerialUSB.println("File cannot be created.");
    return 0;
  }

  return 1;
}

int getFilter(String file) {
  char c;
  int i = 0;
  int numberOfEntries = 0;
  char tempAvgArray[3];
  char humAvgArray[3];
  float tempAvg[3];
  float humAvg[3];
  
  myFile = SD.open(file, FILE_WRITE);
  if(!myFile) {
    SerialUSB.println("Cannot open file " + file);
    return 0;
  }

  // (c > 47 && c < 58)
  while(myFile.available()) {
    c = myFile.read();
    if((c == 43) || (c == 45)) {
      tempAvgArray[0] = myFile.read();
      tempAvgArray[1] = myFile.read();
      tempAvgArray[2] = myFile.read();
      humAvgArray[0] = myFile.read();
      humAvgArray[1] = myFile.read();
      humAvgArray[2] = myFile.read();
      
      tempAvg[i] = (tempAvgArray[0] - 48) * 10.0 + (tempAvgArray[1] - 48) * 1.0 + (tempAvgArray[2] - 48) * 0.1;
      tempAvg[i] = (c == 43) ? tempAvg[i] : tempAvg[i] * (-1);

      humAvgArray[i] = (humAvgArray[0] - 48) * 10.0 + (humAvgArray[1] - 48) * 1.0 + (humAvgArray[2] - 48) * 0.1;

      if(numberOfEntries < 3)
        numberOfEntries++;
      
      i++;
      if(i == 3)
        i=0;
    }
  }

  switch(numberOfEntries) {
    case 0:
      return 0;
    case 1:
      temperatureFilter = tempAvg[0];
      humidityFilter = humAvg[0];
      return 1;
    case 2:
      temperatureFilter = (tempAvg[0] + tempAvg[1]) / 2.0;
      humidityFilter = (humAvg[0] + humAvg[1]) / 2.0;
      return 1;
    case 3:
      temperatureFilter = (tempAvg[0] + tempAvg[1] + tempAvg[2]) / 3.0;
      humidityFilter = (humAvg[0] + humAvg[1] + tempAvg[2]) / 3.0;
      return 1;
    default:
      return 0;
  }
}

int writeDataToFile(String file, String dataString) {
  // TODO: Validate string (19 characters, only numbers, ...)
  
  myFile = SD.open(file, FILE_WRITE);
  if(!myFile) {
    SerialUSB.println("Cannot open file " + file);
    return 0;
  }

  myFile.println(dataString);
  SerialUSB.println("Saved data: " + dataString);
  myFile.close();

  return 1;
}

int getLastEntry(String file) {
  char c;
  char readings[20];
  int i = 0;
  bool hasData = false;
  
  myFile = SD.open(file);
  if(!myFile) {
    SerialUSB.println("Cannot read data from file " + file);
    return 0;
  }

  while(myFile.available()) {
    c = myFile.read();
    if ((c > 47 && c < 58) || c == 43 || c== 45 ) {
      readings[i] = c;
      i++;
      hasData = true;
      if(i==19)
        i=0;
    }
  }

  if(hasData) {
    firstEntry = false;
    lastEntry[0] = (readings[10] - 48) * 10 + (readings[11] - 48);      // seconds
    lastEntry[1] = (readings[8] - 48) * 10 + (readings[9] - 48);        // minutes
    lastEntry[2] = (readings[6] - 48) * 10 + (readings[7] - 48);        // hours
    lastEntry[3] = (readings[0] - 48) * 10 + (readings[1] - 48);        // day
    lastEntry[4] = (readings[2] - 48) * 10 + (readings[3] - 48);        // month
    lastEntry[5] = 2000 + (readings[4] - 48) * 10 + (readings[5] - 48); // year
  } else {
    firstEntry = true;
  }

  if(firstEntry) {
    SerialUSB.println("Ready for first entry");
  }
  
  myFile.close();
  return 1;
}

int getAllEntries(String file) {
  myFile = SD.open(file);
  if(!myFile) {
    SerialUSB.println("Cannot read data from file " + file);
    return 0;
  }

  char c;
  int i = 0;

  temperatureData = "['";
  humidityData = "['";
  timestamp = "['";

  while(myFile.available()) {
    c = myFile.read();
    if ((c > 47 && c < 58) || c == 43 ||  c == 45) {
      switch(i) {
        case 0:
          timestamp += c;
          break;
        case 1:
          timestamp += c;
          timestamp += ".";
          break;
        case 2:
          timestamp += c;
          break;
        case 3:
          timestamp += c;
          timestamp += ".";
          timestamp += "20";
          break;
        case 4:
          timestamp += c;
          break;
        case 5:
          timestamp += c;
          timestamp += ".";
          break;
        case 6:
          timestamp += c;
          break;
        case 7:
          timestamp += c;
          timestamp += ":";
          break;
        case 8:
          timestamp += c;
          break;
        case 9:
          timestamp += c;
          timestamp += ":";
          break;
        case 10:
          timestamp += c;
          break;
        case 11:
          timestamp += c;
          timestamp += "', '";
          break;
        case 12:
          temperatureData += c;
          break;
        case 13:
          temperatureData += c;
          break;
        case 14:
          temperatureData += c;
          temperatureData += ".";
          break;
        case 15:
          temperatureData += c;
          temperatureData += "', '";
          break;
        case 16:
          humidityData += c;
          break;
        case 17:
          humidityData += c;
          humidityData += ".";
          break;
        case 18:
          humidityData += c;
          humidityData += "', '";
          break;
        default:
          SerialUSB.println("Error while reading all entries");
          temperatureData = "[]";
          humidityData = "[]";
          timestamp = "[]";
          return 0;
      }
      
      i++;
      if(i==19)
        i=0;
    }
  }

  if( temperatureData.length() > 3 ) {
    temperatureData.remove( temperatureData.length()-3 );
    humidityData.remove( humidityData.length()-3 );
    timestamp.remove( timestamp.length()-3 );
  
    temperatureData += "]";
    humidityData += "]";
    timestamp += "]";   
  } else {
    SerialUSB.println(file + " have no data");
    temperatureData = "['0']";
    humidityData = "['0']";
    timestamp = "['no time']";
  }

  myFile.close();

  SerialUSB.println(temperatureData);
  SerialUSB.println(humidityData);
  SerialUSB.println(timestamp);
  
  return 1;
}

int deleteFile(String file) {
  if(!SD.exists(file)) {
    SerialUSB.println("File " + file + " not exists so it cannot be deleted.");
    return 0;
  }

  SD.remove(file);

  if(SD.exists(file)) {
    SerialUSB.println("Coudn't delete file: " + file);
    return 0;
  }

  SerialUSB.println(file + " deleted");
  return 1;
}

#endif
