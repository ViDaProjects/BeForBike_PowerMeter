#include <Arduino.h>
#include <Wire.h>
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include "HX711.h"
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

// Create an MPU6050 object
MPU6050 mpu;

// Variables to hold raw acceleration data
int16_t ax, ay, az;

const int LOADCELL_DOUT_PIN = 8;
const int LOADCELL_SCK_PIN = 9;

float calibration_factor = 18750; //TODO: ajustar isso
float weight = 0;

HX711 scale;

JsonDocument doc;

SoftwareSerial bluetooth(5, 6); // RX/TX

void setup() {
    Serial.begin(9600);
    Wire.begin();
    //Wire.setClock(5000);
    // Initialize MPU6050
    Serial.println("Initializing MPU6050...");
    mpu.initialize();

    mpu.setFullScaleAccelRange(MPU6050_ACCEL_FS_2);

    // Check connection
    if (mpu.testConnection()) {
        Serial.println("MPU6050 connection successful");
    } else {
        Serial.println("MPU6050 connection failed");
        while (1); // Halt if connection fails
    }

    scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

    Serial.println("Scale test print");

    scale.set_scale(calibration_factor);
    scale.tare();

    Serial.println("Scale set");

    Wire.begin();
    bluetooth.begin(9600);

    Serial.println("Bluetooth set");
}

void loop() {
    // Get acceleration values
    mpu.getAcceleration(&ax, &ay, &az);

    weight = scale.get_units(1);

    doc["a"] = ax;
    doc["w"] = weight;

    Serial.println(weight);

    serializeJson(doc, bluetooth);
    bluetooth.println();
    delay(50);
}