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


bool EnvironmentService::IsAutomationTime(int currentHour)
{
        if((currentHour >= _turnOnAutomaticallyTime && currentHour <= _maxHour) ||
       (currentHour <= _turnOffAutomaticallyTime && currentHour >= _minHour))
            return true;

        return false;
}

bool EnvironmentService::ShouldTurnLightsOn(int currentHour)
{

    if(IsAutomationTime(currentHour))
    {
        bool bluetoothCheck;
        bool wiFiCheck;

        // bluetoothCheck = _httpService.BluetoothCheck();
        bluetoothCheck = true;

        if(!bluetoothCheck)
        {
            // wiFiCheck = _httpService.WiFiCheck();
            wiFiCheck = false;
        }

        if(!bluetoothCheck && !wiFiCheck)
        {
            return false;
        }

        int photoresistorValue = _httpService.GetPhotoresistorValue();

        bool photoresistorCheck = photoresistorValue <= _photoresistorThreshold;

        if(!photoresistorCheck)
            return false;

        return true;
    }

    return false;
}
        
bool EnvironmentService::ShouldTurnLightsOff(int currentHour)
{
    if(currentHour > _turnOffAutomaticallyTime && currentHour < _resetSystemTime)
        return true;
    else
        return false;
}
