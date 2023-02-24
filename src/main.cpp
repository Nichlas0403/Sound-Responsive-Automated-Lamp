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
const String _timeBetweenChecksFlash = "_timeBetweenChecks";
const String _photoresistorThresholdFlash = "_photoresistorThreshold";

//States
const int _defaultState = 0;
const int _turnedOnManually = 1;
const int _turnedOffManually = 2;
const int _turnedOnAutomatically = 3;
const int _turnedOffAutomatically = 4;
volatile int _currentState = _defaultState;

//Environment Checks
unsigned long _currentTime;
unsigned long _millisAtLastCheck;
unsigned long _timeBetweenChecks;
unsigned int _photoresistorThreshold;
const int _turnOnAutomaticallyHour = 16;
const int _turnOffAutomaticallyHour = 1;
const int _resetSystemHour = 10;

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
  _GPIOService.relayIsHigh = true;

  _httpService.SetCSCSBaseUrl(_flashService.ReadFromFlash(_cscsBaseUrlFlash));
  _wifiName = _flashService.ReadFromFlash(_wifiNameFlash);
  _wifiPassword = _flashService.ReadFromFlash(_wifiPasswordFlash);
  _soundResponseSetting = _flashService.ReadFromFlash(_soundResponsiveSettingFlash).toInt();
  _timeBetweenChecks = _flashService.ReadFromFlash(_timeBetweenChecksFlash).toInt();
  _photoresistorThreshold = _flashService.ReadFromFlash(_photoresistorThresholdFlash).toInt();

  //TODO: Save these values to flash and read
  _environmentService.SetCoreValues(_photoresistorThreshold, _turnOnAutomaticallyHour, _turnOffAutomaticallyHour, _resetSystemHour, _minHour, _maxHour);

  connectToWiFi(); 

}

void loop()
{
  
  _server.handleClient();

  if(_soundResponseSetting)
  {
    int returnedState = _GPIOService.SoundSensorTrigger(_currentState);

    if(returnedState != _currentState)
    {
      int currentHour = _httpService.GetCurrentDateTime().substring(12,14).toInt();
      
      if(_environmentService.IsAutomationTime(currentHour))
      {
        _currentState = returnedState;
      }
    }
  }

  _currentTime = millis();

  if(_currentTime - _millisAtLastCheck < _timeBetweenChecks)
    return;

  _millisAtLastCheck = _currentTime;

  int currentHour = _httpService.GetCurrentDateTime().substring(12,14).toInt();

  if(_currentState == _defaultState && 
  !_GPIOService.relayIsHigh &&
  _environmentService.IsAutomationTime(currentHour)) 
    _currentState = _turnedOnManually;
  

  if(_currentState == _defaultState && _environmentService.ShouldTurnLightsOn(currentHour))
  {
    _GPIOService.SetRelayState(LOW);
    _currentState = _turnedOnAutomatically;
    currentHour = _turnOffAutomaticallyHour + 1;
  }
  else if(_currentState != _defaultState &&
  _currentState != _turnedOffAutomatically && 
  _currentState != _turnedOffManually && 
  _environmentService.ShouldTurnLightsOff(currentHour))
  {
    _GPIOService.SetRelayState(HIGH);
    _currentState = _turnedOffAutomatically;
    currentHour = _resetSystemHour;
  }
  else if(_currentState != _defaultState && currentHour == _resetSystemHour)
  {
    ESP.restart();
  }  
  
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

  if(_GPIOService.relayIsHigh)
    _GPIOService.SetRelayState(LOW);
  else
    _GPIOService.SetRelayState(HIGH);

  //Goal of this endpoint is for the relay to act right away. Therefore this http request is handled afterwards
  //and an extra if-statement is required.
  int currentHour = _httpService.GetCurrentDateTime().substring(12,14).toInt();
      
  if(_environmentService.IsAutomationTime(currentHour))
  {
    if(_GPIOService.relayIsHigh)
      _currentState = _turnedOffManually;
    else
      _currentState = _turnedOnManually;
  }

  Serial.println(_currentState);

  _server.send(200);

}

void updateCscsBaseUrl()
{

  String arg = "cscsBaseurl";

  if(!_server.hasArg(arg))
  {
    _server.send(400, "text/json","Missing parameter: " + arg);
    return;
  }

  String newUrl = _server.arg(arg);

  _flashService.WriteToFlash(_cscsBaseUrlFlash, newUrl);
  _httpService.SetCSCSBaseUrl(newUrl);

  _server.send(200, "text/json", _flashService.ReadFromFlash(_cscsBaseUrlFlash));
}

void updateTimeBetweenChecks()
{

  String arg = "timeBetweenChecks";

  if(!_server.hasArg(arg))
  {
    _server.send(400, "text/json","Missing parameter: " + arg);
    return;
  }

  long newTimeBetweenChecks = _server.arg(arg).toInt();

  if(newTimeBetweenChecks < 60000)
  {
    _server.send(400, "text/json","Error: Minimum value is 60000 ms (1 minute)");
    return;
  }

  _timeBetweenChecks = newTimeBetweenChecks;
  _flashService.WriteToFlash(_timeBetweenChecksFlash, String(_timeBetweenChecks));

  _server.send(200);
}

void updatePhotoresistorThreshold()
{
  String arg = "threshold";

  if(!_server.hasArg(arg))
  {
    _server.send(400, "text/json","Missing parameter: " + arg);
    return;
  }

  int newThreshold = _server.arg(arg).toInt();

  if(newThreshold < 0)
  {
    _server.send(400, "text/json","Error: Minimum value is 0");
    return;
  }

  _photoresistorThreshold = newThreshold;
  _flashService.WriteToFlash(_photoresistorThresholdFlash, String(_photoresistorThreshold));
  _environmentService.SetCoreValues(_photoresistorThreshold, _turnOnAutomaticallyHour, _turnOffAutomaticallyHour, _resetSystemHour, _minHour, _maxHour);

  _server.send(200);
}



// Core server functionality
void restServerRouting() 
{
  _server.on(F("/health-check"), HTTP_GET, healthCheck);
  _server.on(F("/sound-response"), HTTP_PUT, toggleSoundResponseSetting);
  _server.on(F("/relay"), HTTP_PUT, toggleRelay);
  _server.on(F("/cscs-baseurl"), HTTP_PUT, updateCscsBaseUrl);
  _server.on(F("/time-between-checks"), HTTP_PUT, updateTimeBetweenChecks);
  _server.on(F("/photoresistor-threshold"), HTTP_PUT, updatePhotoresistorThreshold);
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