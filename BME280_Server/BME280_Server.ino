#include <ESP8266WiFi.h>
#include <BME280_MOD-1022.h>
#include <Wire.h>

const int LED_PIN = 5; // Thing's onboard, green LED

/* Network Parametres */
const char *ssid = "FlyFi";  // Wifi netowrk name
const char *password = "allyourbasearebelongtous";  // password

WiFiServer server(80);

void setup() {
  // Do the setup of the server and sensor
  delay(1000);
  
  Wire.begin();
  
  Serial.begin(115200);
  Serial.println();
  
  Serial.print("Configuring access point...");
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password); 
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
 
  server.begin();
  Serial.println("HTTP server started");

  // Setup the LED pin as an output to play with
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // Setup the BME280 Sensor
  BME280.readCompensationParams();
  BME280.writeStandbyTime(tsb_0p5ms);        // tsb = 0.5ms
  BME280.writeFilterCoefficient(fc_16);      // IIR Filter coefficient 16
  BME280.writeOversamplingPressure(os16x);    // Pressure oversample x16
  BME280.writeOversamplingTemperature(os16x);  // Temperature oversample x16
  BME280.writeOversamplingHumidity(os16x);     // Humidity oversample x16
  BME280.writeMode(smNormal);
}

void loop() {
  
   // Check if a client has connected
  WiFiClient client = server.available();
  
  if (!client) {
    // If no connections, go back and check again
    return;
  }

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();
  
  // Handle the request
  int val = -1; // We'll use 'val' to keep track of both the
                // request type (read/set) and value if set.
  if (req.indexOf("/led/0") != -1)
    val = 0; // Will write LED low
  else if (req.indexOf("/led/1") != -1)
    val = 1; // Will write LED high
  else if (req.indexOf("/read") != -1)
    val = -2; // Will read the sensor

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
 
  // If we're reading the sensor, do it and display
  else if (val == -2)
  {
    BME280.readMeasurements();
    s += "<h1>Mothership Drone Wx Data</h1>";
    s += "<h3>Drone-Sounding</h3>";
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
    // User sent a bogus request.
    s += "Invalid Request.<br> Try /led/1, /led/0, or /read.";
  }

  // Close the HTML block
  s += "</html>\n";

  // Send the response to the client
  client.print(s);
  delay(1);
  Serial.println("Client disonnected");
}
