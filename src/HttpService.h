#ifndef HttpService_h
#define HttpService_h
#include "Arduino.h"
#include <ESP8266HttpClient.h>
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

    public:
        HttpService();
        String SendSMS(String message);
};

#endif
