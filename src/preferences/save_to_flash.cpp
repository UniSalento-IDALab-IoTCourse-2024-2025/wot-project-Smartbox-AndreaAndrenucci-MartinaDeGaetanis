#include "save_to_flash.h"

Preferences preferences;

void saveAddressData(const String &city, const String &prov, const String &street, const String &lat, const String &lon) {
    preferences.begin("device_data", false);
    preferences.putString("city", city);
    preferences.putString("prov", prov);
    preferences.putString("street", street);
    preferences.putString("lat", lat);
    preferences.putString("lon", lon);
    preferences.end();

    Serial.println("Dati salvati su flash:");
    Serial.println("  City: " + city);
    Serial.println("  Prov: " + prov);
    Serial.println("  Street: " + street);
    Serial.println("  Lat: " + lat);
    Serial.println("  Lon: " + lon);
}

void loadAddressData(String &city, String &prov, String &street, String &lat, String &lon) {
    preferences.begin("device_data", true);  
    city   = preferences.getString("city", "");
    prov   = preferences.getString("prov", "");
    street = preferences.getString("street", "");
    lat    = preferences.getString("lat", "");
    lon    = preferences.getString("lon", "");
    preferences.end();

    Serial.println("Dati caricati da flash:");
    Serial.println("  City: " + city);
    Serial.println("  Prov: " + prov);
    Serial.println("  Street: " + street);
    Serial.println("  Lat: " + lat);
    Serial.println("  Lon: " + lon);
}

void clearAddressData() {
    preferences.begin("device_data", false);
    preferences.clear();
    preferences.end();

    Serial.println("Tutti i dati eliminati dalla flash.");
}
