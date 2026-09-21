#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>
#include <WiFi.h>
#include <time.h>

#define TFT_MOSI 10
#define TFT_SCLK 8
#define TFT_CS 7
#define TFT_DC 6
#define TFT_RST -1

#define BUZZER 9

const int rows[3] = {0, 1, 2};
const int cols[3] = {5, 4, 3};

Adafruit_ST7789 tft(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

bool lastState[3][3] = {};
unsigned long lastPress = 0;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);

  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);

  for (int r = 0; r < 3; r++) {
    pinMode(rows[r], OUTPUT);
    digitalWrite(rows[r], HIGH);
  }

  for (int c = 0; c < 3; c++) {
    pinMode(cols[c], INPUT_PULLUP);
  }

  tft.init(76, 284);
  tft.setOffsets(82, 18);
  tft.invertDisplay(false);
  tft.setRotation(1);
  tft.fillScreen(ST77XX_BLACK);

  showBoot();
}

void loop() {
  // put your main code here, to run repeatedly:
  scanButtons();
}

void showBoot() {
  tft.fillScreen(ST77XX_BLACK);

  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(3);
  tft.setCursor(8, 8);
  tft.print("Sudo Jago");

  tft.setTextColor(ST77XX_BLUE);
  tft.setTextSize(1);
  tft.setCursor(8, 42);
  tft.print("koditraLabs");

  delay(1500);

  showClock();
}








