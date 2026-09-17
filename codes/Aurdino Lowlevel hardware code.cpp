#include <WiFi.h>
#include <WebServer.h>
#include <esp_now.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_PN532.h>
#include <ArduinoJson.h>
#define SDA_PIN 8
#define SCL_PIN 9
#define IN1 11
#define IN2 12
#define ENA 10
#define IN3 13
#define IN4 14
#define ENB 46
#define GAS_SENSOR_PIN 1
#define SOIL_SENSOR_PIN 2
#define PN532_RESET 5
#define SERVOMIN  150 
#define SERVOMAX  600 
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);
LiquidCrystal_I2C lcd(0x27, 16, 2); // Change address to 0x3F if your PCF8574AT chip requires it
Adafruit_PN532 nfc(-1, PN532_RESET); // Soft IRQ (-1), HW Reset (GPIO 5)
WebServer server(80);
String lastScannedUID = "None";
String aiResultLabel = "Standby";
float aiConfidence = 0.0;
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
typedef struct struct_message {
  float gasValue;
  float soilValue;
  int currentPos[7];
} struct_message;
struct_message telemetryData;
float getFilteredSensorRead(int pin, float minValid, float maxValid, float baselineNormal) {
  int raw = analogRead(pin);
  float voltage = (raw / 4095.0) * 3.3;
  
  if (voltage < minValid || voltage > maxValid) {
    return baselineNormal; // Output 100% normal synthetic value on floating/garbage reads
  }
  return voltage;
}
void setServoAngle(uint8_t channel, int angle) {
  angle = constrain(angle, 0, 180);
  int pulse = map(angle, 0, 180, SERVOMIN, SERVOMAX);
  pwm.setPWM(channel, 0, pulse);
  if (channel < 6) {
    telemetryData.currentPos[channel] = angle;
  } else if (channel == 10) {
    telemetryData.currentPos[6] = angle;
  }
}
void moveRover(int leftSpeed, int rightSpeed) {
  if (leftSpeed >= 0) {
    digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  } else {
    digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
    leftSpeed = -leftSpeed;
  }
  if (rightSpeed >= 0) {
    digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  } else {
    digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
    rightSpeed = -rightSpeed;
  }
  analogWrite(ENA, constrain(leftSpeed, 0, 255));
  analogWrite(ENB, constrain(rightSpeed, 0, 255));
}
void updateLCDScreen(float gasVal, float soilVal) {
  lcd.setCursor(0, 0);
  if (lastScannedUID != "None") {
    lcd.print("Tag:" + lastScannedUID.substring(0, 8) + "    ");
  } else {
    lcd.print("AI:" + aiResultLabel.substring(0, 12) + " ");
  }
  lcd.setCursor(0, 1);
  lcd.print("G:");
  lcd.print(gasVal, 1);
  lcd.print("V S:");
  lcd.print(soilVal, 1);
  lcd.print("V  ");
}
void initESPNow() {
  WiFi.mode(WIFI_AP_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("[!] ESP-NOW Init Failed. Continuing without background broadcast...");
    return;
  }
  
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("[!] ESP-NOW Peer Add Failed.");
  } else {
    Serial.println("[+] ESP-NOW Ready.");
  }
}
const char HTML_INDEX[] PROGMEM = R"rawliteral(
<!DOCTYPE html><html>
<head><title>AgriBot ESP32-S3 Engine</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<style>
  body { font-family: Arial; text-align: center; background: #121212; color: white; margin: 0; padding: 15px; }
  .slider { width: 80%; max-width: 400px; margin: 8px; }
  button { padding: 12px 24px; font-size: 15px; margin: 5px; background: #008CBA; color: white; border: none; border-radius: 4px; }
</style>
</head>
<body>
  <h2> AgriBot Control Center</h2>
  <h3>6-DOF Arm + Soil Arm (Ch 10)</h3>
  <div>J0: <input type="range" min="0" max="180" class="slider" onchange="sendServo(0, this.value)"></div>
  <div>J1: <input type="range" min="0" max="180" class="slider" onchange="sendServo(1, this.value)"></div>
  <div>J2: <input type="range" min="0" max="180" class="slider" onchange="sendServo(2, this.value)"></div>
  <div>J3: <input type="range" min="0" max="180" class="slider" onchange="sendServo(3, this.value)"></div>
  <div>J4: <input type="range" min="0" max="180" class="slider" onchange="sendServo(4, this.value)"></div>
  <div>J5: <input type="range" min="0" max="180" class="slider" onchange="sendServo(5, this.value)"></div>
  <div>Soil (10): <input type="range" min="0" max="180" class="slider" onchange="sendServo(10, this.value)"></div>

  <h3>Chassis Drive</h3>
  <div><button onclick="fetch('/drive?l=220&r=220')">FORWARD</button></div>
  <div>
    <button onclick="fetch('/drive?l=-220&r=220')">LEFT</button>
    <button onclick="fetch('/drive?l=0&r=0')">STOP</button>
    <button onclick="fetch('/drive?l=220&r=-220')">RIGHT</button>
  </div>
  <div><button onclick="fetch('/drive?l=-220&r=-220')">BACKWARD</button></div>

<script>
function sendServo(ch, val) { fetch(`/servo?ch=${ch}&val=${val}`); }
</script>
</body>
</html>
)rawliteral";

void handleRoot() { server.send(200, "text/html", HTML_INDEX); }

void handleServo() {
  if (server.hasArg("ch") && server.hasArg("val")) {
    setServoAngle(server.arg("ch").toInt(), server.arg("val").toInt());
    server.send(200, "text/plain", "OK");
  }
}
void handleDrive() {
  if (server.hasArg("l") && server.hasArg("r")) {
    moveRover(server.arg("l").toInt(), server.arg("r").toInt());
    server.send(200, "text/plain", "OK");
  }
}
void handleApiInference() {
  if (!server.hasArg("plain")) {
    server.send(400, "text/plain", "Body missing");
    return;
  }
  StaticJsonDocument<250> doc;
  DeserializationError error = deserializeJson(doc, server.arg("plain"));
  if (!error) {
    aiResultLabel = doc["disease"].as<String>();
    aiConfidence = doc["confidence"].as<float>();
    Serial.printf("[Wi-Fi AI] Tag: %s (%.1f%%)\n", aiResultLabel.c_str(), aiConfidence);
    server.send(200, "application/json", "{\"status\":\"success\"}");
  } else {
    server.send(400, "text/plain", "Invalid JSON");
  }
}
void setup() {
  Serial.begin(115200);
  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT); pinMode(ENA, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT); pinMode(ENB, OUTPUT);
  moveRover(0, 0);
  pinMode(PN532_RESET, OUTPUT);
  digitalWrite(PN532_RESET, LOW);
  delay(100);
  digitalWrite(PN532_RESET, HIGH);
  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(100000); 
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("AgriBot S3 Boot");
  lcd.setCursor(0, 1);
  lcd.print("Init Hardware..");
  pwm.begin();
  pwm.setPWMFreq(50);
  for (int i = 0; i < 6; i++) setServoAngle(i, 90);
  setServoAngle(10, 0); // Stow soil arm
  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (versiondata) {
    nfc.SAMConfig();
    nfc.setPassiveActivationRetries(0x01);
    Serial.println("[+] PN532 Active.");
  } else {
    Serial.println("[-] PN532 missing/bypassed.");
  }
  initESPNow();
  WiFi.softAP("AgriBot-AP", "12345678");

  server.on("/", handleRoot);
  server.on("/servo", handleServo);
  server.on("/drive", handleDrive);
  server.on("/api/inference", HTTP_POST, handleApiInference);
  server.begin();

  delay(1000);
  lcd.clear();
}
void loop() {
  server.handleClient();
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, input);
    if (!error) {
      aiResultLabel = doc["disease"].as<String>();
      aiConfidence = doc["confidence"].as<float>();
      Serial.printf("[USB AI] Class: %s (%.1f%%)\n", aiResultLabel.c_str(), aiConfidence);
    }
  }
  uint8_t uid[7];
  uint8_t uidLength;
  if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 30)) {
    String cardHex = "";
    for (uint8_t i = 0; i < uidLength; i++) {
      if (uid[i] < 0x10) cardHex += "0";
      cardHex += String(uid[i], HEX);
    }
    cardHex.toUpperCase();
    lastScannedUID = cardHex;
    Serial.println("[+] RFID Scanned: " + lastScannedUID);
  }
  telemetryData.gasValue = getFilteredSensorRead(GAS_SENSOR_PIN, 0.1, 3.0, 0.40);
  telemetryData.soilValue = getFilteredSensorRead(SOIL_SENSOR_PIN, 0.2, 3.1, 1.50);
  static unsigned long lastLCDUpdate = 0;
  if (millis() - lastLCDUpdate > 250) {
    lastLCDUpdate = millis();
    updateLCDScreen(telemetryData.gasValue, telemetryData.soilValue);
  }
  static unsigned long lastBroadcast = 0;
  if (millis() - lastBroadcast > 500) {
    lastBroadcast = millis();
    esp_now_send(broadcastAddress, (uint8_t *) &telemetryData, sizeof(telemetryData));
  }
}
