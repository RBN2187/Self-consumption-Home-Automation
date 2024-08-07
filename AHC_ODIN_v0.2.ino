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

enum data {SLAVE_VALUE, SLAVE_STATUS};

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


IPAddress findSlaveIP(String slaveName){                        // Find the IPAdress of a given Slave name in the declared IPAdress Array
  
  for (int i = 0; i < sizeof(slaveNameArray) / sizeof(slaveNameArray[0]); i++){
    
    if (slaveNameArray[i] == slaveName){
      IPAddress slaveIP = slaveIPArray[i];
      return slaveIP;
    }
  }
  return IPAddress (0, 0, 0, 0);
}

int findSlaveNumber(String slaveName){                  // Find the number (position) of a given Slave name in the declared slaveName Array
  
  for (int i = 0; i < sizeof(slaveNameArray) / sizeof(slaveNameArray[0]); i++){
    
    if (slaveNameArray[i] == slaveName){
      int slaveNumber = i;
      return slaveNumber;
    }
  }
  return -1;
}

void getDataFromAllSlaves(data whichData){              // Send a request to all Slaves to reply with their current VALUE or STATUS

  String payload = "";

  switch (whichData){
    case SLAVE_VALUE:
      payload = "data request: current value";
      Serial.println("data request: current value");
      break;

    case SLAVE_STATUS:
      payload = "data request: current status";
      Serial.println("data request: current status");
      break;
  }

  for (int i = 0; i < sizeof(slaveNameArray) / sizeof(slaveNameArray[0]); i++){
    
    bool didReply = 0;
    int tryCount = 0;

    while (didReply == 0 && tryCount < 3){
      if (findSlaveIP(slaveNameArray[i]) != IPAddress (0, 0, 0, 0)){
        client.connect(findSlaveIP(slaveNameArray[i]), 80);
        client.println(payload + "\r");
      }

      String answer = client.readStringUntil('\r');
      
      if (answer.length() == 0){
        answer = "did not reply, try: " + String(tryCount);
      }

      else{
        int responseData = answer.toInt();
        didReply = 1;

        switch (whichData){
          case SLAVE_VALUE:
            slaveValue[i] = responseData;
            break;

          case SLAVE_STATUS:
            slaveStatus[i] = responseData;
            break;
        }  
        
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

void changeSlaveStatus(String slave, int status){       // Send a request to a specified Slave to change STATUS
  bool didReply = 0;
  int tryCount = 0;

  String payload = "change status request: " + String(status);

  while (didReply == 0 && tryCount < 3){
    if (findSlaveIP(slave) != IPAddress (0, 0, 0, 0)){
      client.connect(findSlaveIP(slave), 80);
      client.println(payload + "\r");
    }

    String answer = client.readStringUntil('\r');
      
    if (answer.length() == 0){
      answer = "did not reply, try: " + String(tryCount);
    }

    else if (answer = "successfully changed status"){
      int responseData = answer.toInt();
      didReply = 1;
      if (findSlaveNumber(slave) != -1){
        slaveStatus[findSlaveNumber(slave)] = status;
      }  
    }

    Serial.println(slave + ": " + answer);
    client.flush();
    tryCount++;

    if (tryCount == 3 && didReply == 0){
      if (findSlaveNumber(slave) != -1){
        slaveIsOnline[findSlaveNumber(slave)] = 0;
      }
    }
  }
}

void loop() {
  ///TESTING/////////////////////////////
  getDataFromAllSlaves(SLAVE_VALUE);              
  getDataFromAllSlaves(SLAVE_STATUS);
  delay(2000);
  changeSlaveStatus("VIDAR_001", 0);
  delay(2000);
  changeSlaveStatus("VIDAR_001", 1);
  ///////////////////////////////////////
}
