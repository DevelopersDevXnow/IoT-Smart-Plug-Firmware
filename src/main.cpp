
/*
 * Author: Royston Sanctis , Apoorva N
 * Date: 13 November 2022
 * Board ESP32 DEVKIT V1
 * Project: Smart Plug
 * Email: roystonsanctis@gmail.com , apoorvan721@gmail.com
 */

#include <Arduino.h>

#include "ZMPT101B.h"
#include "ACS712.h"

#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

#include <Firebase_ESP_Client.h>

// Provide the token generation process info.
#include <addons/TokenHelper.h>

// Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>

/* 1. Define the WiFi credentials */
#define WIFI_SSID "CIIPDC"
#define WIFI_PASSWORD "123456789"

/* 2. Define the API Key */
#define API_KEY "AIzaSyDHUDp7EkLVc7jcKBZEXrBQ0h1s8YS-toE"

/* 3. Define the RTDB URL */
#define DATABASE_URL "https://iot-smart-plug-dec66-default-rtdb.asia-southeast1.firebasedatabase.app/" //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app

/* 4. Define the user Email and password that alreadey registerd or added in your project */
#define USER_EMAIL "devxnow@gmail.com"
#define USER_PASSWORD "Devxnow@123"

// Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;

unsigned long count = 0;

// We have ZMPT101B sensor connected to A0 pin of arduino
// Replace with your version if necessary
ZMPT101B voltageSensor(32);

// We have 30 amps version sensor connected to A0 pin of arduino
// Replace with your version if necessary
ACS712 currentSensor(ACS712_30A, 31);

void CalibCurrent()
{
  Serial.println("Calibrating... Ensure that no current flows through the sensor at this moment");
  currentSensor.calibrate();
  Serial.println("Current Calibration Done!");
}

void CalibVoltage()
{

  Serial.println("Calibrating... Ensure that no current flows through the sensor at this moment");
  delay(100);

  voltageSensor.calibrate();
  // Calculates sensitivity for 220V
  Serial.print("Sensitivity= ");
  Serial.println(voltageSensor.calculatesSensitivity(220.0), 8);
  Serial.println("Voltage Calibration Done!");
}

void setup()
{
  // put your setup code here, to run once:
  // Baud rate(Display in serial monitor)-How fast data needs to be sent

  Serial.begin(9600);

  // Caliberation Commands Need To Be Run On First Upload.
  CalibCurrent();
  CalibVoltage();

  //.begin is to initialise wifi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  Serial.print("SSID: ");
  Serial.print(WiFi.getHostname());
  Serial.println();

  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the user sign in credentials */
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h

  // Limit the size of response payload to be collected in FirebaseData
  fbdo.setResponseSize(2048);

  Firebase.begin(&config, &auth);

  // Comment or pass false value when WiFi reconnection will control by your code or third party library
  Firebase.reconnectWiFi(true);

  Firebase.setDoubleDigits(5);

  config.timeout.serverResponse = 10 * 1000;
}

void loop()
{
  // put your main code here, to run repeatedly:
  // Code will be excueting every 15 sec
  // if (Firebase.ready() && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0))
  // {
  //   // Current time will be set to previous milli i.e, sendDataPrevMillis
  //   sendDataPrevMillis = millis();

  //   // Print in serial monitor
  //   // Set Integer Value
  //   Serial.printf("Set int... %s\n", Firebase.RTDB.setInt(&fbdo, F("/test/int"), count) ? "ok" : fbdo.errorReason().c_str());
  //   Serial.println("");
  //   // Increment Count
  //   count++;
  // }

  float U = voltageSensor.getVoltageAC();
  float I = currentSensor.getCurrentAC();

  Serial.printf("Voltage=%f", U);

  Serial.println("");
  Serial.printf("Current=%f", I);
}