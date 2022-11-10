#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int io[] = {23,25,26,27,33,32}; //ESP32 I/O lines dedicated to the 6 bits of attenuator. LSB is 0.5dB

float att = 0;
float lastatt = 0;

AsyncWebServer server(80);

// REPLACE WITH YOUR NETWORK CREDENTIALS
const char* ssid = "your_SSID";
const char* password = "your_password";

// HTML web page to handle attenuation input field
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>ESP Input Form</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  </head><body>
  <form action="/get">
    Attenuation: <input type="number" name="ATT" value="0" max="31.5" min="0" step="0.5">
    <input type="submit" value="Submit">
  </form><br>
</body></html>)rawliteral";

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void mydisplay(float value){
   // Clear the buffer.
  display.clearDisplay();
  // Display Text
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("RF ATTENUATOR");
  display.setCursor(0,20);
  display.print("ACTUAL ATT: ");
  display.println (value/2);
  display.display();
}

void setup() {
  Serial.begin(115200);
  Serial.println("RF attenuator initializing...");

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }

  // set output pins to the attenuator
  for (int x = 0; x < 6; x++) {
    pinMode(io[x], OUTPUT);
  }
  // set input lines for button
  pinMode(34,INPUT); // "-" button
  pinMode(35,INPUT); // "+" button

  // init Wifi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Failed!");
    return;
  }
  else {
    Serial.println("Wifi Connected!");
  }
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // display initial attenuation
  mydisplay(att);

  // Send web page with input fields to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  // Send a GET request to <ESP_IP>/get?ATT=<inputMessage>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    String inputParam;
    // GET att value on <ESP_IP>/get?ATT=<inputMessage>
    if (request->hasParam("ATT")) {
      inputMessage = request->getParam("ATT")->value();
      inputParam = "ATT";
      att=inputMessage.toFloat()*2;
    }
    else {
      inputMessage = "No value sent";
      inputParam = "none";
    }
    
    request->send(200, "text/html", "HTTP GET request sent to your ESP on input field (" 
                                     + inputParam + ") with value: " + inputMessage +
                                     "<br><a href=\"/\">Return to Home Page</a>");
  });

  server.onNotFound(notFound);
  server.begin();

}

void loop() {
  if (digitalRead(35) == LOW and att<63) {
      att++;
   }
   else if (digitalRead(34) == LOW and att>0) {
       att--;
   }
   if (att != lastatt) { //update display and output only if att is changed
      mydisplay(att);
      Serial.print("actual att:");
      Serial.println(att/2);
      for (int n = 0; n < 6; n++) {
        digitalWrite(io[n],(byte(att) & 1<<n) ? HIGH : LOW); //write parallel out bit by bit
      }
   }
   lastatt = att;
   delay(200);
}