#include <Arduino.h>
#include <WiFi.h>
#include "time.h"
#include "device_registration/wifi_connection/wifi_connection.h"
#include "device_registration/mqtt_registration/mqtt_connection.h"
#include "data_preprocessing/generate_datas.h"

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 3600;

bool alreadyTriggered = false;

void setup() {
    Serial.begin(115200);
    delay(100);

    /* Per configurazione wifi e mqtt */
    connectToWifi();
    serverRoutesConfig();
    setupMQTT();

    /* Per configurazione orario */
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

void loop() {
    server.handleClient();
    mqttLoop();
    Pollutants p = generatePollutants();
    String json = pollutantsToJson(p);
    sendMeasurements(json);

    // struct tm timeinfo;
    // if (!getLocalTime(&timeinfo)) {
    //     Serial.println("Errore nel recuperare l'ora");
    //     delay(1000);
    //     return;
    // }

    // int minuteNow = timeinfo.tm_min;

    // if (minuteNow == 50 && !alreadyTriggered) {
    //     Pollutants p = generatePollutants();
    //     String json = pollutantsToJson(p);
    //     Serial.println(json);
    //     sendMeasurements(json); 

    //     alreadyTriggered = true;
    // }

    // if (minuteNow != 50) {
    //     alreadyTriggered = false;
    // }

    delay(5000);
}
