#ifndef EnvironmentService_h
#define EnvironmentService_h
#include "Arduino.h"
#include <LittleFS.h>
#include "HttpService.h"

class EnvironmentService
{
    private:
        int _photoresistorThreshold;
        int _turnOnAutomaticallyTime;
        int _turnOffAutomaticallyTime;
        int _resetSystemTime;

        HttpService _httpService;

    public:
        EnvironmentService();
        void SetCoreValues(int photoresistorThreshold, int turnOnAutomaticallyHour, int turnOffAutomaticallyHour, int resetSystemHour);
        bool ShouldTurnLightsOn(int currentHour);
        bool ShouldTurnLightsOff(int currentHour);
        bool ShouldResetSystem(int currentHour);
};

#endif