#include "HttpService.h"

HttpService::HttpService() : _client(), _wifiClient(), _urlEncoderDecoder()
{
 _sendSMSSubUrl = "/send-SMS";
 _dateTimeSubUrl = "/current-datetime";
 _photoresistorSubUrl = "/photoresistor-value";
}

void HttpService::SetCSCSBaseUrl(String baseUrl)
{
  _baseUrl = baseUrl;
}

int HttpService::GetPhotoresistorValue()
{
  _client.begin(_wifiClient, _baseUrl + _photoresistorSubUrl);

  int httpResponseCode = _client.GET();
  int response;

  if(httpResponseCode > 0)
  {
    response = _client.getString().toInt();
  }
  else
  {
    response = httpResponseCode;
  }

  _client.end();
  return response;

}

String HttpService::GetCurrentDateTime()
{
  _client.begin(_wifiClient, _baseUrl + _dateTimeSubUrl);

  int httpResponseCode = _client.GET();
  String response;

  if(httpResponseCode > 0)
  {
    response = _client.getString();
  }
  else
  {
    response = String(httpResponseCode);
  }

  _client.end();
  return response;
}


bool HttpService::BluetoothCheck()
{
  return true;
}

bool HttpService::WiFiCheck()
{
  return true;
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