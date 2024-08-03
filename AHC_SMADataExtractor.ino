#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>


#define InvertorSSID "***************"    // The name of your Sunnyboy wifi network
#define InvertorPWD "****************"    // The password for this network

#define interval 10000                    // Interval between 2 requests to the Invertor server

int httpCode = 0;

//SCAN STRING CODE VARIABLES//

int indexVal = 0;
int indexValidVals = 0;
int indexTag = 0;

int validValsArray[5];                    // Output of this code
int tag = 0;                              // Output of this code
int val = 0;                              // Output of this code

#define KEY "6100_40263F00"               // desired value (this is the key for power (W))     

///////////////////////////////



void setup() {
  Serial.begin(9600);
  connectWifi(true);


  ///////////////////////////////////////////////////////////////////////////////////

  if (WiFi.status() == WL_CONNECTED) {

    //HTTP REQUEST CODE//
    
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

      //SCAN STRING CODE//

      int indexKeyBegin = payload.indexOf(KEY);                               // search for the desired KEY code in payload
      String payloadSubString = payload.substring(indexKeyBegin);
      int indexKeyEnd = payloadSubString.indexOf("},");                       // end of the part of the string designated to KEY...
      String keyString = payloadSubString.substring(0, indexKeyEnd);          // ...keyString only contains this part of the payload

      indexVal = keyString.indexOf("val");
      indexValidVals = keyString.indexOf("validVals");
      indexTag = keyString.indexOf("tag");



      if (indexValidVals != -1){                                              // get the data designated to "validVals" if it exists
      
        String keyValidValsSubString = keyString.substring(indexValidVals);
        int indexValidValsValueBegin = keyValidValsSubString.indexOf("[");
        int indexValidValsValueEnd = keyValidValsSubString.indexOf("]");

        String validVals = keyValidValsSubString.substring(indexValidValsValueBegin, indexValidValsValueEnd + 1);

        int arrayIndex = 0;
        int indexMSD = 1;
    
        for(int i = 1; i < validVals.length(); i++){

          if (validVals[i] == ',' || validVals[i] == ']'){
            
            validValsArray[arrayIndex] = (validVals.substring(indexMSD, i)).toInt();          // data from "validVals" is stored in validValsArray
            indexMSD = i + 1;
            arrayIndex++;
          
          }

        }

        Serial.print("validVals: ");

        for (int i = 0 ; i <= (arrayIndex - 1) ; i++){
          Serial.print(validValsArray[i]);

          if (i != (arrayIndex - 1)){
            Serial.print(", ");
          }       

        }

        Serial.println("");
      }



      if (indexTag != -1){                                                       // get the data designated to "tag" if it exists

        String keyTagSubString = keyString.substring(indexTag);
        int indexTagValueBegin = keyTagSubString.indexOf(":");
        int indexTagValueEnd = keyTagSubString.indexOf("}");

        String tagStr = keyTagSubString.substring(indexTagValueBegin + 1, indexTagValueEnd);
        tag = tagStr.toInt();                                                   // data from "tag" is stored in tag

        char buffer[40];
        sprintf(buffer, "Tag: %d", tag);
        Serial.println(buffer);

      }



      else{                                                                     // get the data designated to "val" if it exists

        String keyValSubString = keyString.substring(indexVal);
        int indexValValueBegin = keyValSubString.indexOf(":");
        int indexValValueEnd = keyValSubString.indexOf("}");

        String valStr = keyValSubString.substring(indexValValueBegin + 1, indexValValueEnd);
        val = valStr.toInt();                                                   // data from "val" is stored in val

        char buffer[40];
        sprintf(buffer, "Value: %d", val);
        Serial.println(buffer);
      
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

  //delay(interval);                                                             // wait interval ms for next request
  ESP.deepSleep(10e6);
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

  
                                                                            
}
