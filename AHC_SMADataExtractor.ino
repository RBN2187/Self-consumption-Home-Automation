/*
  Example code to extract data from a SMA Sunnyboy Inverter. Tested with a SMA Synnyboy 5.0 and ESP-01 ESP8266 module.
  The code can extract variables as given by a "https://<IPAddressInverter>/dyn/getDashValues.json" request in a browser.
  Probably the most useful variable is represented by Key "6100_40263F00", which is the current power (Watts).
  The code stores the variable as an integer, in "val", "tag" or "validValsArray".
*/


#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>


#define InvertorSSID "***************"    // The name of your Sunnyboy wifi network
#define InvertorPWD "****************"    // The password for this network

#define interval 10000                    // Interval between 2 requests to the Invertor server

int httpCode = 0;


#define KEY "6100_40263F00"               // desired value (this is the key for power (W))     

long keyTag = 0;                          // output of this code
long keyVal = 0;                          // output of this code
int keyHigh = 0;                          // output of this code
int keyLow = 0;                           // output of this code



void setup() {
  Serial.begin(9600);
  connectWifi(true);
}



int connectWifi(bool firstConnect) {                  // Credit to SMA-SunnyBoy-Reader library from Pkoerber and beegee-tokyo

  unsigned int retryCount = 0;

  if (!firstConnect) {
    WiFi.reconnect();
    } 

  else {
    WiFi.mode(WIFI_STA);                              // Station mode
    WiFi.begin(InvertorSSID, InvertorPWD);
  }


  while ((WiFi.status() != WL_CONNECTED) && (retryCount < 20)) {
    delay(500);
    Serial.print(".");
    retryCount++;
  }

  int success = WiFi.status();

  if (success == WL_CONNECTED) {
    Serial.println("Connected to Invertor !");

  } 
  
  else {
    Serial.println("Connection failed");
  }
  
  return success;
}



void loop() {

  if (WiFi.status() == WL_CONNECTED) {

    //HTTP REQUEST CODE///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    const char* host = "https://smalogin.net/dyn/getDashValues.json";     // URL of host; request for dashboard values
              
    WiFiClientSecure client;
    HTTPClient http;

    client.setInsecure();                                                 // insecure as we only request data, not sending any commands
    client.connect(host, 443);                                            // 443 is port for communication
    
    http.begin(client, host);
    String payload;
    httpCode = http.GET();
    

    if (httpCode == HTTP_CODE_OK){
      payload = http.getString();                                             // response of the server

      //SCAN JSON CODE///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

      JsonDocument keyFilter;

      keyFilter["result"]["0199-xxxxx42F"][KEY] = true;
      
      JsonDocument keyData;

      DeserializationError error = deserializeJson(keyData, payload, DeserializationOption::Filter(keyFilter));

      if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return;
      }

      if (KEY == "6800_008AA200" || "6800_00832A00"){
        keyHigh = keyData["1"][0]["high"];
        keyLow = keyData["1"][0]["low"];
      }

      if (KEY == "6800_08822000" || "6800_08811F00" || "6800_088A2900" || "6800_08822B00" || "6802_08834500"){
        JsonArray keyValidVals = keyData["1"][0]["validVals"];                                                         // do something with this array
        keyTag = keyData["1"][0]["val"][0]["tag"];
      }

      if (KEY == "6180_08214800" || "6180_08414900" || "6182_08412B00" || "6180_08412800"){
        keyTag = keyData["1"][0]["val"][0]["tag"];
      }
      
      if (KEY == "6800_10821E00"){
        const char* keyValChar = keyData["1"][0]["val"];
      }

      else {
        keyVal = keyData["1"][0]["val"];
      }
    
    }

    else{
      Serial.printf("GET request failed, code: %d\n", httpCode);
    }

    http.end();
      
  }

  if (WiFi.status() != WL_CONNECTED || httpCode == -1){                        // reconnect to Invertor if connection was lost
    connectWifi(false);
  }

  delay(interval);                                                             // wait interval ms for next request                                                                            
}                      

