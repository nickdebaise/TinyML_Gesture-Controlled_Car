#include <WiFi.h> // Include WIFi Library for ESP32
#include <WebServer.h> // Include WebSwever Library for ESP32
#include <ArduinoJson.h> // Include ArduinoJson Library
#include <Adafruit_Sensor.h>
#include <Adafruit_MPU6050.h>
#include <WebSocketsServer.h>  // Include WebSockets Library by Markus Sattler

const int AIN1 = 22;
const int AIN2 = 21;
const int BIN1 = 18;
const int BIN2 = 17;
const int PWMA = 23;
const int PWMB = 16;
const int STBY = 19;

const char* ssid     = "ECE_328-AP";
const char* password = "123456%!";

int interval = 1000; // virtual delay
unsigned long previousMillis = 0; // Tracks the time since last event fired

WebSocketsServer webSocket = WebSocketsServer(81);  //create instance for webSocket server on port"81"

// Function to stop the wheels
void brake(){
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, LOW);
  analogWrite(PWMA, 0);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, LOW);
  analogWrite(PWMB, 0);
  delay(200);
}

// Function to move the car forward
void goForward(){
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  analogWrite(PWMA, 255);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);
  analogWrite(PWMB, 255);
  delay(500);
  brake();
}

// Function to turn the car right
void turnRight(){
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  analogWrite(PWMA, 255);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, LOW);
  analogWrite(PWMB, 0);
  delay(400);
  brake();
}

// Function to turn the car left
void turnLeft(){
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, LOW);
  analogWrite(PWMA, 0);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);
  analogWrite(PWMB, 255);
  delay(400);
  brake();
}

// Function to move the car backwards
void goBackward(){
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);
  analogWrite(PWMA, 255);
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);
  analogWrite(PWMB, 255);
  delay(500);
  brake();
}

// Function to make a 180 degree turn
void Uturn(){
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, LOW);
  analogWrite(PWMA, 0);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);
  analogWrite(PWMB, 255);
  delay(800);
  brake();
}



void setup() {
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(STBY, OUTPUT); 

  digitalWrite(STBY, HIGH);  // put ON the motor drive, set to low to put OFF
  Serial.begin(115200); // Init Serial for Debugging.
 
 
  WiFi.softAP(ssid, password); // set up as access point
  
  delay(2000); // wait for 2s

  // Print the IP address in the serial monitor windows.
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());
  // Initialize a web server on the default IP address. and send the webpage as a response.
  webSocket.begin();  // init the Websocketserver
  webSocket.onEvent(webSocketEvent);  // init the webSocketEvent function when a websocket event occurs
 
}


void loop() {
  webSocket.loop(); // websocket server methode that handles all Client
  unsigned long currentMillis = millis(); // call millis  and Get snapshot of time
}


// This function gets a call when a WebSocket event occurs
void webSocketEvent(byte num, WStype_t type, uint8_t * payload, size_t length) {
  String message = String((char *) payload);

  switch (type) {
    case WStype_DISCONNECTED: // enum that read status this is used for debugging.
      Serial.print("WS Type ");
      Serial.print(type);
      Serial.println(": DISCONNECTED");
      break;
    case WStype_CONNECTED:  // Check if a WebSocket client is connected or not
      Serial.print("WS Type ");
      Serial.print(type);
      Serial.println(": CONNECTED");
      break;
    case WStype_TEXT: // check responce from client
      Serial.println(); // the payload variable stores teh status internally
      Serial.println(message);
      if (message == "1") {
        goForward();             
      }
      if (message == "2") {
        turnRight();           
      }
      if (message == "3") {
        turnLeft();             
      }
      if (message == "4") {
        goBackward();           
      }
      if (message == "5") {
        Uturn();           
      }
      break;
  }
}
