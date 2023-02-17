#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include "FlashService.h"
#include "HttpService.h"
#include "SensorService.h"

//GPIO
#define relay D7
#define soundSensor D8

//Core variables
String _wifiName = "";
String _wifiPassword = "";

//Flash addresses
const String _wifiNameFlash = "_wifiName";
const String _wifiPasswordFlash = "_wifiPassword";
const String _cscsBaseUrlFlash = "_cscsBaseUrl";

//Services
ESP8266WebServer _server(80);
FlashService _flashService;
HttpService _httpService;
SensorService _sensorService(relay, soundSensor);

//Core server functionality
void restServerRouting();
void connectToWiFi();

void setup()
{
  Serial.begin(9600); 

  _wifiName = _flashService.ReadFromFlash(_wifiNameFlash);
  _wifiPassword = _flashService.ReadFromFlash(_wifiPasswordFlash);

  connectToWiFi();
}

void loop()
{
  _sensorService.SoundSensorTrigger();
}










// ------------------- API -----------------------



// Core server functionality
void restServerRouting() 
{
  // _server.on(F("/health-check"), HTTP_GET, HealthCheck);
}

void handleNotFound() 
{
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += _server.uri();
  message += "_server: ";
  message += (_server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += _server.args();
  message += "\n";

  for (uint8_t i = 0; i < _server.args(); i++) 
  {
    message += " " + _server.argName(i) + ": " + _server.arg(i) + "\n";
  }

  _server.send(404, "text/plain", message);
}

void connectToWiFi()
{
  WiFi.mode(WIFI_STA);

  if (WiFi.SSID() != _wifiName) 
  {
    Serial.println("Creating new connection to wifi");
    WiFi.begin(_wifiName, _wifiPassword);
    WiFi.persistent(true);
    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);
  }
  else
  {
    Serial.println("Using existing wifi settings...");
  }

 
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(_wifiName);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
 
  // Activate mDNS this is used to be able to connect to the server
  // with local DNS hostmane esp8266.local
  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }
 
  // Set server routing
  restServerRouting();
  // Set not found response
  _server.onNotFound(handleNotFound);
  // Start server
  _server.begin();

  Serial.println("HTTP server started");
}