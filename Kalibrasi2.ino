#include <SoftwareSerial.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#define STM32_TX PA2 // Pin TX STM32
#define STM32_RX PA3 // Pin RX STM32
#define RELAY_PIN PA9 // Misalnya, pin GPIO PA9 digunakan untuk mengendalikan relay
// Set alamat I2C untuk LCD
#define I2C_ADDR    0x27  // Alamat I2C untuk LCD biasanya adalah 0x27 atau 0x3F

// Inisialisasi objek LCD
LiquidCrystal_I2C lcd(I2C_ADDR, 20, 4);  // Alamat I2C, Jumlah kolom, Jumlah baris

SoftwareSerial STM32Serial(STM32_TX, STM32_RX);

const int DOpin = A0;
const int pHpin = A1;
int fermentationDay;
float voltageDO, voltagepH, offsetDO, offsetvoltage;
float valueDO, valuepH;
float hasilDO, hasilpH;
float fromlow, fromhigh;
float tolow, tohigh;
float totalhari;
String status_valve, status_pupuk;

void setup() {
  // put your setup code here, to run once:
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);
  STM32Serial.begin(9600); // Inisialisasi SoftwareSerial
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);

}

void loop() {
  // Read fermentation day data from ESP32
  if (STM32Serial.available()) {
    String fermentationDayStr = STM32Serial.readStringUntil('\n');
    fermentationDayStr.trim();
    fermentationDay = fermentationDayStr.toInt();
    Serial.print("Fermentation Day: ");
    Serial.println(fermentationDay);
  }

  // put your main code here, to run repeatedly:
  digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(1000);                      // wait for a second
  valueDO = analogRead(DOpin);
  voltageDO = (valueDO/1023.0)*3.3;
  offsetDO = voltageDO - 2.25;

  valuepH = analogRead(pHpin);
  voltagepH = (valuepH/1023.0)*3.3;
  voltagepH = (3.3/1023.0)*valuepH;
  offsetvoltage = voltagepH - 1.3;

  hasilDO = (5.5037 * voltageDO) + 0.1711;

  hasilpH = (-6.2064 * voltagepH) + 23.054;

  if(hasilpH > 5 && hasilpH <= 7 && fermentationDay >= 14){
    digitalWrite(RELAY_PIN, HIGH); //nyalakan relay
    status_valve = "ON";
  } else {
    digitalWrite(RELAY_PIN, LOW); // Matikan relay
    status_valve = "OFF";
  }

  // mengirim ke esp32 (fungsi IOT)
  // STM32Serial.print("DO Value = ");
  STM32Serial.print(hasilDO);
  // STM32Serial.print(" mg/L ");
  STM32Serial.print(" || ");
  // STM32Serial.print("Analog Value = ");
  // STM32Serial.print(valueDO);
  // STM32Serial.print(" || ");
  // STM32Serial.print("Voltage Value = ");
  // STM32Serial.println(voltageDO);
  // STM32Serial.print("pH Value = ");
  STM32Serial.print(hasilpH);
  STM32Serial.print(" || ");
  // STM32Serial.print("Analog Value pH = ");
  // STM32Serial.println(valuepH);
  // STM32Serial.print(" || ");
  // STM32Serial.print("Voltage Value pH = ");
  // STM32Serial.println(voltagepH);
  // STM32Serial.print("Status Valve = ");
  STM32Serial.print(status_valve);
  STM32Serial.println(" || ");

  lcd.clear();
  //tampilan ke LCD
  lcd.setCursor(3, 0);
  lcd.print("DO = " + String(hasilDO) + " mg/l");

  lcd.setCursor(5, 1);
  lcd.print("pH = " + String(hasilpH));

  lcd.setCursor(4, 2);
  lcd.print("Valve = " + String(status_valve));

  // lcd.setCursor(0, 3);
  // lcd.print("V DO=" + String(voltageDO) + "||V pH=" + String(offsetvoltage));

  lcd.setCursor(8, 3);
  lcd.print("Day:" + String(fermentationDay));

  //Menampilkan ke serial monitor
  Serial.print("DO Value = ");
  Serial.print(hasilDO);
  Serial.print(" mg/L ");
  Serial.print(" || ");
  Serial.print("Analog Value = ");
  Serial.print(valueDO);
  Serial.print(" || ");
  Serial.print("Voltage Value = ");
  Serial.print(voltageDO);
  Serial.print(" || ");
  Serial.print("pH Value = ");
  Serial.print(hasilpH);
  Serial.print(" || ");
  Serial.print("Analog Value pH = ");
  Serial.print(valuepH);
  Serial.print(" || ");
  Serial.print("Voltage Value pH = ");
  Serial.print(voltagepH);
  // Serial.print(offsetvoltage);
  Serial.print(" || ");
  Serial.print("Status Valve = ");
  Serial.println(status_valve);
  
  digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
  delay(1000);

}
