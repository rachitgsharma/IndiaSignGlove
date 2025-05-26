#ifndef GESTURE_WEBSERVER_H
#define GESTURE_WEBSERVER_H

#include <WebServer.h>

WebServer server(80);                     // HTTP server on port 80
String gujaratiWord = " ";   // Default message

// Function to set the Gujarati word
void SendMessageToApp(String word) {
  gujaratiWord = word;
  Serial.println("📲 Sending to App: " + word);
}

// Handles HTTP GET requests to "/"
void handleRoot() {
  server.send(200, "text/plain", gujaratiWord);
}

// Start only the HTTP server (assumes WiFi is already connected)
void startGestureWebServer() {
  server.on("/", handleRoot);
  server.begin();
  Serial.println("🌐 HTTP server started");
}

// Call this in loop()
void handleWebServerClient() {
  server.handleClient();
}

#endif
