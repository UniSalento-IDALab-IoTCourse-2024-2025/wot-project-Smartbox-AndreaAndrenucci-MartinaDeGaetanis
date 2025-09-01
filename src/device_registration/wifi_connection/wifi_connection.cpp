#include <WiFiManager.h>
#include "wifi_connection.h"
#include <Preferences.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "preferences/save_to_flash.h"

Preferences pref;
WebServer server(80);

WiFiManager wm;


WiFiManagerParameter custom_city("prov", "Provincia", "", 40);
WiFiManagerParameter custom_prov("city", "Città", "", 40);
WiFiManagerParameter custom_street("street", "Via", "", 40);

String savedCity;
String savedProv;
String savedStreet;
String latitude;
String longitude;

void connectToWifi() {
    WiFi.mode(WIFI_STA);

    
    wm.addParameter(&custom_city);
    wm.addParameter(&custom_prov);
    wm.addParameter(&custom_street);

    bool res = wm.autoConnect("AutoConnectAP");
    if (!res) {
        Serial.println("Failed to connect");
        ESP.restart();
    } else {
        Serial.println("Connected.");
    }

    
    savedCity = custom_city.getValue();
    savedProv = custom_prov.getValue();
    savedStreet = custom_street.getValue();

    Serial.println("City: " + savedCity);
    Serial.println("Prov: " + savedProv);
    Serial.println("Street: " + savedStreet);

    
    geocodeAddress(savedCity, savedStreet);

    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    delay(1000);
}

void serverRoutesConfig() {
    // Configura le route del server
    server.on("/wifi/options", HTTP_POST, handleWiFiOpt);

    server.begin();
    Serial.println("HTTP server started");
}

void handleWiFiOpt() {
    if (server.hasArg("plain")) {
        String body = server.arg("plain");

        StaticJsonDocument<200> doc;
        DeserializationError error = deserializeJson(doc, body);

        if (error) {
            server.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
            return;
        }

        const char* message = doc["message"];

        if (message && strcmp(message, "disconnect") == 0) {
            server.send(200, "application/json", "{\"status\":\"WiFi is disconnecting\"}");

            Serial.println("Disconnecting and clearing WiFiManager credentials...");

            wm.resetSettings();
            delay(500);

            WiFi.disconnect(true);
            delay(1000);

            Serial.println("Restarting ESP to reconnect using AutoConnect...");
            ESP.restart();
            delay(5000);
        } else {
            server.send(400, "application/json", "{\"error\":\"Invalid WiFi command\"}");
        }

    } else {
        server.send(400, "application/json", "{\"error\":\"Bad Request\"}");
    }
}



void geocodeAddress(String city, String street) {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi not connected!");
        return;
    }

    HTTPClient http;
    String baseUrl = "https://nominatim.openstreetmap.org/search?";
    String query = "street=" + street + "&city=" + city + "&format=json&limit=1";
    query.replace(" ", "%20");

    String url = baseUrl + query;
    Serial.println("Requesting: " + url);

    http.begin(url);
    http.addHeader("User-Agent", "ESP32Geocoder");
    int httpCode = http.GET();

    if (httpCode > 0) {
        String payload = http.getString();
        Serial.println("Response: " + payload);

        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, payload);

        if (!error && doc.size() > 0) {
            latitude = doc[0]["lat"].as<String>();
            longitude = doc[0]["lon"].as<String>();

            Serial.println("Lat: " + latitude);
            Serial.println("Lon: " + longitude);
            saveAddressData(savedCity, savedProv, savedStreet, latitude, longitude);
        } else {
            Serial.println("Errore parsing JSON o indirizzo non trovato");
        }
    } else {
        Serial.printf("HTTP error: %d\n", httpCode);
    }
    http.end();
}
