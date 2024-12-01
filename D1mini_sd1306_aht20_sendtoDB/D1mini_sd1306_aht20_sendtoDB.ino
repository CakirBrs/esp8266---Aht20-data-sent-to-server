
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Wire.h>
#include <AHT20.h>
#include "ssd1306.h"
#include "nano_gfx.h"

#define SEALEVELPRESSURE_HPA (1013.25)
AHT20 aht20;

//You have to change this section according to your own settings.
const char* ssid     = "***";
const char* password = "***";
const char* serverName = "http://dbServerID/dbpost.php";
String apiKeyValue = "tPmAT5Ab3j7F9";
String sensorName = "AHT20";
String sensorLocation = "Room";



void setup()
{
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) { 
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Humidity AHT20 examples");

  Wire.begin(); //Join I2C bus
  //Check if the AHT20 will acknowledge
  if (aht20.begin() == false)
  {
    Serial.println("AHT20 not detected. Please check wiring. Freezing.");
    while (1);
  }
  Serial.println("AHT20 acknowledged.");

  ssd1306_setFixedFont(ssd1306xled_font8x16);
  ssd1306_128x64_i2c_init();
  ssd1306_clearScreen();
  delay(500);
}




void loop()
{

  if(WiFi.status()== WL_CONNECTED && aht20.available() == true){
    WiFiClient client;
    HTTPClient http;
    float temperature = aht20.getTemperature();
    float humidity = aht20.getHumidity();

    char textTemperature[25];
    char textHumidity[25];
    sprintf(textTemperature, "Temp: %0.2f C", temperature);
    sprintf(textHumidity, "Hmd:  %0.2f %RH", humidity);
    
    ssd1306_setFixedFont(ssd1306xled_font8x16);
    ssd1306_clearScreen();
    ssd1306_printFixed(0,  8, textTemperature, STYLE_NORMAL);
    ssd1306_printFixed(0,  26, textHumidity, STYLE_NORMAL);


    Serial.print("Temperature: ");
    Serial.print(temperature, 2);
    Serial.print(" C\t");
    Serial.print("Humidity: ");
    Serial.print(humidity, 2);
    Serial.print("% RH");
    Serial.println();
    
    // Your Domain name with URL path or IP address with path
    http.begin(client, serverName);
    
    // Specify content-type header
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    
    // Prepare your HTTP POST request data
    String httpRequestData = "api_key=" + apiKeyValue + "&sensor=" + sensorName
                          + "&location=" + sensorLocation + "&value1=" + String(temperature)
                          + "&value2=" + String(humidity) + "&value3=" + "1" + ""; //third value for another example
    Serial.print("httpRequestData: ");
    Serial.println(httpRequestData);


    // Send HTTP POST request
    int httpResponseCode = http.POST(httpRequestData);
        
    if (httpResponseCode>0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
    }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    // Free resources
    http.end();
  }
  else {
    Serial.println("WiFi Disconnected");
  }
  //Send an HTTP POST request every 60 seconds
  delay(60000);  
}