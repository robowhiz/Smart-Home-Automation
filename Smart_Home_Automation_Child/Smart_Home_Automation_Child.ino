#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

const char *ssid = "POCO M3";
const char *password = "********";

String serverName = "http://homeconnect.local:80/update?";

void httpGETRequest(const char *path);

unsigned long lastTime = 0;
unsigned long timerDelay = 100;

#define LAUNDRY D1
#define LAUNDRY_IDEL_TIME 5000 // 60 seconds of no vibration to consider laundry done

uint32_t lastVibrationTime = 0;
bool laundryRunning = false;
uint8_t vibrationValue;

uint16_t laserValue;
bool securityBreach = false;

void setup()
{
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  pinMode(LAUNDRY, INPUT);
}

void loop()
{
  if ((millis() - lastTime) > timerDelay)
  {
    laserValue = analogRead(A0);
    Serial.println(laserValue);
    if(!securityBreach && laserValue > 300){
      securityBreach = true;
      httpGETRequest("securityBreach=1");
    }
    else if(securityBreach && laserValue < 300){
      securityBreach = false;
      httpGETRequest("securityBreach=0");
    }
    vibrationValue = digitalRead(LAUNDRY);
    // Serial.println(vibrationValue);

    if (vibrationValue == 1)
    {
      lastVibrationTime = millis();
      if (!laundryRunning)
      {
        laundryRunning = true;
        httpGETRequest("laundry=1");
        // Serial.println("Laundry cycle started!");
      }
    }

    if (laundryRunning && (millis() - lastVibrationTime > LAUNDRY_IDEL_TIME))
    {
      laundryRunning = false;
      httpGETRequest("laundry=0");
      // Serial.println("Laundry cycle completed!");
    }
    lastTime = millis();
  }
}

void httpGETRequest(const char *path)
{
  if (WiFi.status() == WL_CONNECTED)
  {
    WiFiClient client;
    HTTPClient http;

    String serverPath = serverName + path;

    http.begin(client, serverPath.c_str());

    int httpResponseCode = http.GET();

    if (httpResponseCode > 0)
    {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      String payload = http.getString();
      Serial.println(payload);
    }
    else
    {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  }
  else
  {
    Serial.println("WiFi Disconnected");
  }
}