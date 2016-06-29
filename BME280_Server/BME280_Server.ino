#include <ESP8266WiFi.h>
#include <BME280_MOD-1022.h>
#include <Wire.h>
//#include <WiFiClient.h> 
//#include <ESP8266WebServer.h>

const int LED_PIN = 5; // Thing's onboard, green LED

/* Network Parametres */
const char *ssid = "FlyFi";
const char *password = "allyourbasearebelongtous";

//ESP8266WebServer server(80);
WiFiServer server(80);
void setupWiFi() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
}

void setup() {
  delay(1000);
  Wire.begin();
  Serial.begin(115200);
  Serial.println();
  Serial.print("Configuring access point...");

  setupWiFi();

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  
  //server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  BME280.readCompensationParams();
  //BME280.writeOversamplingPressure(os1x); 
  //BME280.writeOversamplingTemperature(os1x);
  //BME280.writeOversamplingHumidity(os1x);
  BME280.writeStandbyTime(tsb_0p5ms);        // tsb = 0.5ms
  BME280.writeFilterCoefficient(fc_16);      // IIR Filter coefficient 16
  BME280.writeOversamplingPressure(os16x);    // pressure x16
  BME280.writeOversamplingTemperature(os16x);  // temperature x2
  BME280.writeOversamplingHumidity(os16x);     // humidity x1
  BME280.writeMode(smNormal);
}

void loop() {
   // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();
  
  // Match the request
  int val = -1; // We'll use 'val' to keep track of both the
                // request type (read/set) and value if set.
  if (req.indexOf("/led/0") != -1)
    val = 0; // Will write LED low
  else if (req.indexOf("/led/1") != -1)
    val = 1; // Will write LED high
  else if (req.indexOf("/read") != -1)
    val = -2; // Will print pin reads
  // Otherwise request will be invalid. We'll say as much in HTML

  // Set GPIO5 according to the request
  if (val >= 0)
    digitalWrite(LED_PIN, val);
  
  client.flush();

  // Prepare the response. Start with the common header:
  String s = "HTTP/1.1 200 OK\r\n";
  s += "Content-Type: text/html\r\n\r\n";
  s += "<!DOCTYPE HTML>\r\n<html>\r\n";
  // If we're setting the LED, print out a message saying we did
  if (val >= 0)
  {
    s += "LED is now ";
    s += (val)?"high":"low";
  }
  else if (val == -2)
  { // If we're reading pins, print out those values:

    
    BME280.readMeasurements();
    s += "<h1>Mothership Drone Wx Data</h1>";
    s += "<h3>Leeman Geophysical</h3>";
    s += "Temperature = ";
    s += String(BME280.getTemperatureMostAccurate());
    s += "<br>"; // Go to the next line.
    s += "Humidity = ";
    s += String(BME280.getHumidityMostAccurate());
    s += "<br>"; // Go to the next line.
    s += "Pressure = ";
    s += String(BME280.getPressureMostAccurate());
    s += "<br>"; // Go to the next line.
  }
  else
  {
    s += "Invalid Request.<br> Try /led/1, /led/0, or /read.";
  }
  s += "</html>\n";

  // Send the response to the client
  client.print(s);
  delay(1);
  Serial.println("Client disonnected");

  // The client will actually be disconnected 
  // when the function returns and 'client' object is detroyed

}
