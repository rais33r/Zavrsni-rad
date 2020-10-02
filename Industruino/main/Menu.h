#ifndef MENU_H
#define MENU_H

#include <UC1701.h>
static UC1701 lcd;

#define MAIN_MENU                   0
#define TEMP_HUM_MENU               1
#define OUTPUTS_MENU                2
#define SET_TEMPERATURE_MENU        3
#define SET_HUMIDITY_MENU           4
#define TEMPERATURE_HYSTERESIS_MENU 5
#define HUMIDITY_HYSTERESIS_MENU    6
#define BACKLIGHT_MENU              7


static int activeMenu;
static int cursorLine;
static int cursorSpace;
static int backlightIntensity;

class Menu {
  private: 
    void createMainMenu();
    void createTempHumMenu(int data[]);
    void createOutputsMenu(bool outputs[]);
    void createSetTemperatureMenu(int settings[]);
    void createSetHumidityMenu(int settings[]);
    void createTemperatureHysteresisMenu(int settings[]);
    void createHumidityHysteresisMenu(int settings[]);
    void createBacklightMenu();
    
    void handleButton(int buttonPressed, int settings[]);
    void displayCursor();
  public:
    Menu();
    void displayMenu(int buttonPressed, int data[], bool outputs[], int settings[]);
};

#endif
