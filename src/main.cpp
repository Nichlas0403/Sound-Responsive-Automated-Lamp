#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include "FlashService.h"
#include "HttpService.h"
#include "GPIOService.h"
#include "EnvironmentService.h"

//GPIO
#define relayGPIO D5
#define soundSensorGPIO D8

//Core variables
String _wifiName = "";
String _wifiPassword = "";
bool _soundResponseSetting;

//Flash addresses
const String _wifiNameFlash = "_wifiName";
const String _wifiPasswordFlash = "_wifiPassword";
const String _cscsBaseUrlFlash = "_cscsBaseUrl";
const String _soundResponsiveSettingFlash = "_soundReponsiveSetting";

//States
const int _defaultState = 0;
const int _turnedOnManually = 1;
const int _turnedOffManually = 2;
const int _turnedOnAutomatically = 3;
const int _turnedOffAutomatically = 4;
volatile int _currentState = _defaultState;

//Environment Checks
unsigned long _currentTime;
int _millisAtLastCheck;
// int _timeBetweenChecks = 900000; //15 minutes
int _timeBetweenChecks = 15000;
int _photoresistorThreshold = 700;
int _turnOnAutomaticallyHour = 16;
int _turnOffAutomaticallyHour = 1;
int _resetSystemHour = 10;

//Basic 24 H clock variables
const int _minHour = 0;
const int _maxHour = 23;

//Services
ESP8266WebServer _server(80);
FlashService _flashService;
HttpService _httpService;
GPIOService _GPIOService(relayGPIO, soundSensorGPIO, _turnedOnManually, _turnedOffManually);
EnvironmentService _environmentService;

//Core server functionality
void restServerRouting();
void connectToWiFi();


void setup()
{
  Serial.begin(9600); 

  pinMode(relayGPIO, OUTPUT);
  pinMode(soundSensorGPIO, INPUT); 
  digitalWrite(relayGPIO, HIGH);
  _GPIOService.relayIsOn = false;

  _wifiName = _flashService.ReadFromFlash(_wifiNameFlash);
  _wifiPassword = _flashService.ReadFromFlash(_wifiPasswordFlash);
  _soundResponseSetting = _flashService.ReadFromFlash(_soundResponsiveSettingFlash).toInt();

  //TODO: Save these values to flash and read
  _environmentService.SetCoreValues(_photoresistorThreshold, _turnOnAutomaticallyHour, _turnOffAutomaticallyHour, _resetSystemHour, _minHour, _maxHour);

  connectToWiFi(); 
}

void loop()
{
  
  _server.handleClient();

  // if(_soundResponseSetting)
  // {
  //   int returnedState = _GPIOService.SoundSensorTrigger(_currentState);

  //   if(returnedState != _currentState)
  //   {
  //     int currentHour = _httpService.GetCurrentDateTime().substring(12,14).toInt();
      
  //     if((currentHour >= _turnOnAutomaticallyHour && currentHour <= _maxHour) || 
  //        (currentHour >= _minHour && currentHour <= _turnOffAutomaticallyHour))
  //       _currentState = returnedState;
  //   }
  // }

  _currentTime = millis();

  if(_currentTime - _millisAtLastCheck < _timeBetweenChecks)
    return;

  _millisAtLastCheck = _currentTime;

  // int currentHour = _httpService.GetCurrentDateTime().substring(12,14).toInt();
  int currentHour = 16;

  if(_currentState == _defaultState && _environmentService.ShouldTurnLightsOn(currentHour))
  {
    _GPIOService.SetRelayState(LOW);
    _currentState = _turnedOnAutomatically;
  }
  // else if(_currentState != _turnedOffAutomatically && _currentState != _turnedOffManually && _environmentService.ShouldTurnLightsOff(currentHour))
  // {
  //   _GPIOService.SetRelayState(HIGH);
  //   _currentState = _turnedOffAutomatically;
  // }
  // else if(_currentState != _defaultState && currentHour == _resetSystemHour)
  // {
  //   ESP.restart();
  // }  

  

}










// ------------------- API -----------------------  

void healthCheck()
{
  _server.send(200);
}

void toggleSoundResponseSetting()
{
  if(_soundResponseSetting)
    _soundResponseSetting = false;
  else
    _soundResponseSetting = true;
  
  _flashService.WriteToFlash(_soundResponsiveSettingFlash, String(_soundResponseSetting));

  _server.send(200);
}

void toggleRelay()
{
  // if(_GPIOService.relayIsOn)
  // {
  //   _GPIOService.SetRelayState(LOW);
  //   _GPIOService.relayIsOn = false;
  //   _currentState = _turnedOffManually;
  // }
  // else
  // {
  //   _GPIOService.SetRelayState(HIGH);
  //   _GPIOService.relayIsOn = true;
  //   _currentState = _turnedOnManually;
  // }

  _server.send(200);

}



// Core server functionality
void restServerRouting() 
{
  _server.on(F("/health-check"), HTTP_GET, healthCheck);
  _server.on(F("/sound-response"), HTTP_PUT, toggleSoundResponseSetting);
  _server.on(F("/relay"), HTTP_PUT, toggleRelay);
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