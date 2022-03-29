/*
 * HTTP Client POST Request
 * Copyright (c) 2018, circuits4you.com
 * All rights reserved.
 * https://circuits4you.com 
 * Connects to WiFi HotSpot. */

#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
//#include <Arduino_JSON.h>

/* Set these to your desired credentials. */
const char *ssid = "Shayon";  //ENTER YOUR WIFI SETTINGS
const char *password = "8583802293";

//Web/Server address to read/write from 
const char *host = "192.168.1.4";   //website or IP address of server

// for the rfid scanner
#include <SPI.h>
#include <MFRC522.h>
constexpr uint8_t RST_PIN = D3;     // Configurable, see typical pin layout above
constexpr uint8_t SS_PIN = D4;     // Configurable, see typical pin layout above
MFRC522 rfid(SS_PIN, RST_PIN);    // Instance of the class
MFRC522::MIFARE_Key key;
String tag;
String rfidData;
String postData;
//=======================================================================
//                    Power on setup
//=======================================================================

void setup() {
  delay(1000);
  Serial.begin(9600);

  // for the rfid
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522
  
  
  WiFi.mode(WIFI_OFF);        //Prevents reconnection issue (taking too long to connect)
  delay(1000);
  WiFi.mode(WIFI_STA);        //This line hides the viewing of ESP as wifi hotspot
  
  WiFi.begin(ssid, password);     //Connect to your WiFi router
  Serial.println("");

  Serial.print("Connecting");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address of ESP: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP
}

//=======================================================================
//                    Main Program Loop
//=======================================================================
void loop() 
{
  HTTPClient http;    //Declare object of class HTTPClient


  //rfid scanning
  if ( ! rfid.PICC_IsNewCardPresent())
    return;
  if (rfid.PICC_ReadCardSerial()) {
    for (byte i = 0; i < 4; i++) {
      tag += rfid.uid.uidByte[i];
    }
  //Serial.println(tag);
  rfidData = tag;
  
  tag = "";
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
  }
  // end rfid
  Serial.println("UID: "+rfidData);
  

  //Post Data
  postData = String("{\"uid\":") + String("\"") + String(rfidData) + String("\"") + String("}");
  //postData = "{\"Testing\": \"Hello\"}";
  
  http.begin("http://192.168.1.14:3000/postit");              //Specify request destination
  // http.addHeader("Content-Type", "application/x-www-form-urlencoded");    //Specify content-type header
  http.addHeader("Content-Type", "application/json");

  int httpCode = http.POST(postData);   //Send the request
  String payload = http.getString();    //Get the response payload

  Serial.println(httpCode);   //Print HTTP return code
  Serial.println(payload);    //Print request response payload

  http.end();  //Close connection
}
