#include "HttpService.h"
#include "Arduino.h"
#include "FlashService.h"

HttpService::HttpService() : _client(), _wifiClient(), _urlEncoderDecoder()
{
  FlashService _flashService;

 _baseUrl = _flashService.ReadFromFlash("_cscsBaseUrl");
 _sendSMSSubUrl = "/send-SMS";

}

String HttpService::SendSMS(String message)
{
    message = _urlEncoderDecoder.urlencode(message);
    String url = _baseUrl + _sendSMSSubUrl + "?message=" + message;
    Serial.println(url);
    _client.begin(_wifiClient, url);

    int httpResponseCode = _client.sendRequest("POST");

    if (httpResponseCode > 0) {
        String payload = _client.getString();
        _client.end();
        return payload;
      }
      else 
      {
        Serial.print("Error code: ");
        Serial.println(_client.errorToString(httpResponseCode));
        Serial.println(httpResponseCode);
        _client.end();
        return String(httpResponseCode);
      }
}
