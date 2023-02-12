#include <Arduino.h>

#define sensor A0

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(sensor, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(analogRead(sensor));
  delay(100);
  

}