////////////web server////////////

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

/* Put your SSID & Password */
const char* ssid = "Anti-fietsdiefstal";  // Enter SSID here
const char* password = "Anti-fietsdiefstal";  //Enter Password here

/* Put IP Address details */
/*IPAddress local_ip(192,168,1,1);
  IPAddress gateway(192,168,1,1);
  IPAddress subnet(255,255,255,0);*/

ESP8266WebServer server(80);

////////////MPU 6050 ////////////

bool alarmon = false;
bool beweging = false;
float acceleratiex = 0;
float acceleratiey = 0;
float acceleratiez = 0;
unsigned long checkvalues = 0;
bool heeftbewogen = true;
// Deze bib zorgen ervoor dat de communicatie goed werkt met de accelero
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

// Deze bib zorgt ervoor dat de accelero met de andere chip kan praten
#include <Wire.h>

Adafruit_MPU6050 mpu;

////////////DFRobotDFPlayerMini////////////
#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"
DFRobotDFPlayerMini myDFPlayer;

int indicatorled = 2;
void setup() {
  Serial.begin(9600);
  pinMode(indicatorled, OUTPUT);
  digitalWrite(indicatorled, LOW);
  myDFPlayer.begin(Serial);
  WiFi.softAP(ssid, password);
  //WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);
  //definieer de commands
  server.on("/", handle_OnConnect);
  server.on("/alarmon", handle_alarmon);
  server.on("/alarmoff", handle_alarmoff);
  server.onNotFound(handle_NotFound);

  server.begin();
  //Serial.println("HTTP server started");
  // als de arduino de chip niet vindt, print een error
  if (!mpu.begin()) {
    //Serial.println("Failed to find MPU6050 chip");
    while (1) {
      digitalWrite(indicatorled, HIGH);
      delay(100);
      digitalWrite(indicatorled, LOW);
      delay(100);
    }
  }
  //Serial.println("MPU6050 Found!");

  // De gevoeligheid als de chip beweegt
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  
  myDFPlayer.volume(30);  //Set volume value. From 0 to 30
  myDFPlayer.play(1);  //Play the first mp3
  delay(10);
  myDFPlayer.pause();
}
void loop() {
  server.handleClient();
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  if (alarmon == true) {
    digitalWrite(indicatorled, HIGH);
    if (millis() - checkvalues >= 100) {
      acceleratiex = a.acceleration.x;
      acceleratiey = a.acceleration.y;
      acceleratiez = a.acceleration.z;
      //Serial.println(a.acceleration.x);
      //Serial.println(a.acceleration.y);
      checkvalues = millis();
    }
    if (acceleratiex - 0.50 < a.acceleration.x < acceleratiex + 0.50) {
      beweging = true;
      //Serial.println("beweging");
      //Serial.println(a.acceleration.x);
      //Serial.println(acceleratiex);
    } else if (acceleratiey - 0.50 < a.acceleration.y < acceleratiey + 0.50) {
      beweging = true;
     //Serial.println("bewegingy");
     //Serial.println(acceleratiey);
     //Serial.println(a.acceleration.y);
    }
  } else {
    digitalWrite(indicatorled, LOW);
    beweging = false;
  }
  if (beweging == true && heeftbewogen ==false ) {
    myDFPlayer.start();
  
    heeftbewogen= true;
  }
  if (heeftbewogen ==true && beweging == false){
    myDFPlayer.pause();
    heeftbewogen = false;
  }
}

void handle_OnConnect() {
  //Serial.println("server online");
  server.send(200, "text/html", SendHTML(beweging));
}

void handle_alarmon() {
 // Serial.println("alarm on");
  alarmon = true;
  server.send(200, "text/html", SendHTML(beweging));
}

void handle_alarmoff() {
 // Serial.println("alarm off");
  alarmon = false;
  server.send(200, "text/html", SendHTML(beweging));
}


void handle_NotFound() {
  server.send(404, "text/plain", "Not found");
}

String SendHTML(uint8_t alarmstat ) {
  String ptr = "<html>\n";
  if (alarmstat) {
    ptr = "on";
  } else {
    ptr = "off";
  }
  return ptr;
}
