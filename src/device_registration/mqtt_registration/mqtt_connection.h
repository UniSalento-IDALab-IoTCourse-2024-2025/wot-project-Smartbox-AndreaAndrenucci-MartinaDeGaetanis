#ifndef MQTT_CONNECTION_H
#define MQTT_CONNECTION_H

#include <PubSubClient.h>
#include <WiFi.h>
#include <ArduinoJson.h>


extern PubSubClient client;

JsonObject getLocation();
void setupMQTT();
void mqttLoop();
void sendMeasurements(const String& json);

#endif
