#include "GPIOService.h"

GPIOService::GPIOService(int relayGPIO, int soundSensorGPIO, int turnedOnManuallyState, int turnedOffManuallyState)
{
    _turnedOnManuallyState = turnedOnManuallyState;
    _turnedOffManuallyState = turnedOffManuallyState;
    _relayGPIO = relayGPIO;
    _soundSensorGPIO = soundSensorGPIO;
    _soundSensorRebounce = 200;
    _soundSensorTimeBeforeTriggerReset = 500;
}

void GPIOService::SetRelayState(int state)
{
    if(state == HIGH && !relayIsOn)
    {
      relayIsOn = true;
      digitalWrite(_relayGPIO, LOW);
    }
    else if(state == LOW && relayIsOn)
    {
      relayIsOn = false;
      digitalWrite(_relayGPIO, HIGH);
    }
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
        return _turnedOnManuallyState;
      }
      else
      {
        SetRelayState(HIGH);
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