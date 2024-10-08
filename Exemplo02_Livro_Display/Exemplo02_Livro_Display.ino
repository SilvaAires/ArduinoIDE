//Placa Heltec Wifi LoRa 32
/*Includes para header files das bibliotecas do OLED*/
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/*endereço I2C do display*/
#define OLED_ADDR 0x3c

/*distancia, em pixels, de cada linha em relação ao topo do display*/
#define OLED_LINE1 0
#define OLED_LINE2 10
#define OLED_LINE3 20
#define OLED_LINE4 30
#define OLED_LINE5 40

//Configuração da resolução do display (este modulo possui display 128x64)
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

//Objeto do display
Adafruit_SSD1306 display (SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, 16);

//Variaveis
int contador = 0;

//Função Setup
void setup() {
  
  //Inicializa display OLED
  Wire.begin(4, 15);
  if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)){
    Serial.println("Display OLED: falha ao inicializar");
  }else{
    Serial.println("Display OLED: inicialização ok");
    
    //Limpa display e configura tamanho de fonte
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
  }
}

//Programa Principal
void loop() {
  display.clearDisplay();
  display.setCursor(0, OLED_LINE1);
  display.println("Ola!");
  display.setCursor(0, OLED_LINE2);
  display.println("Programa:");
  display.setCursor(0, OLED_LINE3);
  display.println("teste oled");
  display.setCursor(0, OLED_LINE4);
  display.println("Tempo ligado: ");
  display.setCursor(0, OLED_LINE5);
  display.print(contador);
  display.println("S");
  display.display();
  contador++;
  delay(1000);

}
