#include <WiFi.h>
#include <Wire.h>
#include <math.h>
#include "gestures.h"
#include "twilio_sms.h"
#include "gesture_webserver.h"

// WiFi Credentials
const char* ssid = "";
const char* password = "";

// Flex Sensor Pins
const int flexSensorPins[5] = {36, 34, 35, 32, 33};
const float supplyVoltage = 5.0;
const int pullUpResistor = 15000;
float minResistance[5];
float maxResistance[5];

// Twilio config
const char* account_sid = "YOUR_ACCOUNT_SID";
const char* auth_token = "YOUR_AUTH_TOKEN";
const char* from_number = "YOUR_TWILIO_NUMBER";
const char* to_number = "RECIPIENT_NUMBER";

// Touch Sensor Pins
const int finger1Pin = 26;
const int finger2Pin = 27;
const int finger3Pin = 14;
const int finger4Pin = 12;

// Emergency Button
const int emergencyButtonPin = 23;

struct SensorData {
  float angles[5];
  String bendStates[5];
  int touchStates[4];

  String toJSON() {
    String json = "{";
    json += "\"angles\": [";
    for (int i = 0; i < 5; i++) {
      json += String(angles[i]);
      if (i < 4) json += ", ";
    }
    json += "], \"bendStates\": [";
    for (int i = 0; i < 5; i++) {
      json += "\"" + bendStates[i] + "\"";
      if (i < 4) json += ", ";
    }
    json += "], \"touchStates\": [";
    for (int i = 0; i < 4; i++) {
      json += String(touchStates[i]);
      if (i < 3) json += ", ";
    }
    json += "]";
    json += "}";
    return json;
  }
};

SensorData lastData;
String lastGesture = "";
String lastSentWord = "";  // ✅ Tracks last word sent to web server
unsigned long previousMillis = 0;
const long interval = 100;

void setup() {
  Serial.begin(115200);
  Wire.begin();

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ WiFi Connected!");
  Serial.println(WiFi.localIP());
  startGestureWebServer();  // ✅ Start server (WiFi is already connected)

  Serial.println("Calibrating Flex Sensors...");
  calibrateFlexSensors();
  Serial.println("✅ Calibration Done!");

  pinMode(finger1Pin, INPUT_PULLUP);
  pinMode(finger2Pin, INPUT_PULLUP);
  pinMode(finger3Pin, INPUT_PULLUP);
  pinMode(finger4Pin, INPUT_PULLUP);
  pinMode(emergencyButtonPin, INPUT_PULLUP);  // ✅ Make sure it's set
}

void loop() {
  handleWebServerClient();  // ✅ Handle incoming HTTP requests

  if (digitalRead(emergencyButtonPin) == HIGH) {
    Serial.println("🚨 EMERGENCY BUTTON PRESSED!");
    sendEmergencySMS();
    
    delay(5000); // prevent spamming
  }

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    SensorData data = readSensors();

    if (!isSameState(data, lastData)) {
      String gesturePattern = "";
      for (int i = 0; i < 5; i++) {
        gesturePattern += data.bendStates[i];
        if (i < 4) gesturePattern += " ";
      }
      gesturePattern += " | ";
      for (int i = 0; i < 4; i++) {
        gesturePattern += String(data.touchStates[i]);
        if (i < 3) gesturePattern += " ";
      }

      String meaning = recognizeGesture(gesturePattern);

      if (gesturePattern != lastGesture) {
        Serial.println("Gesture Pattern: " + gesturePattern);
        Serial.println("Recognized Gesture: " + meaning);
        Serial.println();
        lastGesture = gesturePattern;
      }

      // ✅ Send to web server if it's a new and known word
      if (meaning != "UNKNOWN" && meaning != lastSentWord) {
        SendMessageToApp(meaning);
        Serial.println("📡 Sent to server: " + meaning);
        lastSentWord = meaning;
      }

      lastData = data;
    }
  }
}

void sendEmergencySMS() {
  String msg = "🚨 Emergency! The button was pressed on the ESP32 glove.";
  bool sent = sendSMS(msg);

  if (sent) {
    Serial.println("✅ Emergency SMS sent successfully.");
    SendMessageToApp("Emergency");
  } else {
    Serial.println("❌ Failed to send SMS.");
  }
}

void calibrateFlexSensors() {
  Serial.println("Hold fingers STRAIGHT and press any key...");
  while (Serial.available() == 0) {}
  Serial.read();
  calibrateResistance(minResistance);
  Serial.println("Straight position recorded.\n");

  Serial.println("Now BEND fingers fully and press any key...");
  while (Serial.available() == 0) {}
  Serial.read();
  calibrateResistance(maxResistance);
  Serial.println("Bent position recorded.\n");
}

void calibrateResistance(float resistanceArray[5]) {
  for (int i = 0; i < 5; i++) {
    float sum = 0;
    for (int j = 0; j < 5; j++) {
      sum += readFlexSensorResistance(flexSensorPins[i]);
      delay(50);
    }
    resistanceArray[i] = sum / 5;
  }
}

SensorData readSensors() {
  SensorData data;

  for (int i = 0; i < 5; i++) {
    float resistance = readFlexSensorResistance(flexSensorPins[i]);
    float angle = map(resistance, minResistance[i], maxResistance[i], 0, 180);
    angle = constrain(angle, 0, 180);
    data.angles[i] = angle;
    data.bendStates[i] = (angle < 45) ? "S" : "B";
  }

  data.touchStates[0] = digitalRead(finger1Pin) == LOW ? 1 : 0;
  data.touchStates[1] = digitalRead(finger2Pin) == LOW ? 1 : 0;
  data.touchStates[2] = digitalRead(finger3Pin) == LOW ? 1 : 0;
  data.touchStates[3] = digitalRead(finger4Pin) == LOW ? 1 : 0;

  return data;
}

float readFlexSensorResistance(int pin) {
  float total = 0;
  for (int i = 0; i < 10; i++) {
    total += analogRead(pin);
    delay(5);
  }
  float value = total / 10;
  if (value == 0) return 10000;
  float voltage = value * (supplyVoltage / 1023.0);
  return (pullUpResistor * (supplyVoltage - voltage)) / voltage;
}

bool isSameState(SensorData a, SensorData b) {
  for (int i = 0; i < 5; i++) {
    if (a.bendStates[i] != b.bendStates[i]) return false;
  }
  for (int i = 0; i < 4; i++) {
    if (a.touchStates[i] != b.touchStates[i]) return false;
  }
  return true;
}
