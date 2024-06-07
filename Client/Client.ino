/**
 * Run a TensorFlow model to predict the IRIS dataset
 * For a complete guide, visit
 * https://eloquentarduino.com/tensorflow-lite-esp32
 */
// replace with your own model
// include BEFORE <eloquent_tinyml.h>!
//#include "irisModel.h"
#include "motionModel2.h"
// include the runtime specific for your board
// either tflm_esp32 or tflm_cortexm
#include <tflm_esp32.h>  // install
// now you can include the eloquent tinyml wrapper
#include <eloquent_tinyml.h>  // install

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

// this is trial-and-error process
// when developing a new model, start with a high value
// (e.g. 10000), then decrease until the model stops
// working as expected
#define TF_NUM_OPS 2

Adafruit_MPU6050 mpu;

Eloquent::TF::Sequential<TF_NUM_OPS, ARENA_SIZE> tf;

const int buttonPin = 32;
const int ledPin1 = 26;
const int ledPin2 = 27;

int samples = 0;
int interval = 2000;
int curr_ledstate = 0;
float s_data[600];
String sensordata_ax;
String sensordata_ay;
String sensordata_az;
String sensordata_gx;
String sensordata_gy;
String sensordata_gz;
String sensordata = "";

void setup() {
  Serial.begin(115200);
  delay(3000);

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


  pinMode(buttonPin, INPUT);
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);

  Serial.println("MPU6050 Found!");
  Serial.println("__TENSORFLOW IRIS__");

  // configure input/output
  // (not mandatory if you generated the .h model
  // using the everywhereml Python package)
  tf.setNumInputs(600);
  tf.setNumOutputs(3);
  // add required ops
  // (not mandatory if you generated the .h model
  // using the everywhereml Python package)
  tf.resolver.AddFullyConnected();
  tf.resolver.AddSoftmax();

  while (!tf.begin(modelData).isOk())
    Serial.println(tf.exception.toString());
}


void loop() {
  // x0, x1, x2 are defined in the irisModel.h file
  // https://github.com/eloquentarduino/EloquentTinyML/tree/main/examples/IrisExample/irisModel.h


  if (digitalRead(buttonPin) == HIGH) {
    while (digitalRead(buttonPin) == HIGH) {
      delay(10);
    }
    sensordata.clear();
    sensordata_ax.clear();
    sensordata_ay.clear();
    sensordata_az.clear();
    sensordata_gx.clear();
    sensordata_gy.clear();
    sensordata_gz.clear();

    samples = 0;

    while (samples <= 99) {      // collect 200 data points
      

      /* Get new sensor events with the readings */
      sensors_event_t a, g, temp;
      mpu.getEvent(&a, &g, &temp);
      digitalWrite(ledPin1, HIGH);

      sensordata_ax += a.acceleration.x;
      s_data[samples] = a.acceleration.x;
      sensordata_ax += "\t";
      sensordata_ay += a.acceleration.y;
      s_data[100+samples] = a.acceleration.y;
      sensordata_ay += "\t";
      sensordata_az += a.acceleration.z;
      s_data[200+samples] = a.acceleration.z;
      sensordata_az += "\t";
      sensordata_gx += g.gyro.x;
      s_data[300+samples] = g.gyro.x;
      sensordata_gx += "\t";
      sensordata_gy += g.gyro.y;
      s_data[400+samples] = g.gyro.y;
      sensordata_gy += "\t";
      sensordata_gz += g.gyro.z;
      s_data[500+samples] = g.gyro.z;
      sensordata_gz += "\t";
      delay(10);  //Set sampling rate to 100Hz
      samples = samples + 1;
    }
    
    sensordata = sensordata_ax + sensordata_ay + sensordata_az + sensordata_gx + sensordata_gy + sensordata_gz;
    digitalWrite(ledPin1, LOW);
    

    int n = 600;
    for (int i = 0; i < n; i++){
      Serial.print(s_data[i]);
      Serial.print("   ");
    }
    Serial.println();
    delay(5); 
  

    if (!tf.predict(s_data).isOk()) {
      Serial.println(tf.exception.toString());
      return;
    }
    Serial.print("Predicting motion based on sensor data: ");
    Serial.println(tf.classification);

    delay(1000);

  }

}