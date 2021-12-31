/**
   BasicHTTPSClient.ino

    Created on: 14.10.2018

*/

#include <Arduino.h>
#include <ArduinoJson.h>

#include <WiFi.h>
#include <WiFiMulti.h>

#include <HTTPClient.h>

#include <WiFiClientSecure.h>

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>


// This is GandiStandardSSLCA2.pem, the root Certificate Authority that signed 
// the server certifcate for the demo server https://jigsaw.w3.org in this
// example. This certificate is valid until Sep 11 23:59:59 2024 GMT
/*const char* rootCACertificate = \
"-----BEGIN CERTIFICATE-----\n" \
"-----END CERTIFICATE-----\n";*/

// Not sure if WiFiClientSecure checks the validity date of the certificate. 
// Setting clock just to be sure...
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

void setClock() {
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");

  Serial.print(F("Waiting for NTP time sync: "));
  time_t nowSecs = time(nullptr);
  while (nowSecs < 8 * 3600 * 2) {
    delay(500);
    Serial.print(F("."));
    yield();
    nowSecs = time(nullptr);
  }

  Serial.println();
  struct tm timeinfo;
  gmtime_r(&nowSecs, &timeinfo);
  Serial.print(F("Current time: "));
  Serial.print(asctime(&timeinfo));
}


WiFiMulti WiFiMulti;
//int ledState = LOW;
//const int ledPin0 =  21;
//const int ledPin1 =  12;
//const int ledPin2 =  27;
//const int ledPin3 =  33;
//const int ledPin4 =  32;

void setup() {

  Serial.begin(115200);
  // Serial.setDebugOutput(true);

  Serial.println();
  Serial.println();
  Serial.println();

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("Niagara Research_2.4GHz", "Thenrnetwork24!");

  // wait for WiFi connection
  Serial.print("Waiting for WiFi to connect...");
  while ((WiFiMulti.run() != WL_CONNECTED)) {
    Serial.print(".");
  }
  Serial.println(" connected");

  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(1600);  // This is the maximum PWM frequency
  Wire.setClock(400000);

  setClock();  

  // set the digital pin as output:
  //pinMode(ledPin0, OUTPUT);
  //pinMode(ledPin1, OUTPUT);
  //pinMode(ledPin2, OUTPUT);
  //pinMode(ledPin3, OUTPUT);
  //pinMode(ledPin4, OUTPUT);

  
}

void loop() {
  WiFiClientSecure *client = new WiFiClientSecure;
  struct clientData{
    char name[10];
    bool active;
    };
  const size_t bufferSize = JSON_ARRAY_SIZE(5) + 5*JSON_OBJECT_SIZE(3) + 5*JSON_OBJECT_SIZE(6) + 1000;
  DynamicJsonDocument doc(bufferSize);
 
  if(client) {
    //client -> setCACert(rootCACertificate);

    {
      // Add a scoping block for HTTPClient https to make sure it is destroyed before WiFiClientSecure *client is 
      HTTPClient https;
  
      Serial.print("[HTTPS] begin...\n");
      String server = "https://vendalitemvcwebapiv4.azurewebsites.net/api/candlesapi/AllByChurch/1";
      if (https.begin(*client, server)) {  // HTTPS
        Serial.print("[HTTPS] GET...\n");
        // start connection and send HTTP header
        int httpCode = https.GET();
  
        // httpCode will be negative on error
        if (httpCode > 0) {
          // HTTP header has been send and Server response header has been handled
          Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
  
          // file found at server
          if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
            String payload = https.getString();
            deserializeJson(doc, payload);
            String ledcheck0 = doc[0]["active"];
            String ledcheck1 = doc[1]["active"];
            String ledcheck2 = doc[2]["active"];
            String ledcheck3 = doc[3]["active"];
            String ledcheck4 = doc[4]["active"];
            String ledcheck5 = doc[5]["active"];
            Serial.println(ledcheck5);
            if(ledcheck0 == "true"){
              pwm.setPWM(0, 4096, 0);
            }
            else
            {
              pwm.setPWM(0, 0, 4096);
            }
            //digitalWrite(ledPin0, ledState);

            if(ledcheck1 == "true"){
              pwm.setPWM(1, 4096, 0);
            }
            else
            {
              pwm.setPWM(1, 0, 4096);
            }
            //digitalWrite(ledPin1, ledState);

            if(ledcheck2 == "true"){
              pwm.setPWM(2, 4096, 0);
            }
            else
            {
              pwm.setPWM(2, 0, 4096);
            }
            //digitalWrite(ledPin2, ledState);

            if(ledcheck3 == "true"){
              pwm.setPWM(3, 4096, 0);
            }
            else
            {
              pwm.setPWM(3, 0, 4096);
            }
            //digitalWrite(ledPin3, ledState);

            if(ledcheck4 == "true"){
              pwm.setPWM(4, 4096, 0);
            }
            else
            {
              pwm.setPWM(4, 0, 4096);
            }
            //digitalWrite(ledPin4, ledState);
          }
        } else {
          Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
        }
  
        https.end();
      } else {
        Serial.printf("[HTTPS] Unable to connect\n");
      }

      // End extra scoping block
    }
  
    delete client;
  } else {
    Serial.println("Unable to create client");
  }

  Serial.println();
  Serial.println("Waiting 1s before the next round...");
  Serial.println("This is new software version");
  delay(1000);
}

