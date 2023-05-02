// Libraries ---------------------------------------
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// Wifi Seup ---------------------------------------
const char* ssid = "Emergency-Alarm";
const char* password = "ST12P1-Group1";

// Clinic NodeMCU ip Address -----------------------
const char* serverIP = "192.168.4.1";

// Data for this NodeMCU ---------------------------
const String device_building = "Annex-2";
const String device_Floor = "3rd-Floor";

// Set pins for the buttons ------------------------
const int high_btn = 14;  // pin D5
const int mid_btn = 12;   // Pin D6
const int low_btn = 13;   // Pin D7

// -------------------------------------------------------------------------
// Don't Touch anything below this :)
// Ayaw hilabti and code sugod diri padung ubos para di maguba :)

void setup() {
  // Start Serial for debugging
  Serial.begin(115200);

  // Set pins for the buttons
  pinMode(high_btn, INPUT_PULLUP);
  pinMode(mid_btn, INPUT_PULLUP);
  pinMode(low_btn, INPUT_PULLUP);

  // Enable pull-up resistor
  digitalWrite(high_btn, HIGH);
  digitalWrite(mid_btn, HIGH);
  digitalWrite(low_btn, HIGH);

  // Connect to wifi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected");

  Serial.print("[*] Network information for ");
  Serial.println(ssid);

  Serial.println("[+] BSSID : " + WiFi.BSSIDstr());
  Serial.print("[+] Gateway IP : ");
  Serial.println(WiFi.gatewayIP());
  Serial.println((String) "[+] RSSI : " + WiFi.RSSI() + " dB");
  Serial.print("[+] ESP8266 IP : ");
  Serial.println(WiFi.localIP());
}

void loop() {

  if (digitalRead(high_btn) == LOW) {  // check if High button is pressed
    Serial.println("Button pressed: high");
    send_data("3");
  }

  if (digitalRead(mid_btn) == LOW) {  // check if Medium button is pressed
    Serial.println("Button pressed: medium");
    send_data("2");
  }

  if (digitalRead(low_btn) == LOW) {  // check if Low button is pressed
    Serial.println("Button pressed: low");
    send_data("1");
  }
}

void send_data(String severity) {
  Serial.print("Send Data");
  WiFiClient client;
  HTTPClient http;

  String serverPath = "http://" + String(serverIP) + "/emergency?building=" + device_building + "&Floor=" + device_Floor + "&severity=" + severity;
  Serial.println(serverPath);

  if (!http.begin(client, serverPath)) {
    Serial.println("HTTP failed to begin");
    return;
  }

  int httpResponseCode = http.GET();

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    String payload = http.getString();
    Serial.println(payload);
  } else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  http.end();
}
