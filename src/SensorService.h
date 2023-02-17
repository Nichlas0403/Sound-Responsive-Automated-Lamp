#ifndef SensorService_h
#define SensorService_h
#include "Arduino.h"

class SensorService
{
    private:
        int _relayGPIO;
        bool _relayIsOn;

        int _soundSensorGPIO;
        unsigned long _soundSensorCurrentMillis;
        unsigned long _soundSensorTimeAtFirstTrigger;
        int _soundSensorRebounce;
        bool _soundSensorFirstSoundSensorTriggered;
        int _soundSensorTimeBeforeTriggerReset;
        
        void SetRelayState(int state);
        int GetSoundSensorState();

    public:
        SensorService(int relayGPIO, int soundSensorGPIO);
        void SoundSensorTrigger(); 
};

#endif
