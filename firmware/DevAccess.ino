/*
  DevAccess - ESP32 Access System
  RFID + PIR + Ultrasonic + OLED

  Final firmware file only (single .ino in firmware folder).
  Replace placeholder logic with your production rules if needed.
*/

#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ===== Pin Map =====
static const uint8_t PIN_RFID_SS   = 5;
static const uint8_t PIN_RFID_RST  = 27;
static const uint8_t PIN_PIR       = 13;
static const uint8_t PIN_US_TRIG   = 25;
static const uint8_t PIN_US_ECHO   = 26;
static const uint8_t PIN_RELAY     = 32;
static const uint8_t PIN_LED       = 2;

// ===== OLED =====
static const uint8_t OLED_ADDR = 0x3C;
static const uint8_t OLED_W = 128;
static const uint8_t OLED_H = 64;
Adafruit_SSD1306 display(OLED_W, OLED_H, &Wire, -1);

// ===== RFID =====
MFRC522 rfid(PIN_RFID_SS, PIN_RFID_RST);

// ===== Session Settings =====
static const uint32_t SESSION_TIMEOUT_MS = 10UL * 60UL * 1000UL;  // 10 minutes
static const uint32_t PIR_IDLE_TIMEOUT_MS = 30UL * 1000UL;        // 30 seconds
static const uint16_t DISTANCE_CM_MAX = 60;                       // proximity threshold
static const uint32_t DIST_GRACE_MS = 3000;                       // grace before shutdown

// ===== State =====
enum State {
  STATE_IDLE,
  STATE_AUTH,
  STATE_PRESENCE,
  STATE_PROXIMITY,
  STATE_ACTIVE
};

State state = STATE_IDLE;
uint32_t sessionStartMs = 0;
uint32_t lastMotionMs = 0;
uint32_t lastDistanceOkMs = 0;

// ===== Helpers =====
void showStatus(const String &line1, const String &line2 = "") {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(line1);
  if (line2.length()) {
    display.println(line2);
  }
  display.display();
}

void setDevice(bool on) {
  digitalWrite(PIN_RELAY, on ? HIGH : LOW);
  digitalWrite(PIN_LED, on ? HIGH : LOW);
}

bool readPIR() {
  return digitalRead(PIN_PIR) == HIGH;
}

uint16_t readDistanceCm() {
  digitalWrite(PIN_US_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_US_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_US_TRIG, LOW);

  uint32_t duration = pulseIn(PIN_US_ECHO, HIGH, 30000); // 30ms timeout
  if (duration == 0) return 999;
  return (uint16_t)(duration / 58); // approx cm
}

bool isAuthorizedUID(const MFRC522::Uid &uid) {
  // TODO: Replace with your authorized UID list
  // Example: allow any card for demo
  (void)uid;
  return true;
}

bool checkRFID() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return false;
  }
  bool ok = isAuthorizedUID(rfid.uid);
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
  return ok;
}

void setup() {
  pinMode(PIN_PIR, INPUT);
  pinMode(PIN_US_TRIG, OUTPUT);
  pinMode(PIN_US_ECHO, INPUT);
  pinMode(PIN_RELAY, OUTPUT);
  pinMode(PIN_LED, OUTPUT);
  setDevice(false);

  Serial.begin(115200);
  SPI.begin();
  rfid.PCD_Init();

  Wire.begin();
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("OLED init failed");
  }

  showStatus("DevAccess", "Ready");
}

void loop() {
  uint32_t now = millis();

  switch (state) {
    case STATE_IDLE:
      setDevice(false);
      showStatus("Present Card");
      if (checkRFID()) {
        state = STATE_PRESENCE;
        showStatus("Authorized", "Check motion");
      }
      break;

    case STATE_PRESENCE:
      if (readPIR()) {
        lastMotionMs = now;
        state = STATE_PROXIMITY;
        showStatus("Motion OK", "Check distance");
      } else if (now - sessionStartMs > 5000) {
        state = STATE_IDLE;
      }
      break;

    case STATE_PROXIMITY: {
      uint16_t dist = readDistanceCm();
      if (dist <= DISTANCE_CM_MAX) {
        state = STATE_ACTIVE;
        sessionStartMs = now;
        lastMotionMs = now;
        lastDistanceOkMs = now;
        setDevice(true);
        showStatus("Session", "Active");
      } else {
        showStatus("Move Closer", String(dist) + " cm");
        delay(250);
      }
      break;
    }

    case STATE_ACTIVE: {
      if (readPIR()) {
        lastMotionMs = now;
      }

      uint16_t dist = readDistanceCm();
      if (dist <= DISTANCE_CM_MAX) {
        lastDistanceOkMs = now;
      }

      bool sessionTimeout = (now - sessionStartMs) > SESSION_TIMEOUT_MS;
      bool pirTimeout = (now - lastMotionMs) > PIR_IDLE_TIMEOUT_MS;
      bool distTimeout = (now - lastDistanceOkMs) > DIST_GRACE_MS;

      if (sessionTimeout || pirTimeout || distTimeout) {
        setDevice(false);
        showStatus("Session End", "Shutting down");
        delay(1000);
        state = STATE_IDLE;
      } else {
        showStatus("Session Active", "Running");
      }
      break;
    }

    default:
      state = STATE_IDLE;
      break;
  }
}
