#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define SSID "********"
#define PWD "*********"

/////DATA ARRAYS/////////////////////////////////////////////////////////////////////////////

String slaveNameArray[] = { "VIDAR_001", "VIDAR_002", "VIDAR_003"};
IPAddress slaveIPArray[] = { {***, ***, ***, ***}, {***, ***, ***, ***}, {***, ***, ***, ***}};
bool slaveIsOnline[] = {1, 1, 1};

bool slaveStatus[] = {0, 0, 0};
int slaveValues[] = {0, 0, 0};

/////////////////////////////////////////////////////////////////////////////////////////////

WiFiClient client;

void setup() {
  Serial.begin(9600);
  connectWifi(true);
  client.setTimeout(2000);
}


int connectWifi(bool firstConnect) {                  // Credit to SMA-SunnyBoy-Reader library from Pkoerber and beegee-tokyo

  unsigned int retryCount = 0;

  if (!firstConnect) {
    WiFi.reconnect();
    } 

  else {
    WiFi.mode(WIFI_STA);                              // Station mode
    WiFi.begin(SSID, PWD);
  }


  while ((WiFi.status() != WL_CONNECTED) && (retryCount < 20)) {
    delay(500);
    Serial.print(".");
    retryCount++;
  }

  int success = WiFi.status();

  if (success == WL_CONNECTED) {
    Serial.print("Connected to WLAN !    IP: ");
    Serial.print(WiFi.localIP());
    Serial.print("   gateway: ");
    Serial.println(WiFi.gatewayIP());
  } 
  
  else {
    Serial.println("Connection failed");
  }
  
  return success;
}


IPAddress findSlaveIP(String slaveName){
  
  for (int i = 0; i < sizeof(slaveNameArray) / sizeof(slaveNameArray[0]); i++){
    
    if (slaveNameArray[i] == slaveName){
      IPAddress slaveIP = slaveIPArray[i];
      return slaveIP;
    }
  }
  return IPAddress (0, 0, 0, 0);
}


void getDataFromAllSlaves(){
  for (int i = 0; i < sizeof(slaveNameArray) / sizeof(slaveNameArray[0]); i++){
    
    bool didReply = 0;
    int tryCount = 0;

    while (didReply == 0 && tryCount < 3){
      if (findSlaveIP(slaveNameArray[i]) != IPAddress (0, 0, 0, 0)){
        client.connect(findSlaveIP(slaveNameArray[i]), 80);
        client.println("data request: current consumption\r");
      }

      String answer = client.readStringUntil('\r');
      
      if (answer.length() == 0){
        answer = "did not reply, try: " + String(tryCount);
      }

      Serial.println(slaveNameArray[i]+ ": " + answer);
      client.flush();
      tryCount++;

      if (tryCount == 3 && didReply == 0){
        slaveIsOnline[i] = 0;
      }
    }
  }
}


void loop() {

  //client.connect(IPAddress (***,***,***,***), 80);
  //client.println("Hello server!\r");

  //String answer = client.readStringUntil('\r');

  //Serial.println("from server: " + answer);
  //client.flush();
  getDataFromAllSlaves();
  delay(2000);

}
