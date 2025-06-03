#include <OneWire.h>
#include <DallasTemperature.h>

// -------------------- PIN SETUP --------------------
#define ONE_WIRE_BUS 3         // DS18B20 (Suhu)
const int buzzerPin = 8;       // Buzzer untuk suhu

const int waterLevelPin = 2;   // Sensor water level
const int relayWaterPin = 4;   // Relay air (D4)

const int tdsPin = A1;         // Sensor TDS
const int relayTdsPin = 5;     // Relay TDS (D5)

const int phPin = A0;          // Sensor pH
const int relayBasePin = 6;    // Relay cairan basa (D6)
const int relayAcidPin = 7;    // Relay cairan asam (D7)

// -------------------- THRESHOLD SETUP --------------------
// Bisa diubah sesuai kebutuhan
float suhuMin = 24.0;
float suhuMax = 30.0;

int tdsThreshold = 300;

float phLow = 5.5;
float phHigh = 6.5;

// -------------------- SENSOR SETUP --------------------
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setup() {
  Serial.begin(9600);
  sensors.begin();

  // Setup pin
  pinMode(buzzerPin, OUTPUT);
  pinMode(waterLevelPin, INPUT);
  pinMode(relayWaterPin, OUTPUT);
  pinMode(tdsPin, INPUT);
  pinMode(relayTdsPin, OUTPUT);
  pinMode(phPin, INPUT);
  pinMode(relayBasePin, OUTPUT);
  pinMode(relayAcidPin, OUTPUT);

  // Matikan semua output awal
  digitalWrite(buzzerPin, LOW);
  digitalWrite(relayWaterPin, LOW);
  digitalWrite(relayTdsPin, LOW);
  digitalWrite(relayBasePin, LOW);
  digitalWrite(relayAcidPin, LOW);

  Serial.println("Semua sensor siap.");
}

void loop() {
  // -------------------- SENSOR SUHU --------------------
  sensors.requestTemperatures();
  float temperatureC = sensors.getTempCByIndex(0);
  Serial.print("Suhu: ");
  Serial.print(temperatureC);
  Serial.println(" °C");

  if (temperatureC < suhuMin || temperatureC > suhuMax) {
    digitalWrite(buzzerPin, HIGH);
  } else {
    digitalWrite(buzzerPin, LOW);
  }

  // -------------------- SENSOR WATER LEVEL --------------------
  int waterStatus = digitalRead(waterLevelPin);
  if (waterStatus == HIGH) {
    Serial.println("Water level: Terdeteksi");
    digitalWrite(relayWaterPin, LOW); // Relay OFF (karena air ada)
  } else {
    Serial.println("Water level: Tidak ada air");
    digitalWrite(relayWaterPin, HIGH); // Relay ON (karena air habis)
  }

  // -------------------- SENSOR TDS --------------------
  int rawTds = analogRead(tdsPin);
  float tdsValue = (rawTds * 500.0) / 1023.0;
  Serial.print("TDS: ");
  Serial.print(tdsValue, 1);
  Serial.println(" ppm");

  if (tdsValue < tdsThreshold) {
    digitalWrite(relayTdsPin, HIGH);  // ON
  } else {
    digitalWrite(relayTdsPin, LOW);   // OFF
  }

  // -------------------- SENSOR pH --------------------
  int rawPh = analogRead(phPin);
  float voltagePh = rawPh * (5.0 / 1023.0);
  float phValue = 7.0 + ((2.5 - voltagePh) / 0.18);

  Serial.print("pH: ");
  Serial.println(phValue, 2);

  if (phValue < phLow) {
    digitalWrite(relayBasePin, HIGH);
    digitalWrite(relayAcidPin, LOW);
  } else if (phValue > phHigh) {
    digitalWrite(relayBasePin, LOW);
    digitalWrite(relayAcidPin, HIGH);
  } else {
    digitalWrite(relayBasePin, LOW);
    digitalWrite(relayAcidPin, LOW);
  }

  Serial.println("-------------------------------------------------");
  delay(1000);
}
