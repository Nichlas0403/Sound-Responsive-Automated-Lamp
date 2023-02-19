#include "EnvironmentService.h"

EnvironmentService::EnvironmentService() : _httpService()
{
}

void EnvironmentService::SetCoreValues(int photoresistorThreshold, int turnOnAutomaticallyHour, int turnOffAutomaticallyHour, int resetSystemHour)
{
    _photoresistorThreshold = photoresistorThreshold;
    _turnOnAutomaticallyTime = turnOnAutomaticallyHour;
    _turnOffAutomaticallyTime = turnOffAutomaticallyHour;
    _resetSystemTime = resetSystemHour;
}

bool EnvironmentService::ShouldTurnLightsOn(int currentHour)
{
    if(!(currentHour > _turnOnAutomaticallyTime && currentHour < _turnOffAutomaticallyTime))
        return false;

    bool bluetoothAndWiFiCheck = _httpService.BluetoothAndWiFiCheck();
    int photoresistorValue = _httpService.GetPhotoresistorValue();

    bool photoresistorCheck = photoresistorValue < _photoresistorThreshold;

    if(photoresistorCheck && bluetoothAndWiFiCheck)
        return true;
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


bool EnvironmentService::ShouldResetSystem(int currentHour)
{
    if(currentHour > _resetSystemTime && currentHour < _turnOnAutomaticallyTime)
        return true;
    else
        return false;
}

// int _photoresistorThreshold = 700;
// String _turnOnAutomaticallyTime = "16-00-00";
// String _turnOffAutomaticallyTime = "01-00-00";
// String _resetSystemTime = "10-00-00";