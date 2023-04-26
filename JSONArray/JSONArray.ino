#include <ArduinoJson.h>
void setup() {
  Serial.begin(9600);
 
    
}

void loop() {
   const char* input= "{\"sensor\":\"gps\",\"time\":1451824120,\"data\":[48.756080,2.3020038]}";
  StaticJsonDocument<256> doc;
  DeserializationError err= deserializeJson(doc,input) ;
  if(err){
    Serial.print("ERROR:");
    Serial.print(err.c_str());
    return;
    }
   const char* sensor=doc["sensor"];
   long time=doc["time"];
   float lat= doc["data"][0];
   float lon=doc["data"][1];

   Serial.println(sensor);
    Serial.println(time);
     Serial.println(lat, 6);
     Serial.println(lon, 6);
     delay(1000);
}
