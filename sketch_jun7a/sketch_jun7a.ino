#include <Arduino.h>
#include <DHT.h>
#include <LoRa.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Definições do DHT11
#define DHTPIN 13     // Pino digital onde o DHT11 está conectado
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

#define SCK_LORA 5
#define MISO_LORA 19
#define MOSI_LORA 27
#define RESET_PIN_LORA 14
#define SS_PIN_LORA 18
#define HIGH_GAIN_LORA 20 /*dBm*/
#define BAND 915E6 /*915MHz de frequencia*/

/*Definicoes do OLED*/
#define OLED_SDA_PIN 4
#define OLED_SCL_PIN 15
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C
#define OLED_RESET 16

/*Offset de linhas no display OLED*/
#define OLED_LINE1 0
#define OLED_LINE2 10
#define OLED_LINE3 20
#define OLED_LINE4 30
#define OLED_LINE5 40
#define OLED_LINE6 50

/*Variaveis e objetos globais*/
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

/*Local prototypes*/
void display_init(void);
bool init_comunicacao_lora(void);

/*Funcao: inicializa comunicacao com o display OLED*/
/*Parametros: nenhum - Retorno: nenhum*/
void display_init(void){
  if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)){
    Serial.println("[LoRa Receiver] Falha ao inicializar comunicacao com OLED");
  }else{
    Serial.println("[LoRa Receiver] Comunicacao com OLED inicializada com sucesso");

    /*Limpa display e configura tamanho de fonte*/
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
  }
}

// Definições do sensor de umidade do solo
#define SOIL_MOISTURE_PIN 37  // Pino analógico onde o sensor de umidade do solo está conectado

void setup() {
   /*Configuracao da I²C para o display OLED*/
  Wire.begin(OLED_SDA_PIN, OLED_SCL_PIN);
  /*Display init*/
  display_init();
  /*Mensagem na tela para aguardar*/
  display.clearDisplay();
  display.setCursor(0, OLED_LINE1);
  display.print("Aguarde...");
  display.display();

  Serial.begin(115200);

  while(!Serial);

  dht.begin();

  while(init_comunicacao_lora() == false);

}

void loop() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
    
  // Ler dados do sensor de umidade do solo
  int soilMoistureValue = analogRead(SOIL_MOISTURE_PIN);
  float soilMoisturePercentage = map(soilMoistureValue, 0, 4095, 0, 100);

  // Verificar se as leituras são válidas
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Falha ao ler do sensor DHT11!");
    return;
  }

  int ident = 1;

  sendLoRaData(temperature, humidity, soilMoistureValue, soilMoisturePercentage, ident);

  Serial.println("Identificacao"); 
  Serial.println(ident); 

  Serial.println("Temperatura"); 
  Serial.println(temperature); 
  
  Serial.println("Umidade"); 
  Serial.println(humidity); 
  
  Serial.println("Umidade do Solo"); 
  Serial.println(soilMoistureValue); 
  
  Serial.println("Umidade do Solo em Percentage"); 
  Serial.println(soilMoisturePercentage); 

  display.clearDisplay();
  display.setCursor(0, OLED_LINE1);
  display.print("Ident: ");
  display.println(ident);

  display.setCursor(0, OLED_LINE2);
  display.print("Temperatura: ");
  display.println(temperature);

  display.setCursor(0, OLED_LINE3);
  display.print("Umidade: ");
  display.println(humidity);

  display.setCursor(0, OLED_LINE4);
  display.print("Umidade do Solo: ");
  display.println(soilMoistureValue);

  display.setCursor(0, OLED_LINE5);
  display.println("Solo em Percentage: ");
  display.println(soilMoisturePercentage);

  display.display();

  delay(1000);
}

void sendLoRaData(float Temperatura, float Umidade, int soilMoisture, int soilMoisturePercentage, int ident) {
  LoRa.beginPacket();
  LoRa.write(reinterpret_cast<uint8_t*>(&Temperatura), sizeof(Temperatura));
  LoRa.write(reinterpret_cast<uint8_t*>(&Umidade), sizeof(Umidade));
  LoRa.write(reinterpret_cast<uint8_t*>(&soilMoisture), sizeof(soilMoisture));
  LoRa.write(reinterpret_cast<uint8_t*>(&soilMoisturePercentage), sizeof(soilMoisturePercentage));
  LoRa.write(reinterpret_cast<uint8_t*>(&ident), sizeof(ident));
  LoRa.endPacket();
}

bool init_comunicacao_lora(void){
  bool status_init = false;
  Serial.println("[LoRa Sender] Tentando iniciar comunicacao com o radio LoRa...");
  SPI.begin(SCK_LORA, MISO_LORA, MOSI_LORA, SS_PIN_LORA);
  LoRa.setPins(SS_PIN_LORA, RESET_PIN_LORA, LORA_DEFAULT_DIO0_PIN);
  if(!LoRa.begin(BAND)){
    Serial.println("[LoRa Sender] Comunicacao com o radio LoRa falhou. Nova tentatica em 1 segundo...");
    delay(1000);
    status_init = false;
  }else{
    /*Configura o ganho do receptor LoRa para 20dBm, omaior ganho possivel (visando maior alcance possivel)*/
    LoRa.setTxPower(HIGH_GAIN_LORA);
    Serial.println("[LoRa Sender] Comunicacao com o radio LoRa ok...");
    status_init = true;
  }
  return status_init;
}
