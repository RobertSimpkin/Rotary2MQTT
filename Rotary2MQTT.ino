#include <Button2.h>
#include <ESP8266WiFi.h>
#include <ESPRotary.h>
#include <PubSubClient.h>

#define BUILTIN_LED     2
#define ROTARY_PIN1     5
#define ROTARY_PIN2     4
#define BUTTON_PIN      14

#define ANTI_CLOCKWISE  255
#define CLOCKWISE       1
#define CLICK           8
#define LONGCLICK       9           

// Wifi Information
const char* ssid = "***WiFi SSID***";
const char* wifipass = "***WiFi Password***";

// MQTT Information
const char* mqttserver = "***MQTT Server IP***";
const int mqttport = 1883;
const char* mqttuser = "mqtthass";
const char* mqttpass = "mqttpass";
const char* mqttname = "rotary1";
const char* mqtttopic = "rotary/rotary1";

// Rotary Encoder
ESPRotary rotary = ESPRotary(ROTARY_PIN1, ROTARY_PIN2, 4);
Button2 button = Button2(BUTTON_PIN);

// Networking
WiFiClient wifiClient;
PubSubClient client(wifiClient);

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);
  Serial.begin(115200);
  delay(10);

  rotary.setLeftRotationHandler(showRotation);
  rotary.setRightRotationHandler(showRotation);
  button.setLongClickHandler(buttonClick);

  setupWiFi();
  setupMQTT();
}

void loop() {
  rotary.loop();
  button.loop();
}

void showRotation(ESPRotary& rotary) {
  byte currentDirection = rotary.getDirection();
  if (currentDirection == ANTI_CLOCKWISE) {
    publishMessage("L");
  }
  else if (currentDirection == CLOCKWISE) {
    publishMessage("R");
  }
}

void buttonClick(Button2& button) {
  publishMessage("C");
}

void publishMessage(char* message) {
  Serial.println("Attempting to connect to MQTT server");
  if (client.connect(mqttname, mqttuser, mqttpass)) {
    Serial.print("Publishing ");
    Serial.print(message);
    Serial.print(" to ");
    Serial.println(mqtttopic);
    client.publish(mqtttopic, message);
  } else {
    Serial.println("Error connecting to MQTT server. Ignoring...");
  }
}

void setupWiFi() {
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, wifipass);

  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(BUILTIN_LED, LOW);
    delay(500);
    digitalWrite(BUILTIN_LED, HIGH);
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("WiFi connected! IP Address: ");
  Serial.println(WiFi.localIP());
  digitalWrite(BUILTIN_LED, LOW);
  Serial.println();
}

void setupMQTT() {
  client.setServer(mqttserver, mqttport);
}
