


/*
    Simple HTTP get webclient test
*/

#include <ESP8266WiFi.h>
#include <string.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

#define Serial_Num 1;

/* Set these to your desired credentials. */
const char *main = "http://127.0.0.1:5000/";
const char *plants = "http://127.0.0.1:5000/Plants";
const char *ssid = "test";
const char *password = "test";
const int eeAddress = 0;
const int PlantAddress = 512;



bool C2H = false; //True if connected to home network,false if softAP()

int soil;
bool water_level;

struct Local_Net {
  char ssid[64];
  char password[64];
};

struct Plant_Info {
  char Name[64];
  bool WaterLevel = false;
  int Soil = 0;
  char uri_add[32] = "";
  int serial = 0;
};

Plant_Info Plant;
Local_Net Home;

ESP8266WebServer server(80);

/* Just a little test message.  Go to http://192.168.4.1 in a web browser
   connected to this access point to see it.
*/

void setup() {
  delay(1000);
  pinMode(A0,INPUT);
  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.println();
  Serial.println("Startup");
  Serial.println("Starting wifi setup.");

//  EEPROM.begin(1024);
//    for (int i = 0 ; i < 1024 ; i++) {
//        EEPROM.write(i, 0);
//    }
//    EEPROM.end();

  //  struct Local_Net Home;
  EEPROM.begin(1024);
  delay(10);
  EEPROM.get(eeAddress, Home);
  yield();
  EEPROM.get(PlantAddress, Plant);
  yield();
  EEPROM.end();

  Serial.print("Local: ");
  Serial.println(Home.ssid);
  Serial.print("Local Password:");
  Serial.println(Home.password);
  Serial.print("Name: ");
  Serial.println(Plant.Name);


  if (Home.ssid[0] != 0) {
    Serial.println("Connecting to hOme");
    connect2home();
  }
  else {
    C2H = false;
    WiFi.mode(WIFI_AP);
    createAP();

  }
}

void createAP(void) {
  Serial.println("Starting access point ");
  bool worked = WiFi.softAP("New_Plant");
  launchserver();
}

void launchserver() {
  Serial.println("Server set up.");
  server.on("/", []() {
    server.send(200, "application/json", "{SHIT: IS WORKING}\n");
  });
  server.on("/configssid", setSSID);
  server.begin();

}

void loop() {
//  Serial.println(C2H);
  if(C2H == false){
      Serial.println("Handling client");
      server.handleClient();
  }
  else if (C2H == true){    
      delay(5000);
      Plant.Soil = analogRead(A0);
      Plant.WaterLevel = true;
  //      Plant.WaterLevel = digitalRead(1);
      send2Server("UPDATE", "PUT");
  }

}

void send2Server(char *address, char *action) {
  Serial.println("Posting Server");
  Serial.println(".....................................");
//  Serial.println(Plant.Name);
//  Serial.println(Plant.uri_add);
  HTTPClient http;

  if(address =="NEW"){
    Serial.println(plants);
    http.begin(plants);
  }
  else if(address=="UPDATE"){
    Serial.println(Plant.uri_add);
    http.begin(Plant.uri_add);
  }
//
  http.addHeader("Content-Type", "application/json");
//
  StaticJsonBuffer<300> jsonBuffer;
  JsonObject& data = jsonBuffer.createObject();
////
  uint8_t rest = *action;
  data["Name"] = Plant.Name;
  data["WaterLevel"] = Plant.WaterLevel;
  data["Soil"] = Plant.Soil;
  data["Serial_Num"] = Serial_Num;
  char JSONmessageBuffer[300];
  data.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
  Serial.println(JSONmessageBuffer);
//  String data = mkData();
//  int s = sizeof(data.c_str());
//  char data_str[64];
  String payload;
  int httpCode = 0;
  if (action == "POST") {
      httpCode = http.POST(JSONmessageBuffer);
      payload = http.getString();
  }
  else if(action== "PUT"){
      httpCode = http.sendRequest("PUT",JSONmessageBuffer);
      payload = http.getString();
   }
  Serial.print("Response Code: ");
  Serial.println(httpCode);
  Serial.println(payload);
  if (httpCode < 300 && httpCode > 200) {
    Serial.println("POST/PUT Successful");
  }

}

String mkData(){
  String data;
  data+="{";
  data+=Plant.Name;
  data+=",";
  data+=Plant.Soil;
  data+=",";
  data+=Plant.WaterLevel;
  data+=",";
  data+=Serial_Num;
  data+="}";

  return data;
}
void connect2home() {
  WiFi.begin(Home.ssid, Home.password);
  if (testconnection() == 20) {
    Serial.println("Success");
    send2Server("NEW", "POST");
    C2H = true;
  }
}

int testconnection() {
  Serial.println("Waiting for wifi connection...");
  int t = 0;
  while (t < 20) {
    if (WiFi.status() == WL_CONNECTED) {
      return (20);
    }
    delay(500);
    Serial.println(WiFi.status());
    t++;
  }
}

void setSSID() {
  //    Local_Net Home;
  //    Plant_Info Plant
  String data = server.arg("plain");
  StaticJsonBuffer<200> jBuffer;
  JsonObject& jObject = jBuffer.parseObject(data);
  String local_received = jObject["ssid"];
  String localpass_got = jObject["password"];
  String PlantName = jObject["Name"];

  strcpy(Home.ssid, local_received.c_str());
  strcpy(Home.password, localpass_got.c_str());
  strcpy(Plant.Name, PlantName.c_str());
  
  strcat(Plant.uri_add, main);
  strcat(Plant.uri_add, Plant.Name);
  
  EEPROM.begin(1024);
  EEPROM.put(eeAddress, Home);
  EEPROM.end();
  EEPROM.begin(1024);
  EEPROM.put(PlantAddress, Plant);
  EEPROM.end();


  Serial.println(Home.ssid);
  Serial.println(Home.password);
  Serial.println(Plant.Name);
  Serial.println(Plant.uri_add);

  server.send(204, "");

}
