// This example demonstrates DTMF tone generation for each digit starting from 0.

#include "dtmf.h"

// Create new instance of DTMF generator.
DTMF dtmf;

void setup() {
  Serial.begin(115200);
}
float oldBaudRate;

void loop() {
  // Generate DTMF tones
  int sensorValue = analogRead(A0);
  
  int duration = sensorValue/10;
  float baudRate = 1000/duration;
  if (oldBaudRate != baudRate){
    oldBaudRate = baudRate;
    Serial.print("baudRate:");
    Serial.print(baudRate);
    Serial.print(", bitrate=");
    Serial.println(baudRate*4);
  }
  String s="2035551212";
  int n = s.length();
  char c[n+1]; 
  s.toCharArray(c, n);
  for (int i=0; i < n;i++){
    dtmf.generate(c[i], duration*2);
    delay(duration);
  }

}
