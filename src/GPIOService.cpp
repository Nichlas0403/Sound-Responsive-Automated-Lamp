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

    if(state == HIGH && !relayIsHigh)
    {
      relayIsHigh = true;
      digitalWrite(_relayGPIO, HIGH);
    }
    else if(state == LOW && relayIsHigh)
    {
      relayIsHigh = false;
      digitalWrite(_relayGPIO, LOW);
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
      if(relayIsHigh)
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