#ifndef STORAGE_MANAGER_H
#define STORAGE_MANAGER_H

#include <Preferences.h>
#include <Arduino.h>

void saveAddressData(const String &city, const String &prov, const String &street, const String &lat, const String &lon);
void loadAddressData(String &city, String &prov, String &street, String &lat, String &lon);
void clearAddressData();

#endif
