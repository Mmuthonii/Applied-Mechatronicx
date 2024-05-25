#include <ESP8266WiFi.h>           // Include the WiFi library for ESP8266
#include <WiFiClient.h>            // Include the WiFi client library
#include <ESP8266WebServer.h>      // Include the web server library
#include <ESP8266mDNS.h>           // Include the mDNS library for network discovery
#include <FS.h>                    // Include the file system library for SPIFFS

// Define WiFi credentials
#ifndef STASSID
#define STASSID "Nakai’s iPhone"   // SSID (WiFi network name)
#define STAPSK "farandbeyond"      // Password
#endif

// Setting WiFi credentials
const char* ssid = STASSID;
const char* password = STAPSK;

ESP8266WebServer server(80);       // Create a web server on port 80

// Define pin for LED
const int led = 16;

// Handle root URL "/"
void handleRoot() {
  digitalWrite(led, 1);            // Turn on LED
  server.send(200);                // Send HTTP 200 response
  digitalWrite(led, 0);            // Turn off LED
}

// Functions to control the yellow LED
void yellowON() {
  digitalWrite(16, HIGH);          // Turn on yellow LED
  server.send(500);                // Send HTTP 500 response
}

void yellowOFF() {
  digitalWrite(16, LOW);           // Turn off yellow LED
  server.send(200);                // Send HTTP 200 response
}

// Functions to control the green LED
void greenON() {
  digitalWrite(4, HIGH);           // Turn on green LED
  server.send(500);                // Send HTTP 500 response
}

void greenOFF() {
  digitalWrite(4, LOW);            // Turn off green LED
  server.send(200);                // Send HTTP 200 response
}

// Functions to control the red LED
void redON() {
  digitalWrite(2, HIGH);           // Turn on red LED
  server.send(500);                // Send HTTP 500 response
}

void redOFF() {
  digitalWrite(2, LOW);            // Turn off red LED
  server.send(200);                // Send HTTP 200 response
}

// Handle request not found (404)
void handleNotFound() {
  digitalWrite(led, 1);            // Turn on LED
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);            // Turn off LED
}

// Load files from SPIFFS
bool loadFromSPIFFS(String path) {
  String dataType = "text/html";   // Default data type is HTML
  Serial.print("Requested page -> ");
  Serial.println(path);
  
  // Check if file exists in SPIFFS
  if (SPIFFS.exists(path)) {
    File dataFile = SPIFFS.open(path, "r");
    if (!dataFile) {
      handleNotFound();
      return false;
    }

    // Stream the file to the client
    if (server.streamFile(dataFile, dataType) != dataFile.size()) {
      Serial.println("Sent less data than expected!");
    } else {
      Serial.println("Page served!");
    }

    dataFile.close();
  } else {
    handleNotFound();
    return false;
  }
  return true;
}

// Serve the HTML page from SPIFFS
void handleRoot2() {
  loadFromSPIFFS("/traffic.html");
}

void setup() {
  // Set GPIO pins for LEDs as outputs
  pinMode(16, OUTPUT);
  digitalWrite(16, LOW);

  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);

  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);

  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Initialize SPIFFS
  Serial.print(F("Inizializing FS..."));
  if (SPIFFS.begin()) {
    Serial.println(F("done."));
  } else {
    Serial.println(F("fail."));
  }

  // Wait for WiFi to connect
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {     // Start mDNS responder
    Serial.println("MDNS responder started");
  }

  // Define routes
  server.on("/html", handleRoot2);
  server.on("/yellowon", yellowON);
  server.on("/yellowoff", yellowOFF);
  server.on("/greenon", greenON);
  server.on("/greenoff", greenOFF);
  server.on("/redon", redON);
  server.on("/redoff", redOFF);
  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });

  // Serve a simple GIF
  server.on("/gif", []() {
    static const uint8_t gif[] PROGMEM = {
      0x47, 0x49, 0x46, 0x38, 0x37, 0x61, 0x10, 0x00, 0x10, 0x00, 0x80, 0x01,
      0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x2c, 0x00, 0x00, 0x00, 0x00,
      0x10, 0x00, 0x10, 0x00, 0x00, 0x02, 0x19, 0x8c, 0x8f, 0xa9, 0xcb, 0x9d,
      0x00, 0x5f, 0x74, 0xb4, 0x56, 0xb0, 0xb0, 0xd2, 0xf2, 0x35, 0x1e, 0x4c,
      0x0c, 0x24, 0x5a, 0xe6, 0x89, 0xa6, 0x4d, 0x01, 0x00, 0x3b
    };
    char gif_colored[sizeof(gif)];
    memcpy_P(gif_colored, gif, sizeof(gif));
    // Set the background to a random set of colors
    gif_colored[16] = millis() % 256;
    gif_colored[17] = millis() % 256;
    gif_colored[18] = millis() % 256;
    server.send(200, "image/gif", gif_colored, sizeof(gif_colored));
  });

  server.onNotFound(handleNotFound);  // Handle 404 not found

  server.begin();                    // Start the web server
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();              // Handle incoming client requests
  MDNS.update();                      // Update mDNS
}
