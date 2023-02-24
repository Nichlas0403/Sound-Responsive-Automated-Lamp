#ifndef HttpService_h
#define HttpService_h
#include <ESP8266HttpClient.h>
#include "Arduino.h"
#include "ArduinoJson.h"
#include "UrlEncoderDecoder.h"

class HttpService
{
    private:
        WiFiClient _wifiClient;
        HTTPClient _client;
        UrlEncoderDecoderService _urlEncoderDecoder;

        String _baseUrl;
        String _sendSMSSubUrl;
        String _photoresistorSubUrl;
        String _dateTimeSubUrl;

    public:
        HttpService();
        void SetCSCSBaseUrl(String baseUrl);
        String SendSMS(String message);
        int GetPhotoresistorValue();
        String GetCurrentDateTime();
        bool BluetoothCheck();
        bool WiFiCheck();
        // bool MobileWifiAndBluetoothCheck();
};

#endif
