#include "GPIOService.h"
#include "Arduino.h"

GPIOService::GPIOService(int relayGPIO, int soundSensorGPIO)
{
    _relayGPIO = relayGPIO;
    _soundSensorGPIO = soundSensorGPIO;
    _soundSensorRebounce = 200;
    _soundSensorTimeBeforeTriggerReset = 500;
    SetRelayState(HIGH);
}

void GPIOService::SetRelayState(int state)
{
    if(state == HIGH)
      _relayIsOn = true;
    else
      _relayIsOn = false;
      
    digitalWrite(_relayGPIO, state);
}
        
int GPIOService::GetSoundSensorState()
{
    return digitalRead(_soundSensorGPIO);
}

void GPIOService::SoundSensorTrigger()
{
    _soundSensorCurrentMillis = millis();
  
  if((_soundSensorCurrentMillis - _soundSensorTimeAtFirstTrigger > _soundSensorRebounce) && (GetSoundSensorState() == HIGH))
  {
    _soundSensorTimeAtFirstTrigger = _soundSensorCurrentMillis;

    if(_soundSensorFirstSoundSensorTriggered)
    {
      if(_relayIsOn)
        SetRelayState(LOW);
      else
        SetRelayState(HIGH);
    }
    else
    {
      _soundSensorFirstSoundSensorTriggered = true;
      _soundSensorTimeAtFirstTrigger = _soundSensorCurrentMillis;
    }
  }
  else if(_soundSensorCurrentMillis - _soundSensorTimeAtFirstTrigger > _soundSensorTimeBeforeTriggerReset)
    _soundSensorFirstSoundSensorTriggered = false;
}