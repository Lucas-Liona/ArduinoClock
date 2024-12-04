byte moon[] = {
  B00000,
  B01100,
  B11000,
  B11001,
  B11111,
  B01110,
  B00000,
  B00000
};

byte sun[] = {
  /*B00000,
  B10101,
  B01110,
  B11111,
  B01110,
  B10101,
  B00000,
  B00000*/
  B00000,
  B11111,
  B10001,
  B10101,
  B10001,
  B11111,
  B00000,
  B00000
};

byte bell[] = {
  B00000,
  B00100,
  B01110,
  B01110,
  B01110,
  B11111,
  B00100,
  B00000
};

byte cursor[] = {
  B00000,
  B00100,
  B00110,
  B11111,
  B11111,
  B00110,
  B00100,
  B00000
};

byte gear[] = {
  B00000,
  B10101,
  B01110,
  B11011,
  B01110,
  B10101,
  B00000,
  B00000
};

byte clocksymb[] = {
  B00000,
  B01110,
  B10101,
  B10101,
  B10011,
  B01110,
  B00000,
  B00000
};

byte rightArr[] = {
  B00000,
  B00000,
  B00100,
  B00010,
  B11111,
  B00010,
  B00100,
  B00000
};

byte leftArr[] = {
  B00000,
  B00000,
  B00100,
  B01000,
  B11111,
  B01000,
  B00100,
  B00000
};

byte upArr[] = {
  B00000,
  B00000,
  B00100,
  B01110,
  B10101,
  B00100,
  B00100,
  B00000
};

byte downArr[] = {
  B00000,
  B00000,
  B00100,
  B00100,
  B10101,
  B01110,
  B00100,
  B00000
};

#define LCD_Backlight 10 // 15. BL1 - Backlight +                                Emitter of 2N3904, Collector to VCC, Base to D10 via 10K resistor

#include "DS3231.h"
#include <LiquidCrystal.h>
#include <Wire.h>
#include "pitches.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int analogX = A0;
int analogY = A1;
int buttonPin = 2;
int buttonState = 0;
int analogDir;
int last = 0;

int melody[] = {NOTE_C5, NOTE_D5, NOTE_E5, NOTE_F5, NOTE_G5, NOTE_A5, NOTE_B5, NOTE_C6};

DS3231 clock1;
RTCDateTime dt;

LiquidCrystal lcd(7, 8, 9, 10, 11, 12); // initialize the library with the numbers of the interface pins

_Bool snooze = true;

int cursorXY[2];
int index = -1;

enum {CLOCK, SETTINGS, ALARM} state = CLOCK;

int alarmHr = 9;
int alarmMin = 0;

int bedtimeHr = 22;
int bedtimeMin = 0;

int doOnce = 1;

bool loops = true;

int curX, curY, destX, destY;
int ind;
int dirs[6];
const int setting_spaces[2][5] = {{6, 7, 11, 12, 14}, {8, 9, 13, 14}};
int min = 10000;

char Btxt[20];
char* ampm2;
char Atxt[20];
int standardTime = bedtimeHr;// % 12;
char* ampm;
char* noon;
int hr;
char sec[20];
char sec2[20];
 _Bool bedtime = false;

void setup() {

  lcd.begin(16, 2);  // set up the LCD's number of columns and rows:
  Serial.begin(9600);
  lcd.home();

  lcd.createChar(0, gear);
  lcd.createChar(1, leftArr);
  lcd.createChar(2, downArr);
  lcd.createChar(3, rightArr);
  lcd.createChar(4, upArr);
  lcd.createChar(5, clocksymb);
  lcd.createChar(6, moon);
  lcd.createChar(7, bell);

  clock1.begin();
  clock1.setDateTime(__DATE__, __TIME__);

  clock1.setAlarm1(0, alarmHr, alarmMin, 0, DS3231_MATCH_H_M_S); //Morning 8am
  clock1.setAlarm2(0, bedtimeHr, bedtimeMin, DS3231_MATCH_H_M);

  pinMode(analogX, INPUT);
  pinMode(analogY, INPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  pinMode(LCD_Backlight, OUTPUT); 
  analogWrite(LCD_Backlight, 64); // Set the brightness of the backlight
}

void loop() {

  dt = clock1.getDateTime();
  buttonState = digitalRead(buttonPin);
  analogDir = readInput(analogX, analogY);

  if(!snooze) 
  {
    tone(5, melody[0], 75);
  }

  if(bedtime)
  {
     tone(5, melody[7], 500);
     delay(500);
     tone(5, melody[5], 500);
     delay(500);
     tone(5, melody[3], 500);
     delay(500);
     tone(5, melody[1], 500);
     delay(500);
     tone(5, melody[2], 1000);
     delay(1000);

    bedtime = false;
  }
  
  if (clock1.isAlarm1()) 
  {
    snooze = false;
  }

  if(clock1.isAlarm2())
  {
    bedtime = true;
  }

  switch(state)
  {
    case ALARM:
      lcd.setCursor(0, 0);
      if (index == -1)
      { 
        int j;
        index = 0;
        for(int i = 0; i < 6; i++)
        {
          Serial.println(i);
          j = random(1, 5);
          dirs[i] = j;
          lcd.write(byte(j));
        }
      }
      if (doOnce == 1) 
      {
        Serial.println("DNDNDNAD");
        doOnce = 0;
        cursorXY[0] = 0;
        cursorXY[1] = 10;
        lcd.setCursor(cursorXY[0], cursorXY[1]);
        lcd.write("*");
      }

      if((analogDir != last) && (analogDir == dirs[index]))
        {
          index++;
          cursorXY[0] = cursorXY[0] + 1;
          lcd.setCursor(cursorXY[0], cursorXY[1]);
          lcd.write("*");
        }
          last = analogDir;

        if (index == 6)
        {
          Serial.print("DONE");
          index = -1;
          snooze = true;
          doOnce = 1;
          state = CLOCK;
          lcd.clear();
          return;
        }
        break;

    case CLOCK:
      if (dt.hour >= 12) {
        noon = "pm";
      } else {
        noon = "am";
      }
      if ((dt.hour % 12) == 0) {
        hr = 12;
      } else {
        hr = dt.hour % 12;
      }

      lcd.setCursor(0, 0);
      sprintf(sec, "%.2d/%.2d/%.2d", dt.month, dt.day, dt.year);
      lcd.print(sec); //YOU DONT NEED TO DO THIS EVERY SECOND

      lcd.setCursor(0, 1);
      sprintf(sec2, "%.2d%s:%.2d:%.2d ", hr, noon, dt.minute, dt.second); //YOU SHOULD PUT EVERYTHING INTO ONE SPRINTF
      lcd.print(sec2);

      if (bedtimeHr <= dt.hour || dt.hour < alarmHr)
      {
        lcd.write(byte(6));
      }

      lcd.setCursor(15, 0);
      lcd.write(byte(0)); //gear
      
      lcd.setCursor(15, 1);
      lcd.write(byte(7)); //bell
      break;

    case SETTINGS:
      lcd.setCursor(15, 0);
      lcd.write(byte(5)); //clock
        
      if (alarmHr >= 12) 
      {
        ampm = "pm";
      } 
      else 
      {
        ampm = "am";
      }
      lcd.setCursor(0, 0);  
      sprintf(Atxt, "Alarm %.2d%s:%.2d", alarmHr, ampm, alarmMin);
      lcd.write(Atxt);

      if (bedtimeHr >= 12) 
      {
        ampm2 = "pm";
      } 
      else 
      {
        ampm2 = "am";
      }
        
      lcd.setCursor(0, 1);
      sprintf(Btxt, "Bedtime %d%s:%.2d", standardTime, ampm2, bedtimeMin);
      lcd.write(Btxt);
  }

  if (loops)
  {
    switch(buttonState)
    {
      case 0:
        switch(state)
        {
          case CLOCK:
            switch(cursorXY[1])
            {
              case 0:
                state = SETTINGS;
                doOnce = 1;
                lcd.clear();
                break;
              case 1:
                state = ALARM;
                doOnce = 1;
                lcd.clear();
                break;
            }
            break;
          case ALARM:
            //do nothing?
            break;
          case SETTINGS:
            
            if (cursorXY[0] == 14 && cursorXY[1] == 0)
            {
              clock1.clearAlarm1();
              clock1.clearAlarm2();
              clock1.setAlarm1(0, alarmHr, alarmMin, 0, DS3231_MATCH_H_M_S); //Morning 8am
              clock1.setAlarm2(0, bedtimeHr, bedtimeMin, DS3231_MATCH_H_M);
              state = CLOCK;
              doOnce = 1;
              lcd.clear();
            }
            else
            {
              //loops = false;
              select(cursorXY[0], cursorXY[1], Atxt, Btxt);
            }
            break;
        }
        loops = false;
        return;
        break;
    }
  } 
  else if (buttonState == 1)
  {
    loops = true;
  }


  //draw hud lowkey

  //draw main

  //cursor
  if (last != analogDir)
  {
    switch(analogDir)
    {
      case 1:
        switch(state)
        {
          case CLOCK:
            lcd.setCursor(cursorXY[0], cursorXY[1]);
            lcd.write("*");
            break;
          case ALARM:
            break;
          case SETTINGS:
            curX = cursorXY[0];
            curY = cursorXY[1];
            destY = curY;

            ind = 0;

            for(int i = 0; setting_spaces[curY][i] != curX; i++) 
            {
              ind = i;
            }

            destX = setting_spaces[curY][ind];
            cursorXY[0] = destX;
            cursorXY[1] = destY;
            lcd.setCursor(cursorXY[0], cursorXY[1]);
            lcd.write("*");
            break;
        }
        break;
    
      case 2:
        switch(state)
          {
            case CLOCK:
              cursorXY[1] = cursorXY[1] + 1;
              if (cursorXY[1] > 1)
              {
                cursorXY[1] = 1;
              }
              lcd.setCursor(cursorXY[0], cursorXY[1]);
              lcd.write("*");
              break;
            case ALARM:
              break;
            case SETTINGS:
              curX = cursorXY[0];
              curY = cursorXY[1];

              ind = 0;
              
              for (int i = 0; i < 4; i++)
              {
                if (abs(setting_spaces[1][i] - curX) < min)
                {
                  min = abs(setting_spaces[1][i] - curX);
                  ind = i;
                }
              }

              destY = 1;
              destX = setting_spaces[1][ind];
              cursorXY[0] = destX;
              cursorXY[1] = destY;
              lcd.setCursor(cursorXY[0], cursorXY[1]);
              lcd.write("*");
              min = 1000;
              break;
          }
          break;

      case 3:
        switch(state)
        {
          case CLOCK:
            lcd.setCursor(cursorXY[0], cursorXY[1]);
            lcd.write("*");
            break;
          case ALARM:
            break;
          case SETTINGS:
            curX = cursorXY[0];
            curY = cursorXY[1];
            destY = curY;
            ind = 1; //default value of 1 cause when your on the 0th index the for loop wont even run thus ind will never be set

            for(int i = 0; setting_spaces[curY][i] != curX; i++) 
            {
              Serial.println(i);
              ind = i+2;
            }
            if (ind > (4 - curY)) //needs to update for size
            {
              ind = 4 - curY;
            }
            destX = setting_spaces[curY][ind];
            cursorXY[0] = destX;
            cursorXY[1] = destY;
            lcd.setCursor(cursorXY[0], cursorXY[1]);
            lcd.write("*");
            break;
        }
        break;

      case 4:
        switch(state)
          {
            case CLOCK:
              cursorXY[1] = cursorXY[1] - 1;
              if (cursorXY[1] < 0)
              {
                cursorXY[1] = 0;
              }
              lcd.setCursor(cursorXY[0], cursorXY[1]);
              lcd.write("*");
              break;
            case ALARM:
              break;
            case SETTINGS:
              curX = cursorXY[0];
              curY = cursorXY[1];
  
              ind = 0;
                
              for (int i = 0; i < 5; i++)
              {
                if (abs(setting_spaces[0][i] - curX) < min)
                {
                  min = abs(setting_spaces[0][i] - curX);
                  ind = i;
                }
              }

              destY = 0;
              destX = setting_spaces[0][ind];
              cursorXY[0] = destX;
              cursorXY[1] = destY;
              lcd.setCursor(cursorXY[0], cursorXY[1]);
              lcd.write("*");
              min = 1000;
              break;
          }
          break;
    }
  } 
  else //if (analogDir == 0)
  {
    switch(state)
    {
      case CLOCK:
        if (doOnce == 1)
        {
          doOnce = 0;
          cursorXY[0] = 14;
          cursorXY[1] = 0;
          lcd.setCursor(cursorXY[0], cursorXY[1]);
          lcd.write("*");
        }
        lcd.setCursor(cursorXY[0], cursorXY[1]);
        lcd.write("*");
        break;

      case ALARM:
    
        break;

      case SETTINGS:
        if (doOnce == 1)
        {
          doOnce = 0;
          cursorXY[0] = 14;
          cursorXY[1] = 0;
          lcd.setCursor(cursorXY[0], cursorXY[1]);
          lcd.write("*");
        }
        lcd.setCursor(cursorXY[0], cursorXY[1]);
        lcd.write("*");
        break;
    }

  }
  last = analogDir;

  delay(50);

  if(state != ALARM)
  {
  lcd.setCursor(cursorXY[0], cursorXY[1]);
  lcd.write(" ");
  }
}

void select(int x, int y, char* Atxt, char* Btxt)
{
  int number;

  switch(y)
  {
    case 0:
      number = Atxt[x] - '0';
      break;
    case 1:
      number = Btxt[x] - '0';
      break;
  }

  buttonState = 1;
  analogDir = readInput(analogX, analogY);

  while(buttonState != 0)
  {
    delay(100);

    buttonState = digitalRead(buttonPin);
    analogDir = readInput(analogX, analogY);
    switch(analogDir)
    {
      case 2:
        number = number - 1;
        if (number < 0)
        {
          number = 0;
        }
        lcd.setCursor(x, y);
        lcd.write(number + '0');
        break;
      case 4:
        number++;
        if (number > 9)
        {
          number = 9;
        }
        lcd.setCursor(x, y);
        lcd.write(number + '0');
        break;
    }

    switch(y)
    {
      case 0:
        switch(x)
        {
          case 6:
            alarmHr = number*10 + (alarmHr % 10);
            break;
          case 7:
            alarmHr = (alarmHr/10)*10 + number;
            break;
          case 11:
            alarmMin = number*10 + (alarmMin % 10);
            break;          
          case 12:
            alarmMin = (alarmMin/10)*10 + number;
            break;            
        }
        break;
      case 1:
        switch(x)
        {
          case 8:
            bedtimeHr = number*10 + (bedtimeHr % 10);
            break;          
          case 9:
            bedtimeHr = (bedtimeHr/10)*10 + number;
            break;          
          case 13:
            bedtimeMin = number*10 + (bedtimeMin % 10);
             break;         
          case 14:
            bedtimeMin = (bedtimeMin/10)*10 + number;
            break;          
        }
        break;
    }
    //RESET ALARMS
  }

  return;
}
int readInput(int ax, int ay) 
{
  int xPos = analogRead(ax);
  int yPos = analogRead(ay);
  const int center = 511;
  const int buffer = 50;


    if (xPos < (center - 50))
    {
      return 1;
    }
    else if (xPos > (center + 50))
    {
      return 3;
    }
    else if (yPos < (center - 50))
    {
      return 4;
    }
    else if (yPos > (center + 50))
    {
      return 2;
    }
    return 0;
}

