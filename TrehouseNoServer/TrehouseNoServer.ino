// Analog pins for sensors
const int LM35_PIN = A1;           // LM35 Temperature Sensor (A0)
const int PHOTORESISTOR_PIN = A0;  // LM35 Temperature Sensor (A0)
const int YL65_PIN = 10;           // YL-65 Humidity Sensor (D2)
const int LEVEL_PIN = 9;           // YL-65 Humidity Sensor (D5)
// const int PHOTO_RESISTOR_PIN = 04; // Photoresistor Light Sensor (A2)
// Digital pins for actuators
const int WATER_PUMP_PIN = 4;  // Water Pump (D4)
const int FAN_PIN = 5;         // Fan (D0)
const int LED1_PIN = 6;        // LED 1 (D8)
const int LED2_PIN = 7;        // LED 2 (D3)
const int BUZZER_PIN = 8;      // Buzzer (D1)


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

void AutoValidations()
{
  int lm35Value = analogRead(LM35_PIN);
  int PhotoResistorVal = analogRead(PHOTORESISTOR_PIN);
  float photoResistorVoltage = PhotoResistorVal * (5.0 / 1023.0);
  // Convierte la lectura a temperatura en grados Celsius
  float temperaturaGrados = (lm35Value * 5.0 * 100)/ 1023;
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
 Serial.println("Foto resistencia : ");
 Serial.println(photoResistorVoltage);
}

void setup() {
  Serial.begin(115200);
  delay(10);

  // Configure pins in setup
  pinMode(WATER_PUMP_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(YL65_PIN, INPUT);
  pinMode(LEVEL_PIN,INPUT);
}

void loop() {
  AutoValidations();
  delay(100);
}
