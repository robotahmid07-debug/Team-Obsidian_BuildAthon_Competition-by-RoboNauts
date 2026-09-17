#include <WiFi.h>
#include <WebServer.h>
#include <esp_now.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
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
#define SERVOMIN  150 // Minimum pulse length out of 4096
#define SERVOMAX  600 // Maximum pulse length out of 4096
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);
WebServer server(80);
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
    return baselineNormal; 
  }
  return voltage;
}
void setServoAngle(uint8_t channel, int angle) {
  angle = constrain(angle, 0, 180);
  int pulse = map(angle, 0, 180, SERVOMIN, SERVOMAX);
  pwm.setPWM(channel, 0, pulse);
  if (channel < 7) {
    telemetryData.currentPos[channel] = angle;
  }
}
void moveRover(int leftSpeed, int rightSpeed) {
  // Left side direction
  if (leftSpeed >= 0) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
  } else {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    leftSpeed = -leftSpeed;
  }
  if (rightSpeed >= 0) {
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  } else {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    rightSpeed = -rightSpeed;
  }

  analogWrite(ENA, constrain(leftSpeed, 0, 255));
  analogWrite(ENB, constrain(rightSpeed, 0, 255));
}
void initESPNow() {
  WiFi.mode(WIFI_AP_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("[!] ESP-NOW Init Failed. Skipping and continuing background operations...");
    return;
  }
  
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("[!] ESP-NOW Peer Registration Failed. Continuing without peer.");
  } else {
    Serial.println("[+] ESP-NOW Active.");
  }
}

// --- WEB SERVER CONTROL DASHBOARD ---
const char HTML_INDEX[] PROGMEM = R"rawliteral(
<!DOCTYPE html><html>
<head><title>ESP32-S3 AgriBot Dashboard</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<style>
  body { font-family: Arial; text-align: center; background: #121212; color: white; padding: 10px; }
  .slider { width: 80%; max-width: 400px; margin: 10px; }
  button { padding: 10px 20px; font-size: 16px; margin: 5px; background: #008CBA; color: white; border: none; border-radius: 5px; }
</style>
</head>
<body>
  <h2> ESP32-S3 Manual & Sensor Control</h2>
  
  <h3>6-DOF Arm + Soil Servo (Channel 10)</h3>
  <div>Joint 0: <input type="range" min="0" max="180" class="slider" onchange="sendServo(0, this.value)"></div>
  <div>Joint 1: <input type="range" min="0" max="180" class="slider" onchange="sendServo(1, this.value)"></div>
  <div>Joint 2: <input type="range" min="0" max="180" class="slider" onchange="sendServo(2, this.value)"></div>
  <div>Joint 3: <input type="range" min="0" max="180" class="slider" onchange="sendServo(3, this.value)"></div>
  <div>Joint 4: <input type="range" min="0" max="180" class="slider" onchange="sendServo(4, this.value)"></div>
  <div>Joint 5: <input type="range" min="0" max="180" class="slider" onchange="sendServo(5, this.value)"></div>
  <div>Soil Arm (Ch 10): <input type="range" min="0" max="180" class="slider" onchange="sendServo(10, this.value)"></div>

  <h3>Arm Presets</h3>
  <button onclick="fetch('/preset?id=home')">Home</button>
  <button onclick="fetch('/preset?id=soil_sample')">Soil Sample</button>

  <h3>6WD Control</h3>
  <div><button onclick="fetch('/drive?l=200&r=200')">FORWARD</button></div>
  <div>
    <button onclick="fetch('/drive?l=-200&r=200')">LEFT</button>
    <button onclick="fetch('/drive?l=0&r=0')">STOP</button>
    <button onclick="fetch('/drive?l=200&r=-200')">RIGHT</button>
  </div>
  <div><button onclick="fetch('/drive?l=-200&r=-200')">BACKWARD</button></div>

<script>
function sendServo(ch, val) {
  fetch(`/servo?ch=${ch}&val=${val}`);
}
</script>
</body>
</html>
)rawliteral";
void handleRoot() { server.send(200, "text/html", HTML_INDEX); }
void handleServo() {
  if (server.hasArg("ch") && server.hasArg("val")) {
    int ch = server.arg("ch").toInt();
    int val = server.arg("val").toInt();
    setServoAngle(ch, val);
    server.send(200, "text/plain", "OK");
  }
}
void handleDrive() {
  if (server.hasArg("l") && server.hasArg("r")) {
    int l = server.arg("l").toInt();
    int r = server.arg("r").toInt();
    moveRover(l, r);
    server.send(200, "text/plain", "OK");
  }
}
void handlePresets() {
  if (server.hasArg("id")) {
    String id = server.arg("id");
    if (id == "home") {
      for(int i=0; i<6; i++) setServoAngle(i, 90);
      setServoAngle(10, 0); // Stow soil arm
    } else if (id == "soil_sample") {
      setServoAngle(0, 90); setServoAngle(1, 45); setServoAngle(2, 135);
      setServoAngle(10, 120); // Deploy soil arm
    }
    server.send(200, "text/plain", "Preset Executed");
  }
}
void handleApiInference() {
  if (server.hasArg("plain") == false) {
    server.send(400, "text/plain", "Body missing");
    return;
  }
  StaticJsonDocument<250> doc;
  DeserializationError error = deserializeJson(doc, server.arg("plain"));
  if (error) {
    server.send(400, "text/plain", "Invalid JSON");
    return;
  }
  
  const char* label = doc["label"];
  float confidence = doc["confidence"];
  Serial.printf("[Wi-Fi / Phone AI] Class: %s (%.2f)\n", label, confidence);

  server.send(200, "application/json", "{\"status\":\"accepted\"}");
}
void setup() {
  Serial.begin(115200);
  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT); pinMode(ENA, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT); pinMode(ENB, OUTPUT);
  moveRover(0, 0);
  Wire.begin(SDA_PIN, SCL_PIN);
  pwm.begin();
  pwm.setPWMFreq(50); // 50Hz for analog servos
  for(int i=0; i<6; i++) setServoAngle(i, 90);
  setServoAngle(10, 0);
  initESPNow();
  WiFi.softAP("ESP32S3-AgriBot", "12345678");
  Serial.print("[+] Access Point Started. IP: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/servo", handleServo);
  server.on("/drive", handleDrive);
  server.on("/preset", handlePresets);
  server.on("/api/inference", HTTP_POST, handleApiInference);
  server.begin();
}

void loop() {
  server.handleClient();
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, input);
    if (!error) {
      const char* label = doc["label"];
      float confidence = doc["confidence"];
      Serial.printf("[USB Serial AI] Class: %s (%.2f)\n", label, confidence);
    }
  }
  telemetryData.gasValue = getFilteredSensorRead(GAS_SENSOR_PIN, 0.1, 3.0, 0.40);
  telemetryData.soilValue = getFilteredSensorRead(SOIL_SENSOR_PIN, 0.2, 3.1, 1.50);
  static unsigned long lastBroadcast = 0;
  if (millis() - lastBroadcast > 500) { // Every 500ms
    lastBroadcast = millis();
    esp_now_send(broadcastAddress, (uint8_t *) &telemetryData, sizeof(telemetryData));
  }
}
