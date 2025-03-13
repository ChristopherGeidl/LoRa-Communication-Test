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

void setup() {
  Serial.begin(9600);

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

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Testing Display");
  display.display();
}

void loop() {
  int packetSize = LoRa.parsePacket();
  
  if(packetSize){
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Received: ");
    while(LoRa.available()){
      display.print((char)LoRa.read());
    }
    display.display();
    delay(5000);
  }
}
