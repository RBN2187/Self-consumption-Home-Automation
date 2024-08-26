#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define SSID "********"
#define PWD "*********"

int relayPin = 5;
int analogInPin = A0;

long currTime = 0;
long prevTime = 0;
int period = 1000;

WiFiServer server(80);

IPAddress ip(192, 168, 7, 124);            
IPAddress gateway(192,168,7,1);           
IPAddress subnet(255,255,255,0); 

int measuredValue = 0;
int tempValue = 0;
float ampValue = 0;

int consumption = 9;
bool status = 1;
bool reqStatus = 1;

void setup() {
  Serial.begin(9600);
  pinMode(relayPin, OUTPUT);
  connectWifi(true);
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
    server.begin();
  } 
  
  else {
    Serial.println("Connection failed");
  }
  
  return success;
}

void responseHandler(){
  WiFiClient client = server.accept();

  if (client) {
    if (client.connected()) {
      
      String request = client.readStringUntil('\r');
      Serial.println("From ODIN: " + request);
      client.flush();

      if (request == "data request: current value"){
        client.println(String(consumption) + "\r");
      }

      else if (request == "data request: current status"){
        client.println(String(status) + "\r");
      }

      else if (request == "change status request: 0"){
        reqStatus = 0;
        client.println("successfully changed status\r");
      }

      else if (request == "change status request: 1"){
        reqStatus = 1;
        client.println("successfully changed status\r");
      }

      else{
        client.println("unexpected request format\r");
      }
      
    }
    client.stop();
  }
  
}

void switchStatus(){
  digitalWrite(relayPin, HIGH);
  delay(50);
  digitalWrite(relayPin, LOW);
}

void measureCurrent(){
  for (int i = 0; i < 300; i++){

    tempValue = analogRead(analogInPin);

    if (tempValue > measuredValue){
      measuredValue = tempValue;
    }

    delay(2);
      
    }

    if (measuredValue < 150){
      if (measuredValue < 15){
        ampValue = 0;
      }
      else{
        ampValue = 0.0342*measuredValue;
      }
    }

    else{
      ampValue = 0.000045 * measuredValue * measuredValue - 0.0133 * measuredValue + 5.9828;
    }

    measuredValue = 0;
    tempValue = 0;
    consumption = 0.9 * consumption + 0.1 * ampValue;
  }
}

void loop() {

  currTime = millis();
  if ( currTime - prevTime > period){
    measureCurrent();
    prevTime = currTime;
  }

  responseHandler();

  if (status != reqStatus){
    switchStatus();
    status = reqStatus;
  }
  delay(1);
}
