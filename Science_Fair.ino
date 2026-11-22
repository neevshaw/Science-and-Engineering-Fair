#include <SerialGSM.h>
#include <Keypad.h>
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
#include "uRTCLib.h"


const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {39, 37, 35, 33};
byte colPins[COLS] = {31, 29, 27, 25};


Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
LiquidCrystal lcd(43, 45, 47, 49, 51, 53);
uRTCLib rtc(0x68);
SerialGSM cell(2,3);


String mes[8] = {"Yes", "No", "Maybe", "I am hungry", "I am thirsty", "I love you", "I am hot", "I am cold"};
String texts[6] = {"Hello!", "I need help", "Please come quickly", "I fell down", "Thank you!", "I love you"};
String phone = "";
String old_phone = "";
String alarm_time = "";
String old_alarm_time = "";
int mes_index = 0;
int texts_index = 0;
int hour;
int minute;
int second;
boolean buzz_state = false;
boolean back_mes = false;
boolean dele = false;
boolean back_phone = false;
boolean time = false;
boolean alarm = false;
boolean dele_alarm = false;
boolean back_alarm = false;
boolean alarm_off = true;
boolean am = true;
char new_key;
char phone_key;
char alarm_key;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
byte alarm_icon[8] = {B01100, B10010, B10010, B01100, B00000, B00000, B00000, B00000};

void sendSMS() {
  cell.Message(texts[texts_index].c_str());
  cell.SendSMS();
}

void display_time() {
  lcd.clear();
  lcd.setCursor(0,0);

  lcd.print(rtc.month());
  lcd.print('/');
  lcd.print(rtc.day());
  lcd.print('/');
  lcd.print(rtc.year());
  lcd.print(" ");

  hour = int(rtc.hour());
  minute = int(rtc.minute());
  second = int(rtc.second());

  if (hour > 12) {
    hour = hour - 12;
  }
  else if (hour == 0) {
    hour = 12;
  }
  lcd.print(hour);
  lcd.print(':');
  if (minute < 10) {
    lcd.print("0");
  }
  lcd.print(minute);
  lcd.print(':');
  if (second < 10) {
    lcd.print("0");
  }
  lcd.print(second);

  lcd.setCursor(0,1);
  lcd.print("(");
  lcd.print(daysOfTheWeek[rtc.dayOfWeek()-1]);
  lcd.print(")");

  if (alarm_time.length() == 6) {
    lcd.setCursor(15,1);
    lcd.write(byte(0));
  }
}

void setup() {
  pinMode(8, OUTPUT);
  pinMode(23, OUTPUT);
  URTCLIB_WIRE.begin();
  lcd.begin(16, 2);
  lcd.createChar(0, alarm_icon);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Loading...");
  Serial.begin(9600);
  cell.begin(4800);
  cell.Verbose(true);
  cell.Boot();
  cell.FwdSMS2Serial();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Ready to start!");
}

void loop() {
  char key = keypad.getKey();
  if(alarm_time.length() == 6) {
    rtc.refresh();
    Serial.println(alarm_time);
    if (alarm_time.substring(0,2).toInt() == int(rtc.hour()) && alarm_time.substring(2,4).toInt() == int(rtc.minute()) && alarm_time.substring(4).toInt() == int(rtc.second())) {
      tone(23, 500);
      alarm_off = false;
    }
  }
  if (time) {
    rtc.refresh();
    if (second != rtc.second()) {
      display_time();
    }
  }
  if (key == 'A') {
    alarm = false;
    time = false;
    if (phone.length() < 10) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Please enter a");
      lcd.setCursor(0,1);
      lcd.print("phone number");
    }
    else {
      Serial.println("A pressed");
      back_mes = false;
      texts_index = 0;
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(texts[0]);
      new_key = 0;
      while (new_key != '#' && new_key != '*') {
        new_key = keypad.getKey();
        if (new_key == 'A') {
          texts_index++;
          if (texts_index > 5) {
            texts_index = 0;
          }
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print(texts[texts_index]);
        }
        else if (new_key == '*') {
          back_mes = true;
        }
      }
      if (!back_mes) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Connecting to");
        lcd.setCursor(0,1);
        lcd.print("device");
        digitalWrite(8, HIGH);
        delay(2000);
        digitalWrite(8, LOW);
        delay(14000);
        sendSMS();
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Sent!");
        digitalWrite(8, HIGH);
        delay(2000);
        digitalWrite(8, LOW);
      }
      else {
        lcd.clear();
        time = true;
      }
    }
  }
  else if (key == 'B') {
    alarm = false;
    time = false;
    Serial.println("B pressed");
    phone = "";
    dele = false;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Enter phone #");
    back_phone = false;
    while (phone.length() != 10 || phone_key != '#') {
      dele = false;
      phone_key = 0;
      while (phone_key != '1' && phone_key != '2' && phone_key != '3' && phone_key != '4' && phone_key != '5' && phone_key != '6' && phone_key != '7' && phone_key != '8' && phone_key != '9' && phone_key != '0' && phone_key != '*' && phone_key != '#') {
        phone_key = keypad.getKey();
        if (phone_key == '*') {
          dele = true;
        }
      }
      if (!dele && phone.length() < 10 && phone_key != '#') {
        phone = phone + phone_key;
        lcd.setCursor(0,1);
        lcd.print(phone);
      }
      else if (dele && phone.length() > 0) {
        phone.remove(phone.length() - 1);
        lcd.setCursor(0,1);
        lcd.print("          ");
        lcd.setCursor(0,1);
        lcd.print(phone);
      }
      else if (dele && phone.length() == 0) {
        back_phone = true;
        break;
      }
    }
    if (!back_phone) {
      cell.Rcpt(phone.c_str());
      lcd.clear();
      lcd.setCursor(0,0);
      old_phone = phone;
      lcd.print("Phone # saved!");
    }
    else {
      lcd.clear();
      time = true;
      phone = old_phone;
    }
  }
  else if (key == 'C') {
    alarm = false;
    time = false;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(mes[mes_index]);
    Serial.println("C pressed");
    mes_index++;
    if (mes_index > 7) {
      mes_index = 0;
    }
  }
  else if (key == 'D') {
    alarm = false;
    time = false;
    Serial.println("D pressed");
    if (!buzz_state) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Starting Tone");
      tone(23, 440);

    }
    else {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Ending Tone");
      noTone(23);
    }
    buzz_state = !buzz_state;
  }
  else if (key == '#') {
    if (alarm == true) {
      time = false;
      dele_alarm = false;
      back_alarm = false;
      am = true;
      lcd.clear();
      alarm_time = "";
      lcd.setCursor(0,0);
      lcd.print("Enter alarm time");
      lcd.setCursor(0,1);
      lcd.print("  :  :   AM");
      while (alarm_time.length() != 6  || alarm_key != '#') {
        dele_alarm = false;
        alarm_key = 0;
        while ((alarm_key != '0' || (alarm_time[0] == '0' && alarm_time.length() == 1)) && alarm_key != '1' && (alarm_key != '2' || alarm_time.length() < 1) && ((alarm_key != '3' && alarm_key != '4' && alarm_key != '5') || (alarm_time.length() < 2 && (alarm_time.length() != 1 || alarm_time[0] != '0'))) && ((alarm_key != '6' && alarm_key != '7' && alarm_key != '8' && alarm_key != '9') || ((alarm_time.length() != 1 || alarm_time[0] != '0') && alarm_time.length() != 3 && alarm_time.length() != 5)) && alarm_key != '*' && alarm_key != '#' && alarm_key != 'A') {
          alarm_key = keypad.getKey();
          if (alarm_key == '*') {
            dele_alarm = true;
          }
        }
        if (alarm_key == 'A') {
          if (am) {
            if (alarm_time.length() < 3) {
              lcd.setCursor(0,1);
              lcd.print("  :  :  ");
              lcd.setCursor(0,1);
              lcd.print(alarm_time);
            }
            else if (alarm_time.length() < 5) {
              lcd.setCursor(0,1);
              lcd.print("  :  :  ");
              lcd.setCursor(0,1);
              lcd.print(alarm_time.substring(0,2));
              lcd.setCursor(3,1);
              lcd.print(alarm_time.substring(2));
            }
            else if (alarm_time.length() < 7) {
              lcd.setCursor(0,1);
              lcd.print("  :  :  ");
              lcd.setCursor(0,1);
              lcd.print(alarm_time.substring(0,2));
              lcd.setCursor(3,1);
              lcd.print(alarm_time.substring(2,4));
              lcd.setCursor(6,1);
              lcd.print(alarm_time.substring(4));
            }
            lcd.setCursor(9,1);
            lcd.print("PM");
            am = false;
          }
          else {
            if (alarm_time.length() < 3) {
              lcd.setCursor(0,1);
              lcd.print("  :  :  ");
              lcd.setCursor(0,1);
              lcd.print(alarm_time);
            }
            else if (alarm_time.length() < 5) {
              lcd.setCursor(0,1);
              lcd.print("  :  :  ");
              lcd.setCursor(0,1);
              lcd.print(alarm_time.substring(0,2));
              lcd.setCursor(3,1);
              lcd.print(alarm_time.substring(2));
            }
            else if (alarm_time.length() < 7) {
              lcd.setCursor(0,1);
              lcd.print("  :  :  ");
              lcd.setCursor(0,1);
              lcd.print(alarm_time.substring(0,2));
              lcd.setCursor(3,1);
              lcd.print(alarm_time.substring(2,4));
              lcd.setCursor(6,1);
              lcd.print(alarm_time.substring(4));
            }
            lcd.setCursor(9,1);
            lcd.print("AM");
            am = true;
          }
          Serial.println(am);
        }
        if (!dele_alarm && alarm_time.length() < 6 && alarm_key != '#' && alarm_key != 'A') {
          alarm_time = alarm_time + alarm_key;
          if (alarm_time.length() < 3) {
            lcd.setCursor(0,1);
            lcd.print("  :  :  ");
            lcd.setCursor(0,1);
            lcd.print(alarm_time);
          }
          else if (alarm_time.length() < 5) {
            lcd.setCursor(0,1);
            lcd.print("  :  :  ");
            lcd.setCursor(0,1);
            lcd.print(alarm_time.substring(0,2));
            lcd.setCursor(3,1);
            lcd.print(alarm_time.substring(2));
          }
          else if (alarm_time.length() < 7) {
            lcd.setCursor(0,1);
            lcd.print("  :  :  ");
            lcd.setCursor(0,1);
            lcd.print(alarm_time.substring(0,2));
            lcd.setCursor(3,1);
            lcd.print(alarm_time.substring(2,4));
            lcd.setCursor(6,1);
            lcd.print(alarm_time.substring(4));
          }
        }
        else if (dele_alarm && alarm_time.length() > 0) {
          alarm_time.remove(alarm_time.length() - 1);
          if (alarm_time.length() < 3) {
            lcd.setCursor(0,1);
            lcd.print("  :  :  ");
            lcd.setCursor(0,1);
            lcd.print(alarm_time);
          }
          else if (alarm_time.length() < 5) {
            lcd.setCursor(0,1);
            lcd.print("  :  :  ");
            lcd.setCursor(0,1);
            lcd.print(alarm_time.substring(0,2));
            lcd.setCursor(3,1);
            lcd.print(alarm_time.substring(2));
          }
          else if (alarm_time.length() < 7) {
            lcd.setCursor(0,1);
            lcd.print("  :  :  ");
            lcd.setCursor(0,1);
            lcd.print(alarm_time.substring(0,2));
            lcd.setCursor(3,1);
            lcd.print(alarm_time.substring(2,4));
            lcd.setCursor(6,1);
            lcd.print(alarm_time.substring(4));
          }
        }
        else if (dele_alarm && alarm_time.length() == 0) {
          back_alarm = true;
          break;
        }
      }
      if (!back_alarm) {
        if (am == false && alarm_time.substring(0,2).toInt() > 0 && alarm_time.substring(0,2).toInt() < 12) {
          String hour24 = String(alarm_time.substring(0,2).toInt() + 12);
          alarm_time[0] = hour24[0];
          alarm_time[1] = hour24[1];
        }
        else if (am == true && alarm_time.substring(0,2).toInt() == 12) {
          alarm_time[0] = '0';
          alarm_time[1] = '0';
        }
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Alarm saved!");
        alarm = false;
        alarm_off = true;
        old_alarm_time = alarm_time;
      }
      else {
        lcd.clear();
        alarm_time = old_alarm_time;
        time = true;
      }
    }
    else {
      Serial.println("# pressed");
      texts_index = 0;
      mes_index = 0;
      lcd.clear();
      time = true;
      alarm = true;
    }
  }
  else if (key == '*') {
    if (alarm_off) {
      alarm_time = "";
      old_alarm_time = "";
    }
    else {
      noTone(23);
      alarm_off = true;
    }

  }
}