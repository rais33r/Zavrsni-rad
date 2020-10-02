#include <Indio.h>
#include <Wire.h>
#include "Menu.h"


Menu::Menu() {
  activeMenu = 0;
  cursorLine = 0;
  cursorSpace = 8;
  backlightIntensity = 5;
}

void Menu::displayMenu(int buttonPressed, int data[], bool outputs[], int settings[]) {
  if(buttonPressed)
    handleButton(buttonPressed, settings);
  
  switch (activeMenu) {
    case MAIN_MENU:
      createMainMenu();
      break;
    case TEMP_HUM_MENU:
      createTempHumMenu(data);
      break;
    case OUTPUTS_MENU:
      createOutputsMenu(outputs);
      break;
    case SET_TEMPERATURE_MENU:
      createSetTemperatureMenu(settings);
      break;
    case SET_HUMIDITY_MENU:
      createSetHumidityMenu(settings);
      break;
    case TEMPERATURE_HYSTERESIS_MENU:
      createTemperatureHysteresisMenu(settings);
      break;
    case HUMIDITY_HYSTERESIS_MENU:
      createHumidityHysteresisMenu(settings);
      break;
    case BACKLIGHT_MENU:
      createBacklightMenu();
      break;
    default:
      cursorLine = 0;
      activeMenu = 0;
      createMainMenu();
  }
}

/////////////////////////////////////////////
/////////// HANDLING BUTTONS ////////////////
/////////////////////////////////////////////

void Menu::handleButton(int buttonPressed, int settings[]) {
  lcd.clear();
  
  switch (buttonPressed) {
    // UP BUTTON
    case 25:
      switch (activeMenu) {
        case MAIN_MENU:
          cursorLine = (cursorLine == 0) ? 6 : cursorLine - 1;
          break;
        case TEMP_HUM_MENU:
          // DO NOTHING
          break;
        case OUTPUTS_MENU:
          // DO NOTHING
          break;
        case SET_TEMPERATURE_MENU:
          settings[0] = (settings[1] < 600) ? settings[0] + 5 : settings[0];
          break;
        case SET_HUMIDITY_MENU:
          settings[1] = (settings[1] < 800) ? settings[1] + 5 : settings[1];
          break;
        case TEMPERATURE_HYSTERESIS_MENU:
          settings[2] = (settings[2] < 100) ? settings[2] + 5 : settings[2];
          break;
        case HUMIDITY_HYSTERESIS_MENU:
          settings[3] = (settings[3] < 100) ? settings[3] + 5 : settings[3];
          break;
        case BACKLIGHT_MENU:
          if(backlightIntensity < 5) {
            backlightIntensity++;
            analogWrite(26, map(backlightIntensity, 5, 0, 255, 0));
          }
          break;
      }
      break;
    // ENTER BUTTON
    case 24:
      switch (activeMenu) {
        case MAIN_MENU:
          activeMenu = cursorLine + 1;
          break;
        case TEMP_HUM_MENU:
        case OUTPUTS_MENU:
        case SET_TEMPERATURE_MENU:
        case SET_HUMIDITY_MENU:
        case TEMPERATURE_HYSTERESIS_MENU:
        case HUMIDITY_HYSTERESIS_MENU:
        case BACKLIGHT_MENU:
          cursorLine = 0;
          activeMenu = 0;
          break;
      }
      break;
    // DOWN BUTTON
    case 23:
      switch (activeMenu) {
        case MAIN_MENU:
          cursorLine = (cursorLine == 5) ? 0 : cursorLine + 1;
          break;
        case TEMP_HUM_MENU:
          // DO NOTHING
          break;
        case OUTPUTS_MENU:
          // DO NOTHING
          break;
        case SET_TEMPERATURE_MENU:
          settings[0] = (settings[0] > -400) ? settings[0] - 5 : settings[0];
          break;
        case SET_HUMIDITY_MENU:
          settings[1] = (settings[1] > 0) ? settings[1] - 5 : settings[1];
          break;
        case TEMPERATURE_HYSTERESIS_MENU:
          settings[2] = (settings[2] > 0) ? settings[2] - 5 : settings[2];
          break;
        case HUMIDITY_HYSTERESIS_MENU:
          settings[3] = (settings[3] > 0) ? settings[3] - 5 : settings[3];
          break;
        case BACKLIGHT_MENU:
          if(backlightIntensity > 0) {
            backlightIntensity--;
            analogWrite(26, map(backlightIntensity, 5, 0, 255, 0));
          }
          break;
      }
      break;
  }
}

/////////////////////////////////////////////
/////////////// MENUS ///////////////////////
/////////////////////////////////////////////

void Menu::createMainMenu() {
  lcd.setCursor(cursorSpace, 0);
  lcd.print("Show temp and hum");

  lcd.setCursor(cursorSpace, 1);
  lcd.print("Show outputs");

  lcd.setCursor(cursorSpace, 2);
  lcd.print("Set temperature");

  lcd.setCursor(cursorSpace, 3);
  lcd.print("Set humidity");

  lcd.setCursor(cursorSpace, 4);
  lcd.print("Set temp hysteresis");

  lcd.setCursor(cursorSpace, 5);
  lcd.print("Set hum hysteresis");

  lcd.setCursor(cursorSpace, 6);
  lcd.print("Adjust backlight");

  displayCursor();
}

void Menu::createTempHumMenu(int data[]) {
  lcd.setCursor(cursorSpace, 0);
  lcd.print("Return to Main Menu");
  
  lcd.setCursor(cursorSpace, 1);
  lcd.print("Temp 1: ");
  lcd.print(data[0] / 10.0);
  lcd.print(" C");
  
  lcd.setCursor(cursorSpace, 2);
  lcd.print("Temp 2: ");
  lcd.print(data[2] / 10.0);
  lcd.print(" C");
  
  lcd.setCursor(cursorSpace, 3);
  lcd.print("Temp 3: ");
  lcd.print(data[4] / 10.0);
  lcd.print(" C");
  
  lcd.setCursor(cursorSpace, 4);
  lcd.print("Hum 1: ");
  lcd.print(data[1] / 10.0);
  lcd.print("%");
  
  lcd.setCursor(cursorSpace, 5);
  lcd.print("Hum 2: ");
  lcd.print(data[3] / 10.0);
  lcd.print("%");
  
  lcd.setCursor(cursorSpace, 6);
  lcd.print("Hum 3: ");
  lcd.print(data[5] / 10.0);
  lcd.print("%");

  cursorLine = 0;
  displayCursor();
}

void Menu::createOutputsMenu(bool outputs[]) {
  lcd.setCursor(cursorSpace, 0);
  lcd.print("Return to Main Menu");

  lcd.setCursor(cursorSpace, 2);
  lcd.print("Heater: ");
  lcd.print( outputs[0] ? "ON" : "OFF" );

  lcd.setCursor(cursorSpace, 3);
  lcd.print("Cooler: ");
  lcd.print( outputs[1] ? "ON" : "OFF" );

  lcd.setCursor(cursorSpace, 4);
  lcd.print("Hum More: ");
  lcd.print( outputs[2] ? "ON" : "OFF" );

  lcd.setCursor(cursorSpace, 5);
  lcd.print("Hum Less: ");
  lcd.print( outputs[3] ? "ON" : "OFF" );
  
  cursorLine = 0;
  displayCursor();  
}

void Menu::createSetTemperatureMenu(int settings[]) {
  lcd.setCursor(cursorSpace, 0);
  lcd.print("Return to Main Menu");

  lcd.setCursor(cursorSpace, 2);
  lcd.print("Press UP/DOWN to");

  lcd.setCursor(cursorSpace, 3);
  lcd.print("set new temperature");

  lcd.setCursor(cursorSpace, 5);
  lcd.print("temp: ");
  lcd.print( settings[0] / 10.0 );

  cursorLine = 0;
  displayCursor();  
}

void Menu::createSetHumidityMenu(int settings[]) {
  lcd.setCursor(cursorSpace, 0);
  lcd.print("Return to Main Menu");

  lcd.setCursor(cursorSpace, 2);
  lcd.print("Press UP/DOWN to");

  lcd.setCursor(cursorSpace, 3);
  lcd.print("set new humidity");

  lcd.setCursor(cursorSpace, 5);
  lcd.print("hum: ");
  lcd.print( settings[1] / 10.0 );

  cursorLine = 0;
  displayCursor();  
}

void Menu::createTemperatureHysteresisMenu(int settings[]) {
  lcd.setCursor(cursorSpace, 0);
  lcd.print("Return to Main Menu");

  lcd.setCursor(cursorSpace, 2);
  lcd.print("Press UP/DOWN to set");

  lcd.setCursor(cursorSpace, 3);
  lcd.print("new temp hysteresis");

  lcd.setCursor(cursorSpace, 5);
  lcd.print("hyst: ");
  lcd.print( settings[2] / 10.0 );

  cursorLine = 0;
  displayCursor();  
}

void Menu::createHumidityHysteresisMenu(int settings[]) {
  lcd.setCursor(cursorSpace, 0);
  lcd.print("Return to Main Menu");

  lcd.setCursor(cursorSpace, 2);
  lcd.print("Press UP/DOWN to set");

  lcd.setCursor(cursorSpace, 3);
  lcd.print("new hum hysteresis");

  lcd.setCursor(cursorSpace, 5);
  lcd.print("hyst: ");
  lcd.print( settings[3] / 10.0 );

  cursorLine = 0;
  displayCursor();  
}

void Menu::createBacklightMenu() {
  lcd.setCursor(cursorSpace, 0);
  lcd.print("Return to Main Menu");

  lcd.setCursor(cursorSpace, 2);
  lcd.print("Press UP/DOWN to");

  lcd.setCursor(cursorSpace, 3);
  lcd.print("adjust brightness");

  lcd.setCursor(cursorSpace, 5);
  lcd.print("Brightness: ");
  lcd.print(backlightIntensity);

  cursorLine = 0;
  displayCursor();
}

/////////////////////////////////////////////

void Menu::displayCursor() {
  lcd.setCursor(0, cursorLine);
  lcd.print("*");
}
