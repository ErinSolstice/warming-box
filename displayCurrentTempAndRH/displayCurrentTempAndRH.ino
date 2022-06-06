#include <Adafruit_SHT4x.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

Adafruit_SHT4x sht4 = Adafruit_SHT4x();

Adafruit_SH1107 display = Adafruit_SH1107(64, 128, &Wire);

// OLED FeatherWing buttons map to different pins depending on board:
#if defined(ESP8266)
  #define BUTTON_A  0
  #define BUTTON_B 16
  #define BUTTON_C  2
#elif defined(ESP32) && !defined(ARDUINO_ADAFRUIT_FEATHER_ESP32S2)
  #define BUTTON_A 15
  #define BUTTON_B 32
  #define BUTTON_C 14
#elif defined(ARDUINO_STM32_FEATHER)
  #define BUTTON_A PA15
  #define BUTTON_B PC7
  #define BUTTON_C PC5
#elif defined(TEENSYDUINO)
  #define BUTTON_A  4
  #define BUTTON_B  3
  #define BUTTON_C  8
#elif defined(ARDUINO_NRF52832_FEATHER)
  #define BUTTON_A 31
  #define BUTTON_B 30
  #define BUTTON_C 27
#else // 32u4, M0, M4, nrf52840, esp32-s2 and 328p
  #define BUTTON_A  9
  #define BUTTON_B  6
  #define BUTTON_C  5
#endif


#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char* sbrk(int incr);
#else  // __ARM__
extern char *__brkval;
#endif  // __arm__

int freeMemory() {
  char top;
#ifdef __arm__
  return &top - reinterpret_cast<char*>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
  return &top - __brkval;
#else  // __arm__
  return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif  // __arm__
}


void setup() {
  Serial.begin(115200);
  Serial.println(freeMemory());

  // Seup fr emperaure sensr
  Serial.println(F("Adafruit SHT4x initializing"));
  if (! sht4.begin()) {
    Serial.println(F("Couldn't find SHT4x"));
    while (1) delay(1);
  }
  Serial.println(F("Found SHT4x sensor"));
  Serial.print(F("Serial number 0x"));
  Serial.println(sht4.readSerial(), HEX);

  // You can have 3 different precisions, higher precision takes longer
  sht4.setPrecision(SHT4X_HIGH_PRECISION);
  switch (sht4.getPrecision()) {
     case SHT4X_HIGH_PRECISION: 
       Serial.println(F("High precision"));
       break;
     case SHT4X_MED_PRECISION: 
       Serial.println(F("Med precision"));
       break;
     case SHT4X_LOW_PRECISION: 
       Serial.println(F("Low precision"));
       break;
  }

  // You can have 6 different heater settings
  // higher heat and longer times uses more power
  // and reads will take longer too!
  sht4.setHeater(SHT4X_NO_HEATER);
  switch (sht4.getHeater()) {
     case SHT4X_NO_HEATER: 
       Serial.println(F("No heater"));
       break;
     case SHT4X_HIGH_HEATER_1S: 
       Serial.println("High heat for 1 second");
       break;
     case SHT4X_HIGH_HEATER_100MS: 
       Serial.println(F("High heat for 0.1 second"));
       break;
     case SHT4X_MED_HEATER_1S: 
       Serial.println(F("Medium heat for 1 second"));
       break;
     case SHT4X_MED_HEATER_100MS: 
       Serial.println(F("Medium heat for 0.1 second"));
       break;
     case SHT4X_LOW_HEATER_1S: 
       Serial.println(F("Low heat for 1 second"));
       break;
     case SHT4X_LOW_HEATER_100MS: 
       Serial.println(F("Low heat for 0.1 second"));
       break;
  }
  

  // Seup fr led displa
  Serial.println(F("128x64 OLED FeatherWing initializing"));
  delay(1000); // wait for the OLED to power up
  Serial.println(freeMemory());
  Serial.println(F("Done waiting"));
  if (! display.begin(0x3C, true)) { // Address 0x3C default
    Serial.println(F("Display memory allocation failed. Trying again."));
    while (1) delay(1);
  }
  Serial.println(F("OLED begun"));

  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();
  delay(1000);

  // Clear the buffer.
  display.clearDisplay();
  display.display();

  display.setRotation(1);

  Serial.println(F("Displaying text"));
  // text display tests
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0,0);
  display.println(F("Temp: "));
  display.println(F("Humidity: "));
  display.println(F("Read time (ms): "));
  display.println(F("\nTarget Temp: "));
  delay(10);
  yield();
  display.display(); // actually display all of the above
}


void loop() {
  sensors_event_t humidity, temp;
  
  uint32_t timestamp = millis();
  sht4.getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data
  timestamp = millis() - timestamp;

  // Clear the buffer.
  display.clearDisplay();

  display.setCursor(0,0);
  display.print(F("Temp: ")); display.print(temp.temperature); display.println(F(" deg C"));
  display.print(F("Humidity: ")); display.print(humidity.relative_humidity); display.println(F("% rH"));
  display.print(F("Read time (ms): ")); display.println(timestamp);
  display.println(F("\nTarget Temp: "));
  delay(10);
  yield();
  display.display();

  delay(1000);
}
