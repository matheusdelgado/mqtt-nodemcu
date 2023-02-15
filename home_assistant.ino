#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char *ssid =          "SSID";
const char *password =      "PASSWORD";

const char *mqtt_broker =   "HOST";
const char *mqtt_topic =    "TOPIC";
const char *mqtt_username = "USERNAME";
const char *mqtt_password = "PASSWORD";
const int mqtt_port =       PORT;

long randNumber;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);

  while(WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi...");

  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);

  while(!client.connected()) {
    String client_id = "home-assistant-temperature-";
    client_id += String(WiFi.macAddress());
    Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
    if(client.connect(client_id.c_str(), mqtt_username, mqtt_password)){
      Serial.println("Public emqx mqtt broker connected");
    } else {
      Serial.print("failed with state");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message: ");
  for (int i=0; i < length; i++) {
    Serial.print((char) payload[i]);
  }
  Serial.println();
  Serial.println("-------------------");
}

void loop() {
  randTemperature = random(0, 50);
  randHumidity = random(0, 100);
  randWind = random(0, 300);

  StaticJsonBuffer<300> JSONbuffer;
  JsonObject& JSONencoder = JSONbuffer.createObject();

  JSONencoder["temperature"] = String(randTemperature);
  JSONencoder["humidity"] = String(randHumidity);
  JSONencoder["wind"] = String(randWind);

  char JSONmessageBuffer[100];
  JSONencoder.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));

  client.publish(mqtt_topic, JSONmessageBuffer);
  client.subscribe(mqtt_topic);
  delay(2000);

  client.loop();
}