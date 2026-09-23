#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <Preferences.h>
#include <time.h>

#define TFT_MOSI 10
#define TFT_SCLK 8
#define TFT_CS 7
#define TFT_DC 6
#define TFT_RST -1
#define BUZZER 9

const int rows[3] = {0, 1, 2};
const int cols[3] = {5, 4, 3};

const int buttons[3][3] = {
  {1, 8, 7},
  {2, 9, 6},
  {3, 4, 5}
};

Adafruit_ST7789 tft(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);
WebServer server(80);
DNSServer dns;
Preferences prefs;

bool last[3][3] = {};
bool alarmActive = false;
bool alarmSet = false;
bool buzz = false;

int alarmHour, alarmMinute;
int sequence[4];
int position = 0;

unsigned long lastPress = 0;
unsigned long lastBuzz = 0;
int lastAlarmMinute = -1;

void setup() {
  Serial.begin(115200);

  for (int r = 0; r < 3; r++) {
    pinMode(rows[r], OUTPUT);
    digitalWrite(rows[r], HIGH);
  }

  for (int c = 0; c < 3; c++)
    pinMode(cols[c], INPUT_PULLUP);

  pinMode(BUZZER, OUTPUT);

  tft.init(76, 284);
  tft.setRotation(1);
  tft.fillScreen(ST77XX_BLACK);

  prefs.begin("jago", false);
  alarmHour = prefs.getInt("hour", 7);
  alarmMinute = prefs.getInt("min", 0);
  alarmSet = prefs.getBool("set", false);

  WiFi.mode(WIFI_AP);
  WiFi.softAP("sudo jago", "sudojago");

  dns.start(53, "*", WiFi.softAPIP());

  server.on("/", portal);
  server.on("/clock", HTTP_POST, setClock);
  server.on("/alarm", HTTP_POST, setAlarm);
  server.on("/off", HTTP_POST, disableAlarm);
  server.onNotFound(portal);
  server.begin();

  Serial.print("Portal: http://");
  Serial.println(WiFi.softAPIP());

  showReady();
}

void loop() {
  dns.processNextRequest();
  server.handleClient();
  scanButtons();

  if (alarmActive)
    updateBuzzer();

  checkAlarm();
}

void portal() {
  String page =
    "<html><meta name='viewport' content='width=device-width'>"
    "<style>body{font-family:Arial;background:#111;color:white;"
    "max-width:400px;margin:40px auto;padding:20px}"
    "input,button{width:100%;padding:14px;margin:8px 0;font-size:18px}"
    "button{border:0;border-radius:8px}</style>"
    "<h1>sudo jago</h1>"
    "<p>Set the clock and alarm.</p>"
    "<button onclick='clock()'>Set Clock</button>"
    "<input id='alarm' type='time'>"
    "<button onclick='setAlarm()'>Set Alarm</button>"
    "<button onclick='off()'>Disable Alarm</button>"
    "<script>"
    "function post(u,d){fetch(u,{method:'POST',body:d}).then(()=>location.reload())}"
    "function clock(){post('/clock',Date.now().toString())}"
    "function setAlarm(){post('/alarm',document.getElementById('alarm').value)}"
    "function off(){post('/off','')}"
    "</script></html>";

  server.send(200, "text/html", page);
}

void setClock() {
  unsigned long long ms = server.arg("plain").toInt();
  time_t now = ms / 1000;

  timeval tv = {now, 0};
  settimeofday(&tv, nullptr);

  server.send(200, "text/plain", "OK");
}

void setAlarm() {
  String t = server.arg("plain");

  if (t.length() >= 5) {
    alarmHour = t.substring(0, 2).toInt();
    alarmMinute = t.substring(3, 5).toInt();

    alarmSet = true;

    prefs.putInt("hour", alarmHour);
    prefs.putInt("min", alarmMinute);
    prefs.putBool("set", true);
  }

  server.send(200, "text/plain", "OK");
  showReady();
}

void disableAlarm() {
  alarmSet = false;
  prefs.putBool("set", false);

  server.send(200, "text/plain", "OK");
  showReady();
}

void checkAlarm() {
  if (!alarmSet || alarmActive)
    return;

  tm timeinfo;

  if (!getLocalTime(&timeinfo, 5))
    return;

  if (timeinfo.tm_hour == alarmHour &&
      timeinfo.tm_min == alarmMinute &&
      lastAlarmMinute != timeinfo.tm_min) {

    lastAlarmMinute = timeinfo.tm_min;
    startAlarm();
  }
}

void startAlarm() {
  alarmActive = true;
  position = 0;

  for (int i = 0; i < 4; i++)
    sequence[i] = random(1, 10);

  showAlarm();

  Serial.print("Sequence: ");

  for (int i = 0; i < 4; i++) {
    Serial.print(sequence[i]);
    Serial.print(" ");
  }

  Serial.println();
}

void stopAlarm() {
  alarmActive = false;
  noTone(BUZZER);
  showReady();
}

void updateBuzzer() {
  if (millis() - lastBuzz < 300)
    return;

  lastBuzz = millis();
  buzz = !buzz;

  if (buzz)
    tone(BUZZER, 2200);
  else
    noTone(BUZZER);
}

void scanButtons() {
  for (int r = 0; r < 3; r++) {
    for (int i = 0; i < 3; i++)
      digitalWrite(rows[i], HIGH);

    digitalWrite(rows[r], LOW);

    for (int c = 0; c < 3; c++) {
      bool pressed = digitalRead(cols[c]) == LOW;

      if (pressed && !last[r][c] &&
          millis() - lastPress > 100) {
        lastPress = millis();
        handleButton(buttons[r][c]);
      }

      last[r][c] = pressed;
    }
  }
}

void handleButton(int button) {
  if (!alarmActive)
    return;

  if (button == sequence[position]) {
    position++;

    if (position == 4)
      stopAlarm();
    else
      showAlarm();
  } else {
    position = 0;
    showAlarm();
  }
}

void showReady() {
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE);

  tft.setTextSize(3);
  tft.setCursor(92, 5);
  tft.print("BLARE");

  tft.setTextSize(1);
  tft.setCursor(91, 38);
  tft.print("sudo jago");

  tft.setCursor(10, 60);

  if (alarmSet) {
    tft.print("ALARM ");
    if (alarmHour < 10) tft.print("0");
    tft.print(alarmHour);
    tft.print(":");
    if (alarmMinute < 10) tft.print("0");
    tft.print(alarmMinute);
  } else {
    tft.print("ALARM OFF");
  }
}

void showAlarm() {
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);

  tft.setCursor(92, 3);
  tft.print("WAKE UP");

  tft.setTextSize(1);
  tft.setCursor(88, 23);
  tft.print("PRESS IN ORDER");

  tft.setTextSize(2);

  for (int i = 0; i < 4; i++) {
    tft.setCursor(82 + i * 32, 42);

    if (i < position)
      tft.setTextColor(ST77XX_GREEN);
    else
      tft.setTextColor(ST77XX_WHITE);

    tft.print(sequence[i]);
  }
}
