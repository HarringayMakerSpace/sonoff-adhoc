/**
   Sonoff ad-hoc contol via Wifi probe requests

   This shows how to control a Sonoff Wifi switch without needing to configure
   it for a Wifi access point.

   Author: Anthony Elder
   License: Apache License v2
*/
#include <ESP8266WiFi.h>

// Some random byte to identify this Sonoff
#define SONOFF_ID 01

#define SONOFF_RELAY 12
#define DEVICE_BUTTON 0

WiFiEventHandler probeRequestPrintHandler;

void setup() {
  Serial.begin(115200); Serial.println();
  Serial.println("Sonoff probe request control");

  pinMode(SONOFF_RELAY, OUTPUT);

  WiFi.persistent(false);
  WiFi.mode(WIFI_AP);
  WiFi.softAP("sonoff", "<notused>", true, 0);

  probeRequestPrintHandler = WiFi.onSoftAPModeProbeRequestReceived(&onProbeRequest);

  pinMode(DEVICE_BUTTON, INPUT);
  attachInterrupt(DEVICE_BUTTON, onButtonPress, HIGH);
}

void onProbeRequest(const WiFiEventSoftAPModeProbeRequestReceived& evt) {
  if (evt.mac[0] != 0x36 || evt.mac[1] != SONOFF_ID) return;

  Serial.print("Got probe request, switching: ");
  if (evt.mac[5] == 0x01) {
    digitalWrite(SONOFF_RELAY, HIGH);
    Serial.println("on");
  } else {
    digitalWrite(SONOFF_RELAY, LOW);
    Serial.println("off");
  }
}

void loop() {
}

void onButtonPress() {
  digitalWrite(SONOFF_RELAY, !digitalRead(SONOFF_RELAY));
  Serial.print("Button pressed, switched: "); Serial.println(digitalRead(SONOFF_RELAY) ? "on" : "off");
}
