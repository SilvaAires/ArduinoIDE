#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>

#define relayPin 13

#define SCK_LORA 5
#define MISO_LORA 19
#define MOSI_LORA 27
#define RESET_PIN_LORA 14
#define SS_PIN_LORA 18
#define HIGH_GAIN_LORA 20 /*dBm*/
#define BAND 915E6 // 915 MHz de frequência

/* Definições gerais */
#define DEBUG_SERIAL_BAUDRATE 115200

/* Função: Inicia comunicação com o chip LoRa */
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
  Serial.begin(DEBUG_SERIAL_BAUDRATE);
  while (!Serial); // Aguarda a conexão da porta serial
  while(init_comunicacao_lora() == false); // Tenta iniciar a comunicação LoRa
  pinMode(relayPin, OUTPUT);// Configurar o pino do relé como saída
}

void loop() {
  int packetSize = LoRa.parsePacket();
  int soilMoisture = 0; 

  if (packetSize) {
    // Recebeu um pacote
    Serial.print("Pacote recebido com tamanho: ");
    Serial.println(packetSize);  
    // Leitura dos dados float transmitidos
    if (LoRa.available()) {
      soilMoisture = LoRa.parseInt();
    }
    Serial.print("Umidade do Solo: ");
    Serial.print(soilMoisture);
  }
  if(soilMoisture < 600){
    digitalWrite(relayPin, HIGH);
  }else{
    if(soilMoisture > 600){
      digitalWrite(relayPin, LOW);
    }
  }
  delay(1000); // Espera um segundo antes de checar por mais pacotes
}
