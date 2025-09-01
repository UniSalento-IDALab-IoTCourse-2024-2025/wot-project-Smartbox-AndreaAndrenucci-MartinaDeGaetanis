#include "mqtt_connection.h"
#include <ArduinoJson.h>
#include "mbedtls/sha256.h"
#include <WiFi.h>
#include <time.h>
#include <HTTPClient.h>
#include "preferences/save_to_flash.h"

const char* mqtt_server = "test.mosquitto.org";
const int mqtt_port = 1883;
const char* topic = "digitair/devices";
const char* heartbeatTopic = "digitair/devices/heartbeat";
const char* measurementsTopic = "digitair/measurements";

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastHeartbeat = 0;
unsigned long heartbeatInterval = 5000;

String macDigest;


void getLocation(JsonDocument& doc) {

    String city, prov, street, lat, lon;
    loadAddressData(city,prov, street, lat, lon);
    
    doc["latitude"] = lat;
    doc["longitude"] = lon;
    doc["municipality"] = city;
    doc["province"] = prov;
    doc["denomination"] = "NaN";
}



void callback(char* topic, byte* message, unsigned int length) {
    Serial.print("Messaggio su topic [");
    Serial.print(topic);
    Serial.print("]: ");
    for (unsigned int i = 0; i < length; i++) {
        Serial.print((char)message[i]);
    }
    Serial.println();
}

String getMacDigest() {
    String mac = WiFi.macAddress();

    unsigned char hash[32];
    mbedtls_sha256_context ctx;
    mbedtls_sha256_init(&ctx);
    mbedtls_sha256_starts_ret(&ctx, 0);
    mbedtls_sha256_update_ret(&ctx, (const unsigned char*)mac.c_str(), mac.length());
    mbedtls_sha256_finish_ret(&ctx, hash);
    mbedtls_sha256_free(&ctx);

    String digest = "";
    for (int i = 0; i < 32; i++) {
        char buf[3];
        sprintf(buf, "%02x", hash[i]);
        digest += buf;
    }
    return digest;
}

String getTimestamp() {
    time_t now;
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        return "1970-01-01T00:00:00Z";
    }

    char buffer[25];
    strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%SZ", &timeinfo);
    return String(buffer);
}

void sendHeartbeat() {
    String ip = WiFi.localIP().toString();

    StaticJsonDocument<256> doc;
    doc["ipv4"] = ip;
    doc["device_id"] = macDigest;
    doc["last_seen"] = getTimestamp();
    doc["status"] = true;


    char buffer[256];
    size_t n = serializeJson(doc, buffer);

    client.publish(heartbeatTopic, buffer, n);

    // Serial.println("Heartbeat inviato:");
    // serializeJsonPretty(doc, Serial);
    // Serial.println();
}

void sendMeasurements(const String& json) {
    StaticJsonDocument<1024> doc;
    StaticJsonDocument<512> measurementsDoc;

    doc["device_id"] = macDigest;
    doc["misuration_date"] = getTimestamp();
    
    getLocation(doc);

    DeserializationError err = deserializeJson(measurementsDoc, json);
    if (err) {
        Serial.print("Errore nel parsing del JSON delle misure: ");
        Serial.println(err.c_str());
        Serial.print("JSON ricevuto: ");
        Serial.println(json);
        return;
    }

    for (JsonPair kv : measurementsDoc.as<JsonObject>()) {
            doc[kv.key()] = kv.value();
        }

    char buffer[1024];
    size_t n = serializeJson(doc, buffer);

    if (client.publish(measurementsTopic, buffer, n)) {
        Serial.println("Measurements inviati:");
        serializeJsonPretty(doc, Serial);
        Serial.println();
    } else {
        Serial.println("Errore nell'invio dei measurements via MQTT");
        Serial.print("Topic: ");
        Serial.println(measurementsTopic);
        Serial.print("Payload: ");
        Serial.write((const uint8_t*)buffer, n);
        Serial.println();
    }
}




void reconnect() {
    while (!client.connected()) {
        Serial.print("Connessione a MQTT...");
        String clientId = "ESP32Client-" + String(random(0xffff), HEX);

        // Prepara LWT JSON
        StaticJsonDocument<256> lwtDoc;
        lwtDoc["device_id"] = macDigest;
        lwtDoc["ipv4"] = WiFi.localIP().toString();
        lwtDoc["status"] = false;  // online = false per LWT
        lwtDoc["last_seen"] = getTimestamp();
        String city, prov, street, lat, lon;
        loadAddressData(city, prov, street, lat, lon);

        char lwtBuffer[512];
        size_t lwtLen = serializeJson(lwtDoc, lwtBuffer);

        
        if (client.connect(clientId.c_str(), NULL, NULL, heartbeatTopic, 0, false, lwtBuffer)) {
            Serial.println("connesso");
            client.subscribe(topic);

            
            StaticJsonDocument<256> initDoc;
            initDoc["device_id"] = macDigest;
            initDoc["ipv4"] = WiFi.localIP().toString();
            initDoc["status"] = true;
            initDoc["last_seen"] = getTimestamp();
            initDoc["latitude"] = lat.toFloat();
            initDoc["longitude"] = lon.toFloat();
            initDoc["municipality"] = city;
            initDoc["province"] = prov;
            initDoc["street"] = street;
            initDoc["denomination"] = "NaN";

            char buffer[512];
            size_t n = serializeJson(initDoc, buffer);
            client.publish(topic, buffer, n);

            Serial.println("Messaggio iniziale inviato:");
            serializeJsonPretty(initDoc, Serial);
            Serial.println();
        } else {
            Serial.print("fallito, rc=");
            Serial.print(client.state());
            Serial.println(" ritento tra 5 secondi");
            delay(5000);
        }
    }
}


void setupMQTT() {
    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(callback);

    macDigest = getMacDigest();

    configTime(0, 0, "pool.ntp.org", "time.nist.gov");
}

void mqttLoop() {
    if (!client.connected()) {
        reconnect();
    }
    client.loop();

    unsigned long now = millis();
    if (now - lastHeartbeat >= heartbeatInterval) {
        sendHeartbeat();
        lastHeartbeat = now;
    }
}