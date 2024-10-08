#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const char* ssid = "Adelmo";
const char* password = "29072001tr";
const char* serverName = "http://192.168.1.8/Irrigacao_Autonoma/src/Controller/insertDadosSensor.php";  // Endereço do script PHP no XAMPP

#define SCK_LORA 5
#define MISO_LORA 19
#define MOSI_LORA 27
#define RESET_PIN_LORA 14
#define SS_PIN_LORA 18
#define HIGH_GAIN_LORA 20 /*dBm*/
#define BAND 915E6 // 915 MHz de frequência

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

  while (!Serial); // Aguarda a porta serial conectar

  while(init_comunicacao_lora() == false);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando ao WiFi...");
  }
  Serial.println("Conectado ao WiFi");
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    float Temperatura;
    float Umidade;
    int soilMoisture;
    int soilMoisturePercentage;
    int ident;

    LoRa.readBytes(reinterpret_cast<uint8_t*>(&Temperatura), sizeof(Temperatura));
    LoRa.readBytes(reinterpret_cast<uint8_t*>(&Umidade), sizeof(Umidade));
    LoRa.readBytes(reinterpret_cast<uint8_t*>(&soilMoisture), sizeof(soilMoisture));
    LoRa.readBytes(reinterpret_cast<uint8_t*>(&soilMoisturePercentage), sizeof(soilMoisturePercentage));
    LoRa.readBytes(reinterpret_cast<uint8_t*>(&ident), sizeof(ident));

    // Exibe os valores recebidos no monitor serial
    Serial.print("Identificacao"); 
    Serial.println(ident);
    
    Serial.print("Temperatura: ");
    Serial.println(Temperatura);

    Serial.print("Umidade: ");
    Serial.println(Umidade);
    
    Serial.print("Umidade do Solo: ");
    Serial.println(soilMoisture);
    
    Serial.print("Umidade do Solo em Percentual: ");
    Serial.println(soilMoisturePercentage);

    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;

      String httpRequestData = "temperature=" + String(Temperatura) 
                              + "&humidity=" + String(Umidade) 
                              + "&soil_moisture_percentage=" + String(soilMoisturePercentage)
                              + "&soil_moisture=" + String(soilMoisture)
                              + "&ident=" + String(ident);

      // Especificar URL para enviar o dado
      http.begin(serverName);

      // Enviar solicitação HTTP POST
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      int httpResponseCode = http.POST(httpRequestData);

      // Verificar a resposta do servidor
      if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.println(httpResponseCode);  // Código de resposta HTTP
        Serial.println(response);          // Resposta do servidor
      } else {
        Serial.print("Erro ao enviar POST: ");
        Serial.println(httpResponseCode);
      }

      // Finalizar a conexão
      http.end();
    }

  display.clearDisplay();
  display.setCursor(0, OLED_LINE1);
  display.print("Temperatura: ");
  display.println(Temperatura);
  display.setCursor(0, OLED_LINE2);
  display.print("Umidade: ");
  display.println(Umidade);
  display.setCursor(0, OLED_LINE3);
  display.print("Umidade do Solo: ");
  display.println(soilMoisture);
  display.setCursor(0, OLED_LINE4);
  display.println("Solo em Percentage: ");
  display.println(soilMoisturePercentage);
  display.display();
  }

  delay(1000);
}
