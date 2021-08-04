#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <string>
#include <bitset>
#define HTTP_REST_PORT 80
#define WIFI_RETRY_DELAY 500
//Network Variables 
const char* ssid = "devpinetwork";
const char* password = "raspberry";
const char* boardHostName = "test01";

ESP8266WebServer server(HTTP_REST_PORT);
void getHelloWord() {
    server.send(200, "text/json", "{\"name\": \"Hello world\"}");
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
      if(postObj.containsKey("currentStatus"))
      {
        int val = postObj["currentStatus"].as<int>();
        writeToLED(val);
        server.send(200,F("text/html"),"LED Set");
      }
      
    }
    server.send(400, F("text/html"),"Error in parsin json body! <br>" );
  }
  
}
const int pins[] = {14,12,13}; //pins D5,D6,D7 on Node MCU board;
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
}

void loop() {
  // put your main code here, to run repeatedly:
    server.handleClient();

}

void writeToLED (int color) // numbers above 7 ignored 
{
  if(color > 7)
  {
    return;
  }
  std::string strCol = std::bitset<3>(color).to_string();
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
