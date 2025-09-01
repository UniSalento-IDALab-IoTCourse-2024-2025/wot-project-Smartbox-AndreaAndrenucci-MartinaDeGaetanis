#ifndef DATA_GENERATION
#define DATA_GENERATION

#include <Arduino.h>

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
);


String pollutantsToJson(const Pollutants &p);


#endif