#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Wire.h>
#include <LoRa.h>

#define SCREEN_WIDTH 128 // Defina a largura da tela do seu display OLED
#define SCREEN_HEIGHT 32 // Defina a altura da tela do seu display OLED

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define SCK_LORA 5
#define MISO_LORA 19
#define MOSI_LORA 27
#define RESET_PIN_LORA 14
#define SS_PIN_LORA 18

#define BAND 915E6  // Defina a banda de frequência LoRa que você está utilizando

void setup() {
  Serial.begin(9600);
  
  SPI.begin(SCK_LORA, MISO_LORA, MOSI_LORA, SS_PIN_LORA);
  LoRa.setPins(SS_PIN_LORA, RESET_PIN_LORA, LORA_DEFAULT_DIO0_PIN);
  
  if (!LoRa.begin(BAND)) {
    Serial.println("Falha ao iniciar LoRa");
  } else {
    Serial.println("LoRa iniciado corretamente");
  }

  Wire.begin(4, 15);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Falha ao iniciar o display OLED"));
    for(;;);
  }
  display.display();
  delay(2000);
  display.clearDisplay();
}

void loop() {
  // Seu código principal aqui
}

