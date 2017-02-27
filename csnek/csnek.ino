//#include <WiFi.h>
//#include <WiFiClient.h>
//#include <WiFiServer.h>
//#include <WiFiUdp.h>
//
//#include <EEPROM.h>

#include <ArduinoJson.h>

/*
 *  Simple HTTP get webclient test
 */
 
#include <ESP8266WiFi.h>
#include <string.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>


/* Set these to your desired credentials. */
const char *plants = "http://127.0.0.1:5000/plants";
const char *Dssid = "test";
const char *Dpassword = "test";

const char *local = "";
const char *localpass = "";

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
//
//  connect2home();
//  launchserver();
//  WiFi.begin(local, localpass);
  
  if(EEPROM.read(1) == 1){
    local = ReadEEPROM(2, 32);
    localpass = ReadEEPROM(33, 64);
    connect2home();
    
    }
  else{
    WiFi.mode(WIFI_AP);
    createAP();
  }
}

void createAP(void){
    Serial.print("Starting access point ");
    bool worked = WiFi.softAP("t","t");
//    Serial.println(worked);
//    Serial.println(WiFi.localIP());
    Serial.println(Dssid);
    Serial.println(Dpassword);
    launchserver();
    Serial.println("Here");
}

void launchserver(){
    Serial.println("Server set up.");
    server.on("/",[](){server.send(200,"application/json","{SHIT: IS WORKING}\n");});
    server.on("/configssid", setSSID);
    server.begin();

}


void loop() {
    server.handleClient();  
}

void connect2home(){
  WiFi.begin(local, localpass);
  if(testconnection() ==20){
    Serial.println("Success");
    return;
  }
  
}

int testconnection(){
  Serial.println("Waiting for wifi connection...");
  int t = 0;
  while(t <20){
    if(WiFi.status() == WL_CONNECTED){return(20);}
    delay(500);
    Serial.println(WiFi.status());
    t++;
  }
}

void WritetoEEPROM(String thing, int start){
  for( int i = start; i<thing.length(); ++i){
    EEPROM.write(i, thing[i]);
    Serial.print("Wrote: ");
    Serial.println(thing[i]);
  }
  EEPROM.commit();
}

char * ReadEEPROM(int last, int start){
  char val[last-start];
  for(int i = start; i<last; ++i){
    val[i] = char(EEPROM.read(i));
    Serial.print("Read: ");
    Serial.print(val[i]);
  }
  return val;
}


void setSSID(){
    String data = server.arg("plain");
    StaticJsonBuffer<200> jBuffer;
    JsonObject& jObject = jBuffer.parseObject(data);
    local = jObject["ssid"];
    localpass = jObject["password"];
//    WritetoEEPROM(local,2);
//    WritetoEEPROM(localpass,33);
    Serial.println(local);
    Serial.println(localpass);
    
    server.send(204,"");

}
