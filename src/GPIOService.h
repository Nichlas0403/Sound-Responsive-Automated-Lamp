#ifndef GPIOService_h
#define GPIOService_h
#include "Arduino.h"

class GPIOService
{
    private:
        int _relayGPIO;

        int _soundSensorGPIO;
        unsigned long _soundSensorCurrentMillis;
        unsigned long _soundSensorTimeAtFirstTrigger;
        int _soundSensorRebounce;
        bool _soundSensorFirstSoundSensorTriggered;
        int _soundSensorTimeBeforeTriggerReset;
        
        int GetSoundSensorState();

    public:
        GPIOService(int relayGPIO, int soundSensorGPIO);
        bool _relayIsOn;
        void SoundSensorTrigger(); 
        void SetRelayState(int state);
};

#endif
