#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// Pines
const short R1_15W_PIN = 12;
const short R2_25W_PIN = 14;
const short DHT22_1_PIN = 19;

// Sensores
DHT temperatureSensorIn(DHT22_1_PIN, DHT22);

// Display LCD
LiquidCrystal_I2C lcd(0x27, 20, 4);

// Variables
float temperatureIn;
float humidityIntoIncubator;
short lightLevel = 0; // Nivel mínimo al inicio
const float INCUBATOR_TEMPERATURE = 37.8;
const short MIN_LIGHT_LEVEL = 0; // Nivel mínimo permitido
const short MAX_LIGHT_LEVEL = 3; // Nivel máximo permitido

//functions
void refreshData();
void handleSensorFailure();
void adjustLightLevel();
void printData();
void changeLightLevel();

// Funciones
void setup() {
  pinMode(R1_15W_PIN, OUTPUT);
  pinMode(R2_25W_PIN, OUTPUT);
  Serial.begin(115200);

  temperatureSensorIn.begin();

  lcd.init();
  lcd.backlight();
  lcd.print("        HOLA");
  delay(2000);
  lcd.clear();
}

void loop() {
  refreshData();

  if (isnan(temperatureIn)) {
    handleSensorFailure();
    return;
  }

  adjustLightLevel();

  printData();
  delay(5000);
}

void refreshData() {
  humidityIntoIncubator = temperatureSensorIn.readHumidity();
  temperatureIn = temperatureSensorIn.readTemperature();
}

void handleSensorFailure() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Fallo el Sensor");
  changeLightLevel(0); // Apagar relés en caso de fallo del sensor
}

void adjustLightLevel() {
  if (temperatureIn < INCUBATOR_TEMPERATURE && lightLevel < MAX_LIGHT_LEVEL) {
    short level = lightLevel + 1;
    changeLightLevel(level); // Aumentar nivel de luz
    printData();
    delay(30000); // Esperar 30 segundos
  } else if (temperatureIn > INCUBATOR_TEMPERATURE && lightLevel > MIN_LIGHT_LEVEL) {
    short level = lightLevel - 1;
    changeLightLevel(level); // Disminuir nivel de luz
    printData();
    delay(20000); // Esperar 20 segundos
  }
}

void printData() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp-in: ");
  lcd.print(temperatureIn);
  lcd.print("C");

  lcd.setCursor(0, 1);
  lcd.print("Humedad:  ");
  if (isnan(humidityIntoIncubator)) {
    lcd.print("fallo :(");
  } else {
    lcd.print(humidityIntoIncubator);
    lcd.print("%");
  }

  lcd.setCursor(0, 3);
  lcd.print("Luz: ");
  lcd.print(lightLevel);
  lcd.print("  min:");
  lcd.print(MIN_LIGHT_LEVEL);
  lcd.print("  max:");
  lcd.print(MAX_LIGHT_LEVEL);
}

void changeLightLevel(short level) {
  Serial.print("Level: ");
  Serial.print(level);
  lightLevel = level;

  // Controlar los dos relés para los tres niveles de luz
  digitalWrite(R1_15W_PIN, level == 1 || level == 3);
  digitalWrite(R2_25W_PIN, level >= 2);
}
