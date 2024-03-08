#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// PINS
// relay pins
const short R1_15W = 12;
const short R2_25W = 14;
const short R3_25W = 27;
// sensors pins
const short DHT22_1_PIN = 19;
const short DHT22_2_PIN = 18;

// Sensors
DHT temperatureSenserIn(DHT22_1_PIN, DHT22);  // (Pin, type)
DHT temperatureSensorOut(DHT22_2_PIN, DHT22);

/* Actionators */
LiquidCrystal_I2C lcd(0x27, 20, 4);  // Display LCD

// variables

float temperatureIn;   // temperature into incubator
float temperatureOut;  //temperature out incubator
float humidityIntoIncubator;

// ligthLevel is for incunator don't get higth and low temperature during all of the day
// the changeLightLevel level changes when the clime outdoor chenges
short lightLevel = 1;
short minLevelLight = 0;
short maxLevelLight = 3;


// functions
short getMinLightLevel();
short getMaxLightLevel();

void check_DHT_Sensor();
void changeLightLevel();
void printData();
void refreshData();
void regulationTemperature();

// Run
void setup() {

  // enable pin relay
  pinMode(R1_15W, OUTPUT);
  pinMode(R2_25W, OUTPUT);
  pinMode(R3_25W, OUTPUT);
  Serial.begin(115200);


  // init componets
  temperatureSenserIn.begin();
  temperatureSensorOut.begin();

  lcd.init();
  lcd.backlight();
  lcd.print("        HOLA");
  delay(2000);
  lcd.clear();
}


void loop() {
  refreshData();

  // when the temperature sensor falided into incubartor
  // the light level change to low temperature
  if (isnan(temperatureIn)) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Fallo el Sensor");

    changeLightLevel(0);
    return;
  }

  // when the temperature sensor out incubator
  // the range of light level will not change
  if (isnan(temperatureOut)) {
    minLevelLight = 0;
    maxLevelLight = 3;
  } else {
    minLevelLight = getMinLightLevel();
    maxLevelLight = getMaxLightLevel();
  }

  printData();
  regulationTemperature();
  delay(5000);
}



void refreshData() {
  humidityIntoIncubator = temperatureSenserIn.readHumidity();
  temperatureIn = temperatureSenserIn.readTemperature();
  temperatureOut = temperatureSensorOut.readTemperature();
}

void regulationTemperature() {
  if (temperatureIn < 37.3) {
    if (lightLevel < maxLevelLight) {
      short newLevel = lightLevel + 1;
      changeLightLevel(newLevel);
      printData();
      delay(30000);
      refreshData();
      printData();
    }

  } else if (temperatureIn > 37.3) {
    if (lightLevel > minLevelLight) {
      short newLevel = lightLevel - 1;
      changeLightLevel(newLevel);
      printData();
      delay(20000);
      refreshData();
      printData();
    }
  }
}


void printData() {
  lcd.clear();

  // temperatureIn
  lcd.setCursor(0, 0);
  lcd.print("Temp-in: ");
  lcd.print(temperatureIn);
  lcd.print("C");
  lcd.print(" ");
  lcd.print(humidityIntoIncubator);

  // temperatureOut
  lcd.setCursor(0, 1);
  lcd.print("Temp-ext: ");
  if (isnan(temperatureOut)) {
    lcd.print("fallo :(");
  } else {
    lcd.print(temperatureOut);
    lcd.print("C");
  }

  // humidityIntoIncubator
  lcd.setCursor(0, 2);
  lcd.print("Humedad:  ");
  if (isnan(humidityIntoIncubator)) {
    lcd.print("fallo :(");
  } else {
    lcd.print(humidityIntoIncubator);
    lcd.print("%");
  }

  // level Light
  lcd.setCursor(0, 3);
  lcd.print("Luz: ");
  lcd.print(lightLevel);

  lcd.print("  min:");
  lcd.print(minLevelLight);

  lcd.print("  max:");
  lcd.print(maxLevelLight);
}

// define the minimo range of light level
short getMinLightLevel() {

  return 0;

  // it's no necesary in a good box of incubator
  
  // if (temperatureOut < 25.5) {
  //   return 3;

  // } else if (temperatureOut <= 28.8) {
  //   return 2;

  // } else if (temperatureOut <= 32) {
  //   return 1;

  // } else {
  //   return 0;
  // }
}


// define the max range of light level
short getMaxLightLevel() {

  if (temperatureOut > 33.5) {
    return 2;

  } else if (temperatureOut > 28.4) {
    return 3;

  } else if (temperatureOut >= 27) {
    return 4;

  } else {
    return 5;
  }
}

// switches the lights
void changeLightLevel(short level) {
  Serial.print("Level: ");
  Serial.print(level);
  lightLevel = level;

  if (lightLevel == 0) {
    digitalWrite(R1_15W, LOW);
    delay(200);
    digitalWrite(R2_25W, LOW);
    delay(200);
    digitalWrite(R3_25W, LOW);

  } else if (lightLevel == 1) {
    digitalWrite(R3_25W, LOW);
    delay(200);
    digitalWrite(R1_15W, HIGH);
    delay(200);
    digitalWrite(R2_25W, LOW);

  } else if (lightLevel == 2) {
    digitalWrite(R3_25W, LOW);
    delay(200);
    digitalWrite(R1_15W, LOW);
    delay(200);
    digitalWrite(R2_25W, HIGH);

  } else if (lightLevel == 3) {
    digitalWrite(R3_25W, LOW);
    delay(200);
    digitalWrite(R1_15W, HIGH);
    delay(200);
    digitalWrite(R2_25W, HIGH);

  } else if (lightLevel == 4) {
    digitalWrite(R3_25W, HIGH);
    delay(200);
    digitalWrite(R1_15W, LOW);
    delay(200);
    digitalWrite(R2_25W, HIGH);

  } else if (lightLevel == 5) {
    digitalWrite(R3_25W, HIGH);
    delay(200);
    digitalWrite(R1_15W, HIGH);
    delay(200);
    digitalWrite(R2_25W, HIGH);
  }
}
