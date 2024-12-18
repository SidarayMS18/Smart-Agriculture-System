#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); 

#define DHTPIN 2    
#define DHTTYPE DHT11   
DHT dht(DHTPIN, DHTTYPE);

const int moisturePin = A0;  
const int lightPin = A1;    
const int relayPin = 8;    
const int dryThreshold = 800; 
const int wetThreshold = 400; 

bool motorState = false;

void setup() {
  Serial.begin(9600);  
  dht.begin();         

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Smart Agri System");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");

  pinMode(relayPin, OUTPUT);  
  digitalWrite(relayPin, HIGH); 
  delay(2000); 
}

void loop() {
  int moistureValue = analogRead(moisturePin);
  int lightValue = analogRead(lightPin);
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }


  if (moistureValue > dryThreshold && !motorState) {
    digitalWrite(relayPin, LOW); 
    motorState = true;
    Serial.println("Motor ON");
    updateLCDMotorState();
  } else if (moistureValue < wetThreshold && motorState) {
    digitalWrite(relayPin, HIGH);  
    motorState = false;
    Serial.println("Motor OFF");
    updateLCDMotorState();
  }

  lcd.setCursor(0, 0);
  lcd.print("M:");
  lcd.print(map(moistureValue, 1023, 0, 0, 100)); 
  lcd.print("% L:");
  int lightPercentage = map(lightValue, 0, 1023, 0, 100); 
  lcd.print(lightPercentage);
  lcd.print("%");
  
  lcd.setCursor(0, 1);
  lcd.print("T:"); lcd.print(temperature, 1);
  lcd.print("C H:"); lcd.print(humidity, 1);
  lcd.print("%");

  updateLCDMotorState();

  Serial.print("Moisture: "); Serial.print(moistureValue);
  Serial.print(" | Light: "); Serial.print(lightValue);
  Serial.print(" | Humidity: "); Serial.print(humidity);
  Serial.print(" | Temperature: "); Serial.println(temperature);

  delay(5000); 
}
void updateLCDMotorState() {
  lcd.setCursor(13, 1); // Position the cursor at the end of the second line
  lcd.print("M:");
  if (motorState) {
    lcd.print("ON ");
  } else {
    lcd.print("OFF");
  }
}


