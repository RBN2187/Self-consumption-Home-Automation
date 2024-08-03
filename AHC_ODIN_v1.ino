#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define SSID "*******"
#define PWD "********"

WiFiClient client;

void setup() {

  Serial.begin(9600);
  WiFi.hostname("ESP01");
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
  } 
  
  else {
    Serial.println("Connection failed");
  }
  
  return success;
}

void loop() {

  client.connect("ESP01", 80);
  client.println("Hello server! Are you sleeping?\r");

  String answer = client.readStringUntil('\r');

  Serial.println("from server: " + answer);
  client.flush();

  delay(2000);
  

}
