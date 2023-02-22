#include "EnvironmentService.h"

EnvironmentService::EnvironmentService() : _httpService()
{
}

void EnvironmentService::SetCoreValues(int photoresistorThreshold, int turnOnAutomaticallyHour, int turnOffAutomaticallyHour, int resetSystemHour, int minHour, int maxHour)
{
    _photoresistorThreshold = photoresistorThreshold;
    _turnOnAutomaticallyTime = turnOnAutomaticallyHour;
    _turnOffAutomaticallyTime = turnOffAutomaticallyHour;
    _resetSystemTime = resetSystemHour;
    _minHour = minHour;
    _maxHour = maxHour;
}

bool EnvironmentService::ShouldTurnLightsOn(int currentHour)
{

    if((currentHour >= _turnOnAutomaticallyTime && currentHour <= _maxHour) ||
       (currentHour <= _turnOffAutomaticallyTime && currentHour >= _minHour))
    {
        bool bluetoothAndWiFiCheck = _httpService.BluetoothAndWiFiCheck();
        int photoresistorValue = _httpService.GetPhotoresistorValue();

        bool photoresistorCheck = photoresistorValue < _photoresistorThreshold;

        if(photoresistorCheck && bluetoothAndWiFiCheck)
            return true;
        else
            return false;
    }
    else
        return false;
      



}
        
bool EnvironmentService::ShouldTurnLightsOff(int currentHour)
{
    if(currentHour > _turnOffAutomaticallyTime && currentHour < _resetSystemTime)
        return true;
    else
        return false;
}
