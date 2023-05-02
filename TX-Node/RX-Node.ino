#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>


const char* ssid = "Emergency-Alarm";
const char* password = "ST12P1-Group1";

// -------------------------------------------------------------------------
// Don't Touch anything below this :)
// Ayaw hilabti and code sugod diri padung ubos para di maguba :)

// LCD with i2c Setup (20x4 LCD)
const int lcd_cols = 20;
const int lcd_rows = 4;
LiquidCrystal_I2C lcd(0x27, lcd_cols, lcd_rows);

// Webserver that recieves http requests
ESP8266WebServer server(80);

// Buzzer Pin D6 = GPIO12
const int buzzer_pin = 12;
const int stop_button_pin = 13;

// default variables
String Floor, severity, api_key, building;

unsigned long start_time = 0;    // initialize start time
unsigned long elapsed_time = 0;  // initialize elapsed time
bool stop_buzzer = false;        // initialize stop_buzzer flag


byte verticalLine[8] = {
  B00100,
  B00100,
  B00100,
  B00100,
  B00100,
  B00100,
  B00100,
  B00100
};
byte char2[8] = {
  B00000,
  B00000,
  B00000,
  B11100,
  B00100,
  B00100,
  B00100,
  B00100
};
byte char1[8] = {
  0b00000,
  0b00000,
  0b00000,
  0b00111,
  0b00100,
  0b00100,
  0b00100,
  0b00100
};
byte char3[8] = {
  0b00100,
  0b00100,
  0b00100,
  0b00111,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};
byte char4[8] = {
  0b00100,
  0b00100,
  0b00100,
  0b11100,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};

void setup() {
  // initialize serial for debugging
  Serial.begin(115200);

  pinMode(buzzer_pin, OUTPUT);             // initialize buzzer pin as output
  pinMode(stop_button_pin, INPUT_PULLUP);  // initialize stop button pin as input with internal pull-up resistor
  digitalWrite(stop_button_pin, HIGH);
  start_time = millis();  // set the start time to the current time

  // start lcd display
  lcd.init();
  lcd.backlight();
  lcd.clear();

  WiFi.softAP(ssid, password);

  server.on("/", handle_root);
  server.on("/emergency", handle_alert);
  server.begin();

  createCustomCharacters();
  // Print the IP address of the SoftAP network for debugging
  lcd.setCursor(0, 2);
  lcd.print("IP: ");
  lcd.println(WiFi.softAPIP());
  tone(buzzer_pin, 100, 50);
  delay(500);
  lcd.clear();
  noTone(buzzer_pin);

  // display idle display
  idle_display();
}

void loop() {
  idle_display();
  server.handleClient();

}

// return 404 on root path
void handle_root() {
  server.send(404, "text/plain", "Made by Kintoyyy :)");
}

void handle_alert() {
  // get data form url post
  Floor = server.arg("Floor");
  building = server.arg("building");
  severity = server.arg("severity");

  server.send(200, "text/plain", "success");
  emergency_display();
}

void idle_display() {
  blink_text("EMERGENCY ALARM", 500, 1);
  lcd.setCursor(5, 2);
  lcd.print("By Group 1");
  printFrame();
}

void emergency_display() {
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("EMERGENCY !!");

  lcd.setCursor(0, 1);
  if (severity == "3") {
    lcd.print("Severity: HIGH ALERT");
  } else if (severity == "2") {
    lcd.print("Severity: MEDIUM ALERT");
  } else if (severity == "1") {
    lcd.print("Severity: LOW ALERT");
  }

  lcd.setCursor(0, 2);
  lcd.print("Building: " + building);

  lcd.setCursor(0, 3);
  lcd.print("Floor: " + Floor);

  Serial.println(severity);
  Serial.println(building);
  Serial.println(Floor);

  elapsed_time = millis() - start_time;                                 // calculate the elapsed time
  if (elapsed_time >= 120000 || digitalRead(stop_button_pin) == LOW) {  // check if two minutes have passed, or if stop_buzzer is true, or if the stop button is pressed
    noTone(buzzer_pin);                                                 // stop the buzzer
    Serial.println("Buzzer stopped!");
    lcd.clear();
    severity = "";
  } else if (digitalRead(stop_button_pin) == HIGH) {  // check if the stop button is not pressed
    for (int i = 0; i < 50; i++) {
      if (severity == "3") {
        tone(buzzer_pin, 100, 500);
        Serial.println("HIGH ALERT");
        delay(300);
      } else if (severity == "2") {
        tone(buzzer_pin, 300, 300);
        Serial.println("MEDIUM ALERT");
        delay(500);
      } else if (severity == "1") {
        tone(buzzer_pin, 500, 100);
        Serial.println("LOW ALERT");
        delay(800);
      }
      if (digitalRead(stop_button_pin) == LOW) {
        noTone(buzzer_pin);  // stop the buzzer
        lcd.clear();
        severity = "";
        break;  // exit the loop if the stop signal is received
      }
    }
  }
}


void blink_text(String text, int interval, int row) {
  static unsigned long lastTime = 0;
  static bool visible = true;
  unsigned long currentTime = millis();
  if (currentTime - lastTime >= interval) {
    lastTime = currentTime;
    visible = !visible;
    lcd.setCursor((20 - text.length()) / 2, row);
    if (visible) {
      lcd.print(text);
    } else {
      for (int i = 0; i < text.length(); i++) {
        lcd.print(" ");
      }
    }
  }
}

// Border
void printFrame() {
  lcd.setCursor(1, 0);
  lcd.print("------------------");
  lcd.setCursor(1, 3);
  lcd.print("------------------");
  lcd.setCursor(0, 1);
  lcd.write(byte(0));
  lcd.setCursor(0, 2);
  lcd.write(byte(0));
  lcd.setCursor(19, 1);
  lcd.write(byte(0));
  lcd.setCursor(19, 2);
  lcd.write(byte(0));
  lcd.setCursor(0, 0);
  lcd.write(byte(1));
  lcd.setCursor(19, 0);
  lcd.write(byte(2));
  lcd.setCursor(0, 3);
  lcd.write(byte(3));
  lcd.setCursor(19, 3);
  lcd.write(byte(4));
}
void createCustomCharacters() {
  lcd.createChar(0, verticalLine);
  lcd.createChar(1, char1);
  lcd.createChar(2, char2);
  lcd.createChar(3, char3);
  lcd.createChar(4, char4);
}