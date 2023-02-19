#include "GPIOService.h"
#include "Arduino.h"

GPIOService::GPIOService(int relayGPIO, int soundSensorGPIO, int turnedOnManuallyState, int turnedOffManuallyState)
{
    _turnedOnManuallyState = turnedOnManuallyState;
    _turnedOffManuallyState = turnedOffManuallyState;
    _relayGPIO = relayGPIO;
    _soundSensorGPIO = soundSensorGPIO;
    _soundSensorRebounce = 200;
    _soundSensorTimeBeforeTriggerReset = 500;
    SetRelayState(HIGH);
}

void GPIOService::SetRelayState(int state)
{
    if(state == HIGH)
      relayIsOn = true;
    
    else
      relayIsOn = false;
    
    digitalWrite(_relayGPIO, state);
}
        
int GPIOService::GetSoundSensorState()
{
    return digitalRead(_soundSensorGPIO);
}

int GPIOService::SoundSensorTrigger(int currentState)
{
    _soundSensorCurrentMillis = millis();
  
  if((_soundSensorCurrentMillis - _soundSensorTimeAtFirstTrigger > _soundSensorRebounce) && (GetSoundSensorState() == HIGH))
  {
    _soundSensorTimeAtFirstTrigger = _soundSensorCurrentMillis;

    if(_soundSensorFirstSoundSensorTriggered)
    {
      if(relayIsOn)
      {
        SetRelayState(LOW);
        Serial.println("LOW SOUND");
        return _turnedOnManuallyState;
      }
      else
      {
        SetRelayState(HIGH);
        Serial.println("HIGH SOUND");
        return _turnedOffManuallyState;
      }
    }
    else
    {
      _soundSensorFirstSoundSensorTriggered = true;
      _soundSensorTimeAtFirstTrigger = _soundSensorCurrentMillis;
    }
  }
  else if(_soundSensorCurrentMillis - _soundSensorTimeAtFirstTrigger > _soundSensorTimeBeforeTriggerReset)
    _soundSensorFirstSoundSensorTriggered = false;

  return currentState;
}