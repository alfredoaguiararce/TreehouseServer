#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

const char* ssid = "Experimento";
const char* password = "LAYLA2020";

WiFiServer server(8080); // Cambiado el puerto a 80

// Analog pins for sensors
const int LM35_PIN = A0;           // LM35 Temperature Sensor (A0)
const int YL65_PIN = 04;           // YL-65 Humidity Sensor (D2)
const int LEVEL_PIN = 14;           // YL-65 Humidity Sensor (D5)
// const int PHOTO_RESISTOR_PIN = 04; // Photoresistor Light Sensor (A2)
// Digital pins for actuators
const int WATER_PUMP_PIN = 02;  // Water Pump (D4)
const int FAN_PIN = 16;         // Fan (D0)
const int LED1_PIN = 15;        // LED 1 (D8)
const int LED2_PIN = 00;        // LED 2 (D3)
const int BUZZER_PIN = 05;      // Buzzer (D1)

// Extracts the path from the HTTP request
String extractPath(String request) {
  int start = request.indexOf(' ') + 1;
  int end = request.indexOf(' ', start);
  return request.substring(start, end);
}

void handleNotFound(WiFiClient& client) {
  client.println("HTTP/1.1 404 Not Found");
  client.println("Content-type:text/html");
  client.println();
  client.println("<p>Route not found</p>");
}

void OnBuzzer() {
  digitalWrite(BUZZER_PIN, HIGH);
}

void OffBuzzer() {
  digitalWrite(BUZZER_PIN, LOW);
}

void WaterPumpOn() {
  digitalWrite(WATER_PUMP_PIN, HIGH);
}

void WaterPumpOff() {
  digitalWrite(WATER_PUMP_PIN, LOW);
}

void Led2On() {
  digitalWrite(LED2_PIN, HIGH);
}

void Led2Off() {
  digitalWrite(LED2_PIN, LOW);
}

void Led1On() {
  digitalWrite(LED1_PIN, HIGH);
}

void Led1Off() {
  digitalWrite(LED1_PIN, LOW);
}

void FanOn() {
  digitalWrite(FAN_PIN, HIGH);
}

void FanOff() {
  digitalWrite(FAN_PIN, LOW);
}

String readSensors() {
  int lm35Value = analogRead(LM35_PIN);
  // Convierte la lectura a temperatura en grados Celsius
  float temperaturaCelsius = (lm35Value * 3.3 * 100)/ 1023;
  int yl65Value = digitalRead(YL65_PIN);
  int LvlValue = digitalRead(LEVEL_PIN);
  bool IsFullLevel = false;
  if(LvlValue == 0){
    IsFullLevel = true;
  }
  bool IsHum = false;
  if(yl65Value == 0)
  {
    IsHum = true;
    }

  DynamicJsonDocument doc(200);
  doc["LM35"] = lm35Value;
  doc["YL65"] = yl65Value;
  doc["Temperature"] = temperaturaCelsius;
  doc["IsHumidity"] = IsHum;
  doc["IsFullOfLiquid"] = IsFullLevel;
  //doc["Light"] = photoResistorVoltage;

  String json;
  serializeJson(doc, json);
  return json;
}

void AutoValidations()
{
  int lm35Value = analogRead(LM35_PIN);
  // Convierte la lectura a temperatura en grados Celsius
  float temperaturaGrados = (lm35Value * 3.3 * 100)/ 1023;
  int yl65Value = digitalRead(YL65_PIN);
  int LvlValue = digitalRead(LEVEL_PIN);
  bool IsFullLevel = false;
  if(LvlValue == 0){
    IsFullLevel = true;
  }
  bool IsHum = false;
  if(yl65Value == 0)
  {
    IsHum = true;
    }
  //Si la temperatura rebasa los 50 grados se enciende ventiladores(D0 -16)
 if(temperaturaGrados > 50){
    Led1Off();
    FanOn();
    OffBuzzer();
 }
  //Si temperatura cae a menor de 10 grados , enciende luminaria 1 (D8- 15)
   if(temperaturaGrados < 10){
    Led1On();
    FanOff();
    OffBuzzer();
 }
  //La luminaria 2 se enciende directo desde la aplicación 
  
  //Si suelo está seco se enciende Bomba ( D4 -02)
  if(!IsHum){
    WaterPumpOn();
  }else{
    WaterPumpOff();
  }
  //Si temperatura arrebasa los 80 grados , se enciende buzzer (D1-05)
   if(temperaturaGrados > 80){
    Led1Off();
    FanOn();
    OnBuzzer();
 }
  //Si la temperatura es menor a 5 grados se enciende buzzer (D1-05)
   if(temperaturaGrados < 5){
    Led1On();
    FanOff();
    OnBuzzer();
 }
 
 Serial.println("Temperatura : ");
 Serial.println(temperaturaGrados);
}

void setup() {
  Serial.begin(115200);
  delay(10);

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Connected to WiFi");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Failed to connect to WiFi");
  }

  // Configure pins in setup
  pinMode(WATER_PUMP_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(YL65_PIN, INPUT);
  pinMode(LEVEL_PIN,INPUT);

  server.begin();
}

void loop() {
  WiFiClient client = server.available();
 
  if (client) {
    Serial.println("New client");

    while (client.connected()) {
      if (client.available()) {
        String request = client.readStringUntil('\r');
        Serial.println(request);

        if (request.indexOf("GET") != -1) {
          handleGetRequest(client, request);
          break;
        }
      }
    }

    delay(1);
  }

  //AutoValidations();
}

void handleGetRequest(WiFiClient& client, String request) {
  String getPath = extractPath(request);

  // Define your routes and associated functions
  if (getPath.equals("/data")) {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type: application/json");
    client.println();
    client.println(readSensors());
  } else if (getPath.equals("/buzzer/on")) {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type: application/json");
    client.println("Access-Control-Allow-Origin: *");  // Permitir solicitudes desde cualquier origen (*)
    client.println();
    OnBuzzer();
  } else if (getPath.equals("/buzzer/off")) {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type: application/json");
    client.println("Access-Control-Allow-Origin: *");  // Permitir solicitudes desde cualquier origen (*)
    client.println();
    OffBuzzer();
  } else if (getPath.equals("/waterpump/on")) {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type: application/json");
    client.println("Access-Control-Allow-Origin: *");  // Permitir solicitudes desde cualquier origen (*)
    client.println();
    WaterPumpOn();
  } else if (getPath.equals("/waterpump/off")) {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type: application/json");
    client.println("Access-Control-Allow-Origin: *");  // Permitir solicitudes desde cualquier origen (*)
    client.println();
    WaterPumpOff();
  } else if (getPath.equals("/led2/on")) {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type: application/json");
    client.println("Access-Control-Allow-Origin: *");  // Permitir solicitudes desde cualquier origen (*)
    client.println();
    Led2On();
  } else if (getPath.equals("/led2/off")) {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type: application/json");
    client.println("Access-Control-Allow-Origin: *");  // Permitir solicitudes desde cualquier origen (*)
    client.println();
    Led2Off();
  } else if (getPath.equals("/led1/on")) {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type: application/json");
    client.println("Access-Control-Allow-Origin: *");  // Permitir solicitudes desde cualquier origen (*)
    client.println();
    Led1On();
  } else if (getPath.equals("/led1/off")) {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type: application/json");
    client.println("Access-Control-Allow-Origin: *");  // Permitir solicitudes desde cualquier origen (*)
    client.println();
    Led1Off();
  } else if (getPath.equals("/fan/on")) {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type: application/json");
    client.println("Access-Control-Allow-Origin: *");  // Permitir solicitudes desde cualquier origen (*)
    client.println();
    FanOn();
  } else if (getPath.equals("/fan/off")) {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type: application/json");
    client.println("Access-Control-Allow-Origin: *");  // Permitir solicitudes desde cualquier origen (*)
    client.println();
    FanOff();
  } else {
    handleNotFound(client);
  }
}
