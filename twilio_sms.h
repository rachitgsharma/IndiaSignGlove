#ifndef TWILIO_SMS_H
#define TWILIO_SMS_H

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <base64.h>

// Twilio credentials - declare only!
extern const char* account_sid;
extern const char* auth_token;

// Phone numbers
extern const char* from_number;
extern const char* to_number;

// Send an SMS via Twilio (assumes WiFi is already connected)
bool sendSMS(String message = "Emergency Alert Triggered!") {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("❌ WiFi not connected!");
    return false;
  }

  WiFiClientSecure client;
  client.setInsecure();  // Use with caution (skip SSL cert validation)

  const char* host = "api.twilio.com";
  const int httpsPort = 443;

  if (!client.connect(host, httpsPort)) {
    Serial.println("❌ Connection to Twilio failed");
    return false;
  }

  String auth = base64::encode(String(account_sid) + ":" + String(auth_token));
  String postData = "To=" + String(to_number) + "&From=" + String(from_number) + "&Body=" + message;

  String request =
    "POST /2010-04-01/Accounts/" + String(account_sid) + "/Messages.json HTTP/1.1\r\n" +
    "Host: " + host + "\r\n" +
    "Authorization: Basic " + auth + "\r\n" +
    "Content-Type: application/x-www-form-urlencoded\r\n" +
    "Content-Length: " + String(postData.length()) + "\r\n" +
    "Connection: close\r\n\r\n" +
    postData;

  client.print(request);

  // Wait for the response
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") break;
  }

  String response = "";
  while (client.available()) {
    String line = client.readStringUntil('\n');
    response += line;
  }

  Serial.println(response);

  if (response.indexOf("\"status\": \"queued\"") != -1 || response.indexOf("\"status\": \"sent\"") != -1) {
    Serial.println("✅ SMS Sent via Twilio!");
    return true;
  } else {
    Serial.println("❌ Twilio SMS send failed.");
    return false;
  }
}

#endif
