# Science and Engineering Fair

An Arduino-based assistive device for people who have difficulty speaking. Using a 16-key keypad and a small LCD, the user can show common phrases to people nearby, text a caregiver for help over a GSM cellular module, sound a buzzer to get attention, and set an alarm.

## Hardware

| Component | Connection |
| --- | --- |
| Arduino Mega 2560 | — |
| 4x4 membrane keypad | rows on pins 39, 37, 35, 33; columns on pins 31, 29, 27, 25 |
| 16x2 character LCD (HD44780) | RS 43, E 45, D4–D7 on 47, 49, 51, 53 |
| Real-time clock (DS1307/DS3231, I2C address `0x68`) | SDA/SCL |
| GSM module (e.g. SIM900) | serial on pins 2/3, power key on pin 8 |
| Piezo buzzer | pin 23 |

## Libraries

- [Keypad](https://github.com/Chris--A/Keypad)
- LiquidCrystal (bundled with the Arduino IDE)
- [uRTCLib](https://github.com/Naguissa/uRTCLib)
- SerialGSM

## Controls

| Key | Action |
| --- | --- |
| **A** | Pick a preset text message ("I need help", "I fell down", …). Press **A** to cycle, **#** to send, **\*** to cancel. Requires a saved phone number. |
| **B** | Enter the caregiver's 10-digit phone number. **\*** deletes a digit (or cancels when empty), **#** saves. |
| **C** | Cycle through phrases shown on the LCD ("Yes", "No", "I am hungry", …). |
| **D** | Toggle the buzzer on/off to get someone's attention. |
| **#** | Show the date and time. Press **#** again from the clock screen to set an alarm (HH:MM:SS, **A** toggles AM/PM, **#** saves). |
| **\*** | Silence a ringing alarm, or clear the saved alarm. |

A small icon in the bottom-right corner of the clock screen shows when an alarm is set.

## Building

Open `Science_Fair.ino` in the Arduino IDE, install the libraries above, select **Arduino Mega 2560**, and upload.
