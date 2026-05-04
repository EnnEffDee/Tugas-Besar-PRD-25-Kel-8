#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

int buzzerPin = 8;

int input[6];
int password[6] = {1, 2, 3, 4, 5, 6};
int count = 0;
const int ArduinoItemID = 899123;

// rows x cols
const byte ROWS = 4;
const byte COLS = 4;

LiquidCrystal_I2C lcd(0x27, 16, 2);

char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {5, 4, 3, 2}; // R1,R2,R3,R4
byte colPins[COLS] = {A3, A2, A1, A0};   // C1,C2,C3,C4

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void printPIN(int arr[], int len) {
  lcd.clear();
  lcd.setCursor(0, 0);
  for (int i = 0; i < len; i++) {
    lcd.print("*");
  }
  return;
}

bool comparr(int arr1[], int arr2[], int len) {
  for (int i = 0; i < len; i++) {
    if (arr1[i] != arr2[i]) return false;
  }
  return true;
}

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
}

bool status = true;

void loop() {
  if (count == 6) {
    if (comparr(password, input, 6)) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Benar");
      tone(buzzerPin, 800, 300);
      delay(500);
      tone(buzzerPin, 1200, 500);
      delay(500);
      lcd.clear();
    } else {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Gagal");
      tone(buzzerPin, 400, 100);
      delay(150);
      tone(buzzerPin, 100, 600);
      delay(500);
      lcd.clear();
    }
    status = true;
    count = 0;
  }

  char key = keypad.getKey();
  if (key == 'B') {
    if (status) {
      status = false;
      count = 0;
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Mulai menerima");
      lcd.setCursor(0,1);
      lcd.print("input");
      delay(1000);
      lcd.clear();
      lcd.setCursor(0,0);
    }
  } else if (key == 'A' && count > 0) {
      count--;
      printPIN(input, count);
    }
  else if (key >= '0' && key <= '9' && count < 6) {
    if ( !status ) {
      input[count] = key - '0';
      count++;
      printPIN(input, count);
    } else {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Pencet tombol");
      lcd.setCursor(0,1);
      lcd.print("mulai dulu");
      delay(1000);
      lcd.clear();
      lcd.setCursor(0,0);
    }
  }

  // Handle response from PC if PIN not invalid
  // if (Serial.available()) {
  //   String response = Serial.readStringUntil('\n');
  //
  //   if (response.startsWith("VALID")) {
  //     lcd.clear();
  //     lcd.setCursor(0, 0);
  //     lcd.print("Success");
  //
  //     lcd.clear();
  //
  //     lcd.print("Masuk? ('#': Ya, '*': Tidak)");
  //     key = keypad.getKey();
  //     if (key == '#') {
  //       lcd.print("Berhasil");
  //       Serial.print("MASUK");
  //     } else {
  //       Serial.print("BATAL_MASUK");
  //     }
  //     Serial.println();
  //
  //     tone(buzzerPin, 800, 300);
  //     delay(500);
  //     tone(buzzerPin, 1200, 500);
  //     delay(500);
  //
  //     lcd.clear();
  //   } else if (response.startsWith("INVALID")) {
  //     lcd.clear();
  //     lcd.setCursor(0, 0);
  //     lcd.print("Wrong");
  //     Serial.print("SALAH");
  //     Serial.println();
  //
  //     tone(buzzerPin, 300, 300);
  //     delay(500);
  //     tone(buzzerPin, 100, 500);
  //     delay(500);
  //
  //     lcd.clear();
  //   }
  // }
}
