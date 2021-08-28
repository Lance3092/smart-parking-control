#include <Arduino_JSON.h>

#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
//#include <HTTPClient.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <string>
#include <bitset>
//#include <Bridge.h>

//Ultra Sonic Sensor Variabls 
// defines pins numbers
const int trigPin = 4;  
const int echoPin = 5;  
unsigned long previousMillis = 0;
unsigned int checkInterval = 1000;
int ultrasonicdistance = 0;
bool currentState ;
int carHeight = 40;
bool previousOccupied = false;
// defines variables
long duration;
int distance;

const int pins[] = {14,12,13}; //pins D5,D6,D7 on Node MCU board;

#define HTTP_REST_PORT 80
#define WIFI_RETRY_DELAY 500
//Network Variables 
const char* ssid = "devpinetwork";
const char* password = "raspberry";
const String bayID = "0001";
const String boardHostName = "baycontroller" + bayID;
const String hostController = "http://devpi.local:5000";
WiFiClient wifiClient;
bool isOccupied = false; 
int ledColor = 0 ;
ESP8266WebServer server(HTTP_REST_PORT);
void getHelloWord() {
    server.send(200, "text/json", "{\"name\": \"Hello world\"}");
}

void setUpLEDStatus() {
  HTTPClient http;
  String serverName = hostController + "/setup/" + bayID ;
  http.begin(wifiClient , serverName);
  int httpResponseCode = http.GET();
  String payload = "{}"; 
  if (httpResponseCode>0) {
    payload = http.getString();
    Serial.println(payload);
    DynamicJsonDocument doc(512);
    DeserializationError error = deserializeJson(doc, payload);
    if(error){
      Serial.println("Error parsing JSON");
    }
    JsonObject jObj = doc.as<JsonObject>();
    if(jObj.containsKey("bayStatus"))
      {
        int val = jObj["bayStatus"].as<int>();
        writeToLED(val);
      }
  }
  
  http.end();

 
}

void postStatusLED(){
  String postBody = server.arg("plain");
  Serial.println(postBody);
  server.send(200,"text/json","{\"status\" : \"OK\"}");
  DynamicJsonDocument doc(512);
  DeserializationError error = deserializeJson(doc, postBody);
  if(error)
  {
    Serial.print(F("Error parsing JSON "));
    Serial.println(error.c_str());
    String msg = error.c_str();
    server.send(400, F("text/html"),"Error in parsin json body! <br>" + msg);
  }
  else{
    
    JsonObject postObj = doc.as<JsonObject>();
    Serial.print(F("HTTP Method: "));
    Serial.println(server.method());
    if(server.method() == HTTP_POST)
    {
      if(postObj.containsKey("bayStatus"))
      {
        int val = postObj["bayStatus"].as<int>();
        writeToLED(val);
        server.send(200,F("text/html"),"LED Set");
      }
      
    }
    server.send(400, F("text/html"),"Error in parsin json body! <br>" );
  }
  
}

void setupServer() {
    server.on("/", HTTP_GET, []() {
        server.send(200, "text/html",
            "Welcome to the ESP8266 REST Web Server");
    });
   //http_rest_server.on("/leds", HTTP_GET, writeToLED);
    server.on(F("/helloWorld"), HTTP_GET, getHelloWord);
    server.on(F("/setStatus"),HTTP_POST,postStatusLED);
   //http_rest_server.on("/leds", HTTP_PUT, post_put_leds);
   server.begin();
}
void setupWifi()
{
  WiFi.hostname("test01");
  WiFi.persistent( true );
  WiFi.begin(ssid, password);             // Connect to the network
  Serial.print("Connecting to ");
  Serial.print(ssid); Serial.println(" ...");
  int i = 0;
  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    delay(1000);
    Serial.print(++i); Serial.print(' ');
    Serial.println("Connection Status : "+ String(WiFi.status()));
  }
  
  Serial.println('\n');
  Serial.println("Connection established!");  
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());         // Send the IP address of the ESP8266 to the computer
}
void setupPins()
{
  pinMode(pins[0],OUTPUT);
  pinMode(pins[1],OUTPUT);
  pinMode(pins[2],OUTPUT);

  // Ultra Sonic 
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  
  //LEDS flash all colors to show intialisation 
  for(int i = 0;i <= 7;i++){
    writeToLED(i);
    delay(500);
    }

} 
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  setupWifi();
  setupServer();
  setupPins();
  setUpLEDStatus() ;
}

void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis >= checkInterval) 
  {
    previousMillis = currentMillis;
    CheckValue();  
  }

}
void updateParkingState(bool newState) {
  HTTPClient http;
  String serverName = hostController + "/updateParkingState/" + bayID ;
  http.begin(wifiClient , serverName);
  StaticJsonDocument<512> docSend;
  JsonObject object = docSend.to<JsonObject>();
  http.addHeader("Content-Type", "application/json");
  object["bayState"] = newState;
  String jsonObj;
  serializeJson(docSend,jsonObj);
  Serial.println("Sending Message : ");
  Serial.println(jsonObj);
  Serial.println("To : ");
  Serial.println(serverName);
  int httpResponseCode = http.POST(jsonObj);
  String payload = "{}"; 
  if (httpResponseCode>0) {
    payload = http.getString();
    Serial.println(payload);
    DynamicJsonDocument doc(512);
    DeserializationError error = deserializeJson(doc, payload);
    if(error){
      Serial.println("Error parsing JSON");
      }
    JsonObject jObj = doc.as<JsonObject>();
    if(jObj.containsKey("bayStatus"))
      {
        int val = jObj["bayStatus"].as<int>();
        writeToLED(val);
      }
  }
  
  http.end();

 
}
void CheckValue() {
  digitalWrite(echoPin, LOW);
  delay(10);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  float duration = pulseIn(echoPin, HIGH);
  int distance = duration * 0.034 / 2;

  bool currentOccupied = (distance >carHeight);
  if(currentOccupied == previousOccupied)
  {
    
    if(currentOccupied != currentState) 
    {
      updateParkingState(currentOccupied);
    }
    currentState = currentOccupied; 
  }
  previousOccupied = currentOccupied;
}
void writeToLED (int color) // numbers above 7 ignored 
{
  if(color > 7)
  {
    return;
  }
  std::string strCol = std::bitset<3>(color).to_string();
  Serial.println("writing value to LED : " );
  Serial.print(color );
  for(int i = 0 ; i< 3 ;i++)
  {
    if(strCol[i] == '1')
    {
      digitalWrite(pins[i],LOW);
    }
    else 
    {
      digitalWrite(pins[i],HIGH);
    } 
  }
}