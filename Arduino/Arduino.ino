#include <SHT1x.h>
#include <Adafruit_CC3000.h>
#include <ccspi.h>
#include <SPI.h>
#include <string.h>
#include "utility/debug.h"

#define dataPin 11
#define clockPin 13

// These are the interrupt and control pins
#define ADAFRUIT_CC3000_IRQ   3  // MUST be an interrupt pin!
// These can be any two pins
#define ADAFRUIT_CC3000_VBAT  5
#define ADAFRUIT_CC3000_CS    10
// Use hardware SPI for the remaining pins
// On an UNO, SCK = 13, MISO = 12, and MOSI = 11
Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS, 
                                         ADAFRUIT_CC3000_IRQ, 
                                         ADAFRUIT_CC3000_VBAT, 
                                         SPI_CLOCK_DIV2); // you can change this clock speed

#define WLAN_SSID       "Greenland"           // cannot be longer than 32 characters!
#define WLAN_PASS       "testtest"
// Security can be WLAN_SEC_UNSEC, WLAN_SEC_WEP, WLAN_SEC_WPA or WLAN_SEC_WPA2
#define WLAN_SECURITY   WLAN_SEC_WPA2

#define IDLE_TIMEOUT_MS  3000      // Amount of time to wait (in milliseconds) with no data 
                                   // received before closing the connection.  If you know the server
                                   // you're accessing is quick to respond, you can reduce this value.

uint32_t ip = cc3000.IP2U32(192,168,24,100);
int port = 8080;
float temp_c;
float temp_f;
float humidity;
int cl;
String temperature;
Adafruit_CC3000_Client www;

SHT1x sht1x(dataPin, clockPin);

void setup()
{
    Serial.begin(38400);
    Serial.println("SHT1x Starting up");
    Serial.println(F("Hello, CC3000!")); 
    Serial.print("Free RAM: "); Serial.println(getFreeRam(), DEC);
      /* Initialise the module */
    Serial.println(F("CC3000 Initializing..."));
    if (!cc3000.begin())
    {
      Serial.println(F("Couldn't begin()! Check your wiring?"));
      while(1);
    }
    
    if (!cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY)) {
      Serial.println(F("Failed!"));
      while(1);
    }

    Serial.println(F("WLAN Connected!"));
  
    /* Wait for DHCP to complete */
    Serial.println(F("Request DHCP"));
    while (!cc3000.checkDHCP())
    {
      delay(100); // ToDo: Insert a DHCP timeout!
    }  
}

void loop()
{
    www = cc3000.connectTCP(ip, port);
    // Read values from the sensor
    temp_c = sht1x.readTemperatureC();
    temp_f = sht1x.readTemperatureF();
    humidity = sht1x.readHumidity();
  
    // Print the values to the serial port
    Serial.print("Temperature: ");
    Serial.print(temp_c, DEC);
    Serial.print("C / ");
    Serial.print(temp_f, DEC);
    Serial.print("F. Humidity: ");
    Serial.print(humidity);
    Serial.println("%");
    
    if (www.connected()) {
      temperature = String((int) temp_c);
      String request = "GET /plan/changeTemp/" + temperature + "/ HTTP/1.1 ";
      www.println(request);
      Serial.println(request);
    } else {
      Serial.println(F("Connection failed"));   
      return;
    }
  
    Serial.println(F("-------------------------------------"));
    
    /* Read data until either the connection is closed, or the idle timeout is reached. */ 
    
    unsigned long lastRead = millis();
    while (www.connected() && (millis() - lastRead < IDLE_TIMEOUT_MS)) {
      while (www.available()) {
        char c = www.read();
        Serial.print(c);
        lastRead = millis();
      }
    }
    
    www.close();
    Serial.println(F("\n-------------------------------------"));
    
    /* You need to make sure to clean up after yourself or the CC3000 can freak out */
    /* the next time your try to connect ... */
    //Serial.println(F("\n\nDisconnecting"));
    //cc3000.disconnect();
}
