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
        int _minHour;
        int _maxHour;

        HttpService _httpService;

    public:
        EnvironmentService();
        void SetCoreValues(int photoresistorThreshold, int turnOnAutomaticallyHour, int turnOffAutomaticallyHour, int resetSystemHour, int minHour, int maxHour);
        bool ShouldTurnLightsOn(int currentHour);
        bool ShouldTurnLightsOff(int currentHour);
        bool IsAutomationTime(int currentHour);
};

#endif