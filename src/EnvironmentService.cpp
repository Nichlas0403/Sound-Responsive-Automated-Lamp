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

bool EnvironmentService::ShouldTurnLightsOn()
{
    bool bluetoothAndWiFiCheck = _httpService.BluetoothAndWiFiCheck();
    int photoresistorValue = _httpService.GetPhotoresistorValue();
    String currentHour = _httpService.GetCurrentDateTime().substring(12,14);

    bool photoresistorCheck = photoresistorValue < _photoresistorThreshold;
    bool hourCheck = currentHour > _turnOnAutomaticallyTime && currentHour < _turnOffAutomaticallyTime;

    if(photoresistorCheck && hourCheck && bluetoothAndWiFiCheck)
        return true;
    else
        return false;

}
        
bool EnvironmentService::ShouldTurnLightsOff()
{
    
}


bool EnvironmentService::ShouldResetSystem()
{

}

// int _photoresistorThreshold = 700;
// String _turnOnAutomaticallyTime = "16-00-00";
// String _turnOffAutomaticallyTime = "01-00-00";
// String _resetSystemTime = "10-00-00";