#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

int buzzerPin = 8;
int relayPin = 7;

unsigned long start;
unsigned long inputStart = 0;
const unsigned long INPUT_TIMEOUT = 15000; // 15 detik
int i;
int input[6];
int count = 0;
String pinstr = "";
String access_mode = "";
String ArduinoItemID = "899123";

// rows x cols
const byte ROWS = 4;
const byte COLS = 4;

LiquidCrystal_I2C lcd(0x27, 16, 2);

char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

byte rowPins[ROWS] = { 5, 4, 3, 2 };      // R1,R2,R3,R4
byte colPins[COLS] = { A3, A2, A1, A0 };  // C1,C2,C3,C4

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void printPIN(int arr[], int len) {
  lcd.clear();
  lcd.setCursor(0, 0);
  for (int i = 0; i < len; i++) {
    lcd.print("*");
  }
  lcd.setCursor(0, 1);
  lcd.print("MODE: ");
  lcd.print(access_mode);

  return;
}

void setup() {
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
}

bool status = true;

void loop() {
  if (count == 6) {
    pinstr = "";

    for (int i = 0; i < 6; i++) {
      pinstr += String(input[i]);
    }

    Serial.println("PIN," + ArduinoItemID + "," + pinstr + "," + access_mode);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Checking...");

    start = millis();
    while (!Serial.available()) {
      if (millis() - start > 5000) {
        lcd.clear();
        lcd.print("TIMEOUT");
        delay(1500);
        count = 0;
        pinstr = "";
        return;
      }
    }

    String response = Serial.readStringUntil('\n');
    response.trim();

    if (response == "VALID") {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("SUCCESS");

      if (access_mode == "MASUK") {
        tone(buzzerPin, 1000, 150);
        delay(200);
        tone(buzzerPin, 1500, 150);
        delay(200);
      } else {
        tone(buzzerPin, 500, 600);
        delay(650);
      }
    } else {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("ACCESS DENIED");

      tone(buzzerPin, 300, 300);
      delay(500);
      tone(buzzerPin, 100, 500);
    }
    delay(1500);
    lcd.clear();
    status = true;
    count = 0;
    pinstr = "";
    // actuate the solenoid
    digitalWrite(relayPin, HIGH);
    delay(10000);
    digitalWrite(relayPin, LOW);
  }

  char key = keypad.getKey();
  // Timeout input PIN setelah tekan B/C
  if (!status && inputStart > 0 && millis() - inputStart > INPUT_TIMEOUT) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("TIMEOUT");
    tone(buzzerPin, 200, 600);
    delay(1500);
    lcd.clear();
    count = 0;
    pinstr = "";
    access_mode = "";
    inputStart = 0;
    status = true;
  }

  if (key == 'B') {
    access_mode = "MASUK";
    status = false;
    count = 0;
    inputStart = millis();

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Masukkan PIN");
    lcd.setCursor(0, 1);
    lcd.print("MODE: ");
    lcd.print(access_mode);

    delay(1000);
    lcd.clear();
  } else if (key == 'C') {
    access_mode = "KELUAR";
    status = false;
    count = 0;
    inputStart = millis();

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Masukkan PIN");
    lcd.setCursor(0, 1);
    lcd.print("MODE: ");
    lcd.print(access_mode);

    delay(1000);
    lcd.clear();
  } else if (key == 'A' && count > 0) {
    count--;
    printPIN(input, count);
  } else if (key >= '0' && key <= '9' && count < 6) {
    if (!status) {
      input[count] = key - '0';
      count++;
      printPIN(input, count);
    } else {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Pencet tombol");
      lcd.setCursor(0, 1);
      lcd.print("mulai dulu");
      delay(1000);
      lcd.clear();
      lcd.setCursor(0, 0);
    }
  }
}
