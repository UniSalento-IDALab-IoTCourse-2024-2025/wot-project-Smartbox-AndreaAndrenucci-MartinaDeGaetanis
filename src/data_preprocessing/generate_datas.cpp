#include <Arduino.h>
#include <ArduinoJson.h>

struct Pollutants {
  float c6h6_value;
  float co_value;
  float h2s_value;
  float ipa_value;
  float no2_value;
  float o3_value;
  float pm10_value;
  float pm2dot5_value;
  float so2_value;
};


int calcSubIndex(float conc, float cLow, float cHigh, int iLow, int iHigh) {
  return (int)(((iHigh - iLow) / (cHigh - cLow)) * (conc - cLow) + iLow);
}


int aqiFromPM25(float conc) {
  if (conc <= 12.0) return calcSubIndex(conc, 0, 12.0, 0, 50);
  if (conc <= 35.4) return calcSubIndex(conc, 12.1, 35.4, 51, 100);
  if (conc <= 55.4) return calcSubIndex(conc, 35.5, 55.4, 101, 150);
  if (conc <= 150.4) return calcSubIndex(conc, 55.5, 150.4, 151, 200);
  if (conc <= 250.4) return calcSubIndex(conc, 150.5, 250.4, 201, 300);
  if (conc <= 350.4) return calcSubIndex(conc, 250.5, 350.4, 301, 400);
  if (conc <= 500.4) return calcSubIndex(conc, 350.5, 500.4, 401, 500);
  return 500;
}


int aqiFromPM10(float conc) {
  if (conc <= 54) return calcSubIndex(conc, 0, 54, 0, 50);
  if (conc <= 154) return calcSubIndex(conc, 55, 154, 51, 100);
  if (conc <= 254) return calcSubIndex(conc, 155, 254, 101, 150);
  if (conc <= 354) return calcSubIndex(conc, 255, 354, 151, 200);
  if (conc <= 424) return calcSubIndex(conc, 355, 424, 201, 300);
  if (conc <= 504) return calcSubIndex(conc, 425, 504, 301, 400);
  if (conc <= 604) return calcSubIndex(conc, 505, 604, 401, 500);
  return 500;
}


int calculateAQI(const Pollutants &p) {
  int aqi_pm25 = aqiFromPM25(p.pm2dot5_value);
  int aqi_pm10 = aqiFromPM10(p.pm10_value);

  
  int finalAQI = max(aqi_pm25, aqi_pm10);

  Serial.printf("AQI PM2.5: %d | AQI PM10: %d | AQI Final: %d\n", aqi_pm25, aqi_pm10, finalAQI);

  return finalAQI;
}


Pollutants generatePollutants(
  float c6h6_value = NAN,
  float co_value = NAN,
  float h2s_value = NAN,
  float ipa_value = NAN,
  float no2_value = NAN,
  float o3_value = NAN,
  float pm10_value = NAN,
  float pm2dot5_value = NAN,
  float so2_value = NAN
) {
  const float defaults[] = {
    1.0,   // c6h6
    0.7,   // co
    0.1,   // h2s
    0.05,  // ipa
    20.0,  // no2
    40.0,  // o3
    30.0,  // pm10
    18.0,  // pm2.5
    5.0    // so2
  };

  auto rnd = [](float base) {
    float min = base * 0.8;
    float max = base * 1.2;
    return random(min * 100, max * 100) / 100.0;
  };

  Pollutants p;
  p.c6h6_value    = isnan(c6h6_value)   ? rnd(defaults[0]) : c6h6_value;
  p.co_value      = isnan(co_value)     ? rnd(defaults[1]) : co_value;
  p.h2s_value     = isnan(h2s_value)    ? rnd(defaults[2]) : h2s_value;
  p.ipa_value     = isnan(ipa_value)    ? rnd(defaults[3]) : ipa_value;
  p.no2_value     = isnan(no2_value)    ? rnd(defaults[4]) : no2_value;
  p.o3_value      = isnan(o3_value)     ? rnd(defaults[5]) : o3_value;
  p.pm10_value    = isnan(pm10_value)   ? rnd(defaults[6]) : pm10_value;
  p.pm2dot5_value = isnan(pm2dot5_value)? rnd(defaults[7]) : pm2dot5_value;
  p.so2_value     = isnan(so2_value)    ? rnd(defaults[8]) : so2_value;

  return p;
}


String pollutantsToJson(const Pollutants &p) {
  StaticJsonDocument<256> doc;

  
  
  JsonObject obj = doc.createNestedObject("pollutants");
  
  obj["c6h6_value"] = p.c6h6_value;
  obj["co_value"] = p.co_value;
  obj["h2s_value"] = p.h2s_value;
  obj["ipa_value"] = p.ipa_value;
  obj["no2_value"] = p.no2_value;
  obj["o3_value"] = p.o3_value;
  obj["pm10_value"] = p.pm10_value;
  obj["pm2dot5_value"] = p.pm2dot5_value;
  obj["so2_value"] = p.so2_value;
  doc["quality_index"] = calculateAQI(p);  
  

  String output;
  serializeJson(doc, output);
  return output;
}
