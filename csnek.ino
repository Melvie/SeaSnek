#include <ArduinoJson.h>

/*
 *  Simple HTTP get webclient test
 */
 
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <EEPROM.h>


/* Set these to your desired credentials. */
const char *plants = "http://127.0.0.1:5000/plants"
const char *ssid = "test";
const char *password = "test";

ESP8266WebServer server(80);

/* Just a little test message.  Go to http://192.168.4.1 in a web browser
 * connected to this access point to see it.
 */


void setup() {
  delay(1000);
  Serial.begin(115200);
    delay(10);
    Serial.println();
    Serial.println();
    Serial.println("Startup");
    Serial.println("Starting wifi setup.");
    createAP();

    if 

}

void createAP(void){
    Serial.print("Starting access point ");
    Serial.println(WiFi.localIP());
    WiFi.softAP(ssid, password);
    launchserver();
}

void launchserver(void){
    Serial.println('Server set up.');
    server.on("/",[](){server.send(200,"tapplication/json","{WORKING}\n");});
    server.on("/configssid", setSSID);
    server.begin();

}


void loop() {
    server.handleClient();

  
}

void connect2home(){
  WiFi.begin(ssid.c_string, password.c_string);
  if(testconnection() ==20){
    //Connect to home!
    return;
  }
  
}

int testconnection(){
  Serial.println("Waiting for wifi connection...");
  while(t <20){
    if(WiFi.Status() == WL_CONNECTED){return(20);}
    delay(500);
    Serial.println(WiFi.status());
    t++;
  }
}

void WritetoEEPROM(char thing, int start){
  for( int i = start; i<thing.length(); ++i){
    EEPROM.write(i, thing[i]);
    Serial.print("Wrote: ");
    Serial.println(thing[i]);
  }
  EEPROM.commit();
}

void ReadfEEPROM(char thing, int start){
  for(int i = start; i<thing.length(); ++i){
    EEPROM.read(i,thing[i]);
    Serial.print("Read: ");
    Serial.print(thing[i]);
  }
}


void setSSID(){
    String data = server.arg("plain");
    StaticJsonBuffer<200> jBuffer;
    JsonObject& jObject = jBuffer.parseObject(data);
    ssid = jObject["ssid"];
    password = jObject["password"];
    WritetoEEPROM(ssid,2);
    WritetoEEPROM(password,33);
    Serial.println(ssid);
    Serial.println(password);
    
    server.send(204,"");

}