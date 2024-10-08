#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <WiFi.h>
#include <ThingsBoard.h>
#include <Arduino_MQTT_Client.h>

#define relayPin 13

#define SCK_LORA 5
#define MISO_LORA 19
#define MOSI_LORA 27
#define RESET_PIN_LORA 14
#define SS_PIN_LORA 18
#define HIGH_GAIN_LORA 20 /*dBm*/
#define BAND 915E6 // 915 MHz de frequência

#define WIFI_AP "SSID"
#define WIFI_PASS "Senha"

#define TB_SERVER "thingsboard.cloud"
#define TOKEN "TokenDoDispositivoNoSite"

constexpr uint16_t MAX_MESSAGE_SIZE = 128U;

WiFiClient espClient;
Arduino_MQTT_Client mqttClient(espClient);
ThingsBoard tb(mqttClient, MAX_MESSAGE_SIZE);

void connectToWiFi() {//Conexão do WiFi
  Serial.println("Conectando ao WiFi...");
  int attempts = 0;
  
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    WiFi.begin(WIFI_AP, WIFI_PASS, 6);
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nFalha ao conectar ao Wi-Fi.");
  } else {
    Serial.println("\nConectado ao Wi-Fi");
  }
}

void connectToThingsBoard() {
  if (!tb.connected()) {
    Serial.println("Conectando ao servidor ThingsBoard server");
    
    if (!tb.connect(TB_SERVER, TOKEN)) {
      Serial.println("Falha ao conectar ao ThingsBoard");
    } else {
      Serial.println("Conectado ao ThingsBoard");
    }
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
  Serial.begin(115200);
  while (!Serial); // Aguarda a porta serial conectar
  while(init_comunicacao_lora() == false);
  pinMode(relayPin, OUTPUT);
  connectToWiFi();
  connectToThingsBoard();
}

void loop() {
  static unsigned long lastWiFiConnectAttempt = 0;
  static unsigned long lastThingsBoardConnectAttempt = 0;

  // Reconectar ao WiFi em intervalos regulares
  if (millis() - lastWiFiConnectAttempt > 30000) {
    connectToWiFi();
    lastWiFiConnectAttempt = millis();
  }

  // Reconectar ao ThingsBoard em intervalos regulares
  if (millis() - lastThingsBoardConnectAttempt > 60000) {
    connectToThingsBoard();
    lastThingsBoardConnectAttempt = millis();
  }

  // Tentar receber um pacote
  int packetSize = LoRa.parsePacket();

  if (packetSize) {
    // Recebeu um pacote
    Serial.print("Pacote recebido: ");
    Serial.print("Pacote recebido com tamanho: ");
    Serial.println(packetSize);

    // Prepara para ler os floats integer
    int soilMoisture = 0;
    float temperatura = 0.0;
    float umidade = 0.0;

    // Leitura dos dados float e integer transmitidos
    if (LoRa.available()) {
      temperatura = LoRa.parseFloat();
      umidade = LoRa.parseFloat();
      soilMoisture = LoRa.parseInt();
    }

    // Exibe os valores recebidos
    Serial.print("Temperatura: ");
    Serial.print(temperatura);
    Serial.print(" C");

    Serial.print(" Umidade: ");
    Serial.print(umidade);
    Serial.print("%");

    Serial.print(" Umidade do Solo: ");
    Serial.print(soilMoisture);

    // Atualizar o estado do relé com base na umidade do solo
    digitalWrite(relayPin, soilMoisture < 600 ? HIGH : LOW);

    // Verificar conexão ao ThingsBoard e enviar os dados
    if (tb.connected()) {
      sendDataToThingsBoard(temperatura, umidade, soilMoisture);
    }
  }

  // Mantém a conexão com o ThingsBoard
  tb.loop();

  delay(2000);
}

void sendDataToThingsBoard(float temperatura, float umidade, int soilMoisture) {
  char buffer[128];
  snprintf(buffer, sizeof(buffer), "{\"temperatura\":%.2f, \"umidade\":%.2f, \"solo\":%d}", temperatura, umidade, soilMoisture);
  tb.sendTelemetryJson(buffer);
}