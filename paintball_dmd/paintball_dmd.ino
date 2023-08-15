 /*P10 led pannel pins to ESP 8266
  A D0
  B D6
  CLK D5
  SCK D3
  R D7
  NOE D8  
  GND GND
*/

#include <SPI.h>
#include <DMD2.h>
#include "ArduinoJson.h"
#include <fonts/SystemFont5x7.h>
#include <fonts/Arial14.h>
#include <fonts/Arial_Black_16.h>
#include <fonts/Droid_Sans_12.h>
#include <fonts/Droid_Sans_16.h>
#include <fonts/Droid_Sans_24.h>

const int HORIZONTAL_PANEL_NUMBER = 4;
const int VERTICAL_PANEL_NUMBER = 4;
const int HORIZONTAL_PANEL_PIXEL_NUMBER = 32;
const int VERTICAL_PANEL_PIXEL_NUMBER = 16;
const int CHARACTER_WIDTH = 6;
const int CHARACTER_HEIGHT = 8;
const int MARGIN_IN_PX = 3;
const int ONE = 1;
const int ZERO = 0;

const int TEAM_NUBMER = 2;

SPIDMD dmd(HORIZONTAL_PANEL_NUMBER, VERTICAL_PANEL_NUMBER); // DMD controls the entire display

int SCORE[2] = {0, 0};
int TIME[2] = {0, 0};
int BOMBTIME1[2] = {0,0};
int BOMBTIME2[2] = {0,0};
bool isTime = false;
bool isFirstBomb = false;
bool isSecondBomb = false;
bool game_end = false;
long prevMillis = -1;
long prevMillis1 = -1;
long prevMillis2 = -1;
long game_end_sec = 0;

long last_sec = 0;
String jsonStr = "";
DynamicJsonDocument json(1024);

void refresh() {
  dmd.clearScreen();
  int x = ZERO;
  int y = ZERO;
  int width = HORIZONTAL_PANEL_PIXEL_NUMBER * HORIZONTAL_PANEL_NUMBER - ONE;
  int height = VERTICAL_PANEL_PIXEL_NUMBER * VERTICAL_PANEL_NUMBER - ONE;
  if (game_end){
    if (game_end_sec % 2 == 0){
      dmd.drawBox(width, height, x, y,GRAPHICS_OFF);
    } else {
      dmd.drawBox(width, height, x, y,GRAPHICS_ON);
    }
  }else {
    dmd.drawBox(width, height, x, y);
  }
  printScore(ZERO, ONE, Droid_Sans_24);
  printScore(ONE, 3, Droid_Sans_24);
  printBombTime1(Droid_Sans_16);
  printBombTime2(Droid_Sans_16);
  printTime(Droid_Sans_16);
}

void printBombTime1(const uint8_t* font) {
  dmd.drawFilledBox(HORIZONTAL_PANEL_PIXEL_NUMBER - CHARACTER_WIDTH * 3, VERTICAL_PANEL_PIXEL_NUMBER * 2.5, HORIZONTAL_PANEL_PIXEL_NUMBER * 2 - 2 * CHARACTER_WIDTH, VERTICAL_PANEL_PIXEL_NUMBER * 2.5 + CHARACTER_HEIGHT * 1.5, GRAPHICS_OFF);
  dmd.selectFont(font);
  
  int x10 = HORIZONTAL_PANEL_PIXEL_NUMBER - CHARACTER_WIDTH * 3;
  int x11 = HORIZONTAL_PANEL_PIXEL_NUMBER;
  
  int y = VERTICAL_PANEL_PIXEL_NUMBER * 2.5;
  String dot = (BOMBTIME1[1] % 2 == 0) ? ":":"";
  char buffer0[40];
  sprintf(buffer0, "%02d%s", BOMBTIME1[0], dot);
  
  char buffer1[40];
  sprintf(buffer1, "%02d", BOMBTIME1[1]);
  dmd.drawString(x10, y, buffer0);
  dmd.drawString(x11, y, buffer1);
}

void printBombTime2(const uint8_t* font) {
  dmd.drawFilledBox(HORIZONTAL_PANEL_PIXEL_NUMBER * 3 - CHARACTER_WIDTH * 3, VERTICAL_PANEL_PIXEL_NUMBER * 2.5, HORIZONTAL_PANEL_PIXEL_NUMBER * 4 - 2 * CHARACTER_WIDTH, VERTICAL_PANEL_PIXEL_NUMBER * 2.5 + CHARACTER_HEIGHT * 1.5, GRAPHICS_OFF);
  dmd.selectFont(font);

  int x20 = HORIZONTAL_PANEL_PIXEL_NUMBER * 3 - CHARACTER_WIDTH * 3;
  int x21 = HORIZONTAL_PANEL_PIXEL_NUMBER * 3;

  int y = VERTICAL_PANEL_PIXEL_NUMBER * 2.5;

  String dot = (BOMBTIME2[1] % 2 == 0) ? ":":"";
  char buffer0[40];
  sprintf(buffer0, "%02d%s", BOMBTIME2[0], dot);
  
  char buffer1[40];
  sprintf(buffer1, "%02d", BOMBTIME2[1]);
  dmd.drawString(x20, y, buffer0);
  dmd.drawString(x21, y, buffer1);

}

void printTime(const uint8_t* font) {
  dmd.drawFilledBox(HORIZONTAL_PANEL_PIXEL_NUMBER * 2 - CHARACTER_WIDTH * 3, MARGIN_IN_PX, HORIZONTAL_PANEL_PIXEL_NUMBER * 3 - 2 * CHARACTER_WIDTH, MARGIN_IN_PX + CHARACTER_HEIGHT * 1.5, GRAPHICS_OFF);
  dmd.selectFont(font);
  String dot = (TIME[1] % 2 == 0) ? ":":"";
  
  char buffer[40];
  sprintf(buffer, "%02d%s", TIME[0], dot);

  int x = HORIZONTAL_PANEL_PIXEL_NUMBER * 2 - CHARACTER_WIDTH * 3;
  int y = MARGIN_IN_PX;
  dmd.drawString(x, y, buffer);

  sprintf(buffer, "%02d", TIME[1]);

  x = HORIZONTAL_PANEL_PIXEL_NUMBER * 2;
  dmd.drawString(x, y, buffer);
}

void printScore(int team_number, int k, const uint8_t* font) {
  dmd.selectFont(font);
  int score = SCORE[team_number];
  char buffer[40];
  sprintf(buffer, "%02d", score);
  int x = HORIZONTAL_PANEL_PIXEL_NUMBER * k - 12;
  int y = VERTICAL_PANEL_PIXEL_NUMBER + MARGIN_IN_PX;
  dmd.drawString(x, y, buffer);
}

void timeA() {
  if (isTime && prevMillis == -1) {
    prevMillis = millis();
  } else if (!isTime) {
    prevMillis = -1;
    TIME[1] = 0;
    TIME[0] = 0;
    printTime(Droid_Sans_16);
  }

  if (isFirstBomb && prevMillis1 == -1) {
    prevMillis1 = millis();
  } else if (!isFirstBomb) {
    prevMillis1 = -1;
    BOMBTIME1[1] = 0;
    BOMBTIME1[0] = 0;
    printBombTime1(Droid_Sans_16);
  }

  if (isSecondBomb && prevMillis2 == -1) {
    prevMillis2 = millis();
  } else if (!isSecondBomb) {
    prevMillis2 = -1;
    BOMBTIME2[1] = 0;
    BOMBTIME2[0] = 0;
    printBombTime2(Droid_Sans_16);
  }
}

void match() {
  
  SCORE[0] = 0;
  SCORE[1] = 0;

  last_sec = millis();
  
  timeA();
  refresh();
}

// the setup routine runs once when you press reset:
void setup() {
  
  Serial.begin(115200);
  Serial.println("hihihihih");

  dmd.begin();
  match();
}

void loop() {
  if(Serial.available()) {
    Serial.println("fadfasdawsdwa");
    jsonStr = Serial.readString();
    jsonStr.trim();
    Serial.println(jsonStr);
    
    deserializeJson(json, jsonStr);
    
    if (String(json["time"]).equals("true") && !isTime){
      isTime = true;
      prevMillis = String(json["time_num"]).toInt() * 60 * 1000;
      timeA();
    }

    SCORE[0] = String(json["num1"]).toInt();
    SCORE[1] = String(json["num2"]).toInt();

    if (String(json["bomb1"]).equals("true")){
      prevMillis1 = String(json["bomb_time"]).toInt() * 60 * 1000;
      isFirstBomb = true;
      isSecondBomb = false;
      timeA();
    }

    if (String(json["bomb2"]).equals("true")){
      prevMillis2 = String(json["bomb_time"]).toInt() * 60 * 1000;
      isFirstBomb = false;
      isSecondBomb = true;
      timeA();
    }

    if (String(json["reset"]).equals("true")){
      SCORE[0] = 0;
      SCORE[1] = 0;
      isFirstBomb = false;
      isSecondBomb = false;
      isTime = false;
      timeA();
    }

    if (String(json["stop"]).equals("true")){
      isTime = false;
      timeA();
    }
    
    refresh();
  }
  
  if (millis() - last_sec > 1000) {
    Serial.printf("%s - %s - %s - %s\n", String(game_end), String(isTime), String(game_end_sec), String(prevMillis));
    if (game_end) {
      if (game_end_sec==10) {
        SCORE[0] = 0;
        SCORE[1] = 0;
        isFirstBomb = false;
        isSecondBomb = false;
        isTime = false;
        timeA();
        game_end = false;
        game_end_sec = 0;
      }
      game_end_sec += 1;
      refresh();
    }
    if (isTime){
      int seconds = prevMillis / 1000;
      int minutes = seconds / 60;
      seconds %= 60;
      if (seconds == 0 && minutes == 0) {
        game_end = true;
        game_end_sec += 1;
        isTime = false;
        isFirstBomb = false;
        isSecondBomb = false;
        TIME[1] = seconds;
        TIME[0] = minutes;
        printTime(Droid_Sans_16);
        prevMillis -= 1000;
      } else if (!(seconds == TIME[1]) || !(minutes == TIME[0])) {
        TIME[1] = seconds;
        TIME[0] = minutes;
        game_end = false;
        game_end_sec = 0;
        printTime(Droid_Sans_16);
        prevMillis -= 1000;
      }
    }
    
    if (isFirstBomb) {
      int seconds = (prevMillis1) / 1000;
      int minutes = seconds / 60;
      seconds %= 60;
      if (seconds == 0 && minutes == 0) {
        game_end = true;
        game_end_sec += 1;
        isTime = false;
        isFirstBomb = false;
        isSecondBomb = false;
        BOMBTIME1[1] = seconds;
        BOMBTIME1[0] = minutes;
        printBombTime1(Droid_Sans_16);
        prevMillis1 -= 1000;
      } else if (!(seconds == BOMBTIME1[1]) || !(minutes == BOMBTIME1[0])) {
        BOMBTIME1[1] = seconds;
        BOMBTIME1[0] = minutes;
        game_end = false;
        game_end_sec = 0;
        printBombTime1(Droid_Sans_16);
        prevMillis1 -= 1000;
      } 
    }
  
    if (isSecondBomb) {
      int seconds = prevMillis2 / 1000;
      int minutes = seconds / 60;
      seconds %= 60;
  
      if (seconds == 0 && minutes == 0) {
        game_end = true;
        game_end_sec += 1;
        isTime = false;
        isFirstBomb = false;
        isSecondBomb = false;
        BOMBTIME2[1] = seconds;
        BOMBTIME2[0] = minutes;
        printBombTime2(Droid_Sans_16);
        prevMillis2 -= 1000;
      } else if (!(seconds == BOMBTIME2[1]) || !(minutes == BOMBTIME2[0])) {
        BOMBTIME2[1] = seconds;
        BOMBTIME2[0] = minutes;
        game_end = false;
        game_end_sec = 0;
        printBombTime2(Droid_Sans_16);
        prevMillis2 -= 1000;
      } 
    }
    last_sec = millis();
  }

}
