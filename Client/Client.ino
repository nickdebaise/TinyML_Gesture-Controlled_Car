/*
Control a robotic car using gestures
Client Code
ECE-328: IoT
Professor Okwori

Author: Nick DeBaise, Austin Gregory
*/
#include "motionModel2.h"
#include <tflm_esp32.h>
#include <eloquent_tinyml.h>
#include <WiFi.h> 
#include <HTTPClient.h>

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WebSocketsClient_Generic.h>

#define ARENA_SIZE 10000

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 32  // OLED display height, in pixels

const char* ssid = "ECE_328-AP"; 
const char* password = "123456%!";

WebSocketsClient webSocket;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
Adafruit_MPU6050 mpu;
Eloquent::TF::Sequential<TF_NUM_OPS, ARENA_SIZE> tf;

const int buttonPin = 32;
const int ledPin1 = 26;

int forwardsPin = 14;
int backwardsPin = 13;
int leftPin = 33;
int rightPin = 12;

int samples = 0;
int gesture_detected;
float s_data[600];


// Set up variables to be used for debouncing the 4 input buttons
// last debounce time, current bounce state, and last bounce state for each of the 4 buttons
unsigned long ldt1 = 0;
unsigned long ldt2 = 0;
unsigned long ldt3 = 0;
unsigned long ldt4 = 0;

int bs1;
int lbs1 = LOW;
int bs2;
int lbs2 = LOW;
int bs3;
int lbs3 = LOW;
int bs4;
int lbs4 = LOW;

unsigned long debounceDelay = 50;

void setup() {
  Serial.begin(115200);
  delay(3000);

  // Initialize MPU
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  delay(100);

  // Initialize display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);

  // Set up GPIO
  pinMode(ledPin1, OUTPUT);
  pinMode(buttonPin, INPUT);
  pinMode(leftPin, INPUT);
  pinMode(rightPin, INPUT);
  pinMode(backwardsPin, INPUT);
  pinMode(forwardsPin, INPUT);

  // Connect to server
  WiFi.begin(ssid, password);
  Serial.println("Connecting"); 
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("."); 
  }

  // Begin WebSocket connection
  webSocket.begin("192.168.4.1", 81, "/");
  Serial.println("WebSocket setup done.");
  webSocket.setReconnectInterval(5000);

  Serial.println("MPU6050 Found!");
  Serial.println("__LOAD TENSORFLOW GESTURE DETECTION MODEL__");

  // Load model
  while (!tf.begin(motionModel2).isOk())
    Serial.println(tf.exception.toString());

}

void loop() {  
  webSocket.loop();

  // Check 4 command button states
  int goLeft = digitalRead(leftPin);
  int goRight = digitalRead(rightPin);
  int goForwards = digitalRead(forwardsPin);
  int goBackwards = digitalRead(backwardsPin);

  if(goLeft != lbs1) {
    ldt1 = millis();
  }

  if(goRight != lbs2) {
    ldt2 = millis();
  }

  if(goForwards != lbs3) {
    ldt3 = millis();
  }

  if(goBackwards != lbs4) {
    ldt4 = millis();
  }

  if ((millis() - ldt1) > debounceDelay) {
    if (goLeft != bs1) {
      bs1 = goLeft;
      if (bs1 == HIGH) {
        Serial.println("Pressed");
        webSocket.sendTXT("3");
      }
    }
  }

   if ((millis() - ldt2) > debounceDelay) {
    if (goRight != bs2) {
      bs2 = goRight;
      if (bs2 == HIGH) {
        Serial.println("Pressed");
        webSocket.sendTXT("2");
      }
    }
  }

   if ((millis() - ldt3) > debounceDelay) {
    if (goForwards != bs3) {
      bs3 = goForwards;
      if (bs3 == HIGH) {
        Serial.println("Pressed");
        webSocket.sendTXT("1");
      }
    }
  }

   if ((millis() - ldt4) > debounceDelay) {
    if (goBackwards != bs4) {
      bs4 = goBackwards;
      if (bs4 == HIGH) {
        Serial.println("Pressed");
        webSocket.sendTXT("4");
      }
    }
  }

  lbs1 = goLeft;
  lbs2 = goRight;
  lbs3 = goForwards;
  lbs4 = goBackwards;

  // Gesture detection
  if (digitalRead(buttonPin) == HIGH) {
    while (digitalRead(buttonPin) == HIGH) {
      delay(10);
    }

    samples = 0;

    // Collect 100 samples
    while (samples <= 99) {

      /* Get new sensor events with the readings */
      sensors_event_t a, g, temp;
      mpu.getEvent(&a, &g, &temp);
      digitalWrite(ledPin1, HIGH);

      s_data[samples] = a.acceleration.x;        //first 100 data in array set to accel_x to match the data collection order
      s_data[100 + samples] = a.acceleration.y;  //next  100 data in array set to accel_y to match the data collection order
      s_data[200 + samples] = a.acceleration.z;  //next  100 data in array set to accel_z to match the data collection order
      s_data[300 + samples] = g.gyro.x;          //next  100 data in array set to gyro_x to match the data collection order
      s_data[400 + samples] = g.gyro.y;          //next  100 data in array set to gyro_y to match the data collection order
      s_data[500 + samples] = g.gyro.z;          //next  100 data in array set to gyro_z to match the data collection order
      delay(10);                                 //Set sampling rate to 100Hz (must match the sampling frequency used in training data collection)
      samples = samples + 1;
    }

    digitalWrite(ledPin1, LOW);

    int n = 600;
    for (int i = 0; i < n; i++) {
      Serial.print(s_data[i]);
      Serial.print("   ");
    }
    Serial.println();
    delay(5);

    // Make the prediction
    if (!tf.predict(s_data).isOk()) {
      Serial.println(tf.exception.toString());
      return;
    }
    Serial.print("Predicting motion based on sensor data: ");
    Serial.println(tf.classification);

    gesture_detected = tf.classification;
    gesture_detected++;

    // Send to Websockets
    webSocket.sendTXT(String(gesture_detected));

    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Gesture: ");
    display.print(gesture_detected );
    display.display();
  }
}
