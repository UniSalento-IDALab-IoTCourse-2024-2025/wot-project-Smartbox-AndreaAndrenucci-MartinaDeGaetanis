#ifndef REGISTER_DEVICE
#define REGISTER_DEVICE

#include <WebServer.h>

extern WebServer server;

void connectToWifi();
void serverRoutesConfig();
void handleWiFiOpt();
void geocodeAddress(String city, String street);

#endif
