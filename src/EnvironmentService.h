#ifndef EnvironmentService_h
#define EnvironmentService_h
#include "Arduino.h"
#include <LittleFS.h>
#include "HttpService.h"

class EnvironmentService
{
    private:
        int _photoresistorThreshold;
        String _turnOnAutomaticallyTime;
        String _turnOffAutomaticallyTime;
        String _resetSystemTime;

        HttpService _httpService;

    public:
        EnvironmentService();
        void SetCoreValues(int photoresistorThreshold, int turnOnAutomaticallyHour, int turnOffAutomaticallyHour, int resetSystemHour);
        bool ShouldTurnLightsOn();
        bool ShouldTurnLightsOff();
        bool ShouldResetSystem();
};

#endif