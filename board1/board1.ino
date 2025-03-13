#include <WiFi.h>
#include <WebServer.h>
#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SS 18
#define RST 23
#define DIO0 26
#define SCK 5
#define MISO 19
#define MOSI 27

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const char* ssid = "LoRa_AP";
const char* password = "12345678";
WebServer server(80);

void handleRoot(){
  server.send(200, "text/html",
    "<html></body>"
    "<h2>Send LoRa Message</h2>"
    "<input type='text' id='msg' placeholder='Enter message'>"
    "<button onclick='sendMessage()'>Send</button>"
    "<script>"
    "function sendMessage(){"
    " var msg = document.getElementById('msg').value;"
    " fetch('/send?msg=' + encodeURIComponent(msg));"
    "}"
    "</script>"
    "</body></html>"
  );
}

void handleSend(){
  if(server.hasArg("msg")){
    String message = server.arg("msg");
    setDisplay(message);
    for(int i=0; i<5; i++){
      LoRa.beginPacket();
      LoRa.print(message);
      LoRa.endPacket();
      delay(1000);
    }
    server.send(200, "text/plain", "Send: " + message);
  }else{
    server.send(400, "text/plain", "No message provided");
  }
  setDisplay(" ");
}

void setDisplay(String message){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print("SSID: ");
  display.print(ssid);
  display.println();
  display.print("Password: ");
  display.print(password);
  display.println();
  display.print("IP: ");
  display.print(WiFi.softAPIP());
  display.println();
  display.print("Message: ");
  display.print(message);
  display.println();
  display.display();
}

void setup() {
  Serial.begin(9600);

  WiFi.softAP(ssid, password);

  server.on("/", handleRoot);
  server.on("/send", handleSend);
  server.begin();

  LoRa.setPins(SS, RST, DIO0);
  SPI.begin(SCK, MISO, MOSI,SS);

  pinMode(18, OUTPUT);
  digitalWrite(18, LOW);
  delay(100);
  digitalWrite(18, HIGH);

  if(!LoRa.begin(915000000)){
    Serial.println("LoRa init failed");
    while(1);
  }
  delay(5000);

  Serial.println("LoRa Transmitter Ready");

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)){
    Serial.println("OLED initialization failed!");
    while(1);
  }

  setDisplay(" ");
}

void loop() {
  server.handleClient();
}
