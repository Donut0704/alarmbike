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
int acceleratiex = 0;
int acceleratiey = 0;
int acceleratiez = 0;
unsigned long checkvalues = 0;

// Deze bib zorgen ervoor dat de communicatie goed werkt met de accelero
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

// Deze bib zorgt ervoor dat de accelero met de andere chip kan praten
#include <Wire.h>

Adafruit_MPU6050 mpu;

////////////buzzer////////////
int buzzer = 2;

void setup() {
  Serial.begin(9600);
  WiFi.softAP(ssid, password);
  //WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);
  //definieer de commands
  server.on("/", handle_OnConnect);
  server.on("/alarmon", handle_alarmon);
  server.on("/alarmoff", handle_alarmoff);
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("HTTP server started");
  // als de arduino de chip niet vindt, print een error
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  // De gevoeligheid als de chip beweegt
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
}
void loop() {
  server.handleClient();
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  if (alarmon == true) {
    if (millis() - checkvalues >= 100) {
      acceleratiex = a.acceleration.x;
      acceleratiey = a.acceleration.y;
      acceleratiez = a.acceleration.z;
      Serial.println(a.acceleration.x);
      Serial.println(a.acceleration.y);
      Serial.println(a.acceleration.x);
    }
    if (acceleratiex - 0.30 < a.acceleration.x < acceleratiex + 0.30) {
      beweging = true;
      Serial.println("beweging");
    } else if (acceleratiey - 0.30 < a.acceleration.y < acceleratiey + 0.30) {
      beweging = true;
      Serial.println("bewegingy");
    }
  } else {
    beweging = false;
  }
}

void handle_OnConnect() {
  Serial.println("server online");
  server.send(200, "text/html", SendHTML(beweging));
}

void handle_alarmon() {
  Serial.println("alarm on");
  alarmon = true;
  server.send(200, "text/html", SendHTML(beweging));
}

void handle_alarmoff() {
  Serial.println("alarm off");
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
