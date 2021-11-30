////////////web server////////////

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

/* Put your SSID & Password */
const char* ssid = "NodeMCU";  // Enter SSID here
const char* password = "12345678";  //Enter Password here

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

// deze bib zorgen ervoor dat de communicatie goed werkt met de accelero
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
  if (alarmon == true){
    if(millis() - checkvalues >= 100){
    acceleratiex = a.acceleration.x;
    acceleratiey = a.acceleration.y;
    acceleratiez = a.acceleration.z;
    Serial.println(a.acceleration.x);
    Serial.println(a.acceleration.y);
    Serial.println(a.acceleration.x);
    }
    if (acceleratiex - 0.30 < a.acceleration.x < acceleratiex +0.30){
      beweging = true;
      Serial.println("beweging");
    }else if (acceleratiey - 0.30 < a.acceleration.y < acceleratiey +0.30){
      beweging = true;
      Serial.println("bewegingy");
    }
  }else{
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


void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}

String SendHTML(uint8_t alarmstat ){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>Alarm control</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr +=".button {display: block;width: 80px;background-color: #1abc9c;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  ptr +=".button-on {background-color: #1abc9c;}\n";
  ptr +=".button-on:active {background-color: #16a085;}\n";
  ptr +=".button-off {background-color: #34495e;}\n";
  ptr +=".button-off:active {background-color: #2c3e50;}\n";
  ptr +="p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  if(alarmstat){
    ptr +="on</h1>\n";
  }else{
    ptr +="off</h1>\n";
  }

  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}
