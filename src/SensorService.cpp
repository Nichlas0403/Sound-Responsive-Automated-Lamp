#include "SensorService.h"
#include "Arduino.h"

SensorService::SensorService(int relayGPIO, int soundSensorGPIO)
{
    _relayGPIO = relayGPIO;
    _soundSensorGPIO = soundSensorGPIO;

    pinMode(_relayGPIO, OUTPUT);
    pinMode(_soundSensorGPIO, INPUT); 
    SetRelayState(LOW);

    _relayIsOn = false;
    _soundSensorRebounce = 200;
    _soundSensorTimeBeforeTriggerReset = 500;
}

void SensorService::SetRelayState(int state)
{
    digitalWrite(_relayGPIO, state);
}
        
int SensorService::GetSoundSensorState()
{
    return digitalRead(_soundSensorGPIO);
}

void SensorService::SoundSensorTrigger()
{
    _soundSensorCurrentMillis = millis();
  
  if((_soundSensorCurrentMillis - _soundSensorTimeAtFirstTrigger > _soundSensorRebounce) && (GetSoundSensorState() == HIGH))
  {
    Serial.println("First trigger");
    _soundSensorTimeAtFirstTrigger = _soundSensorCurrentMillis;

    if(_soundSensorFirstSoundSensorTriggered)
    {
      if(_relayIsOn)
      {
        SetRelayState(LOW);
        _relayIsOn = false;
      }
      else
      {
        SetRelayState(HIGH);
        _relayIsOn = true;
      }

    }
    else
    {
      _soundSensorFirstSoundSensorTriggered = true;
      _soundSensorTimeAtFirstTrigger = _soundSensorCurrentMillis;
    }
  }
  else if(_soundSensorCurrentMillis - _soundSensorTimeAtFirstTrigger > _soundSensorTimeBeforeTriggerReset)
  {
    _soundSensorFirstSoundSensorTriggered = false;
  }
}