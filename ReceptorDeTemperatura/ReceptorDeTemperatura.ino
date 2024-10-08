#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>

#define SCK_LORA 5
#define MISO_LORA 19
#define MOSI_LORA 27
#define RESET_PIN_LORA 14
#define SS_PIN_LORA 18
#define HIGH_GAIN_LORA 20 /*dBm*/
#define BAND 915E6 // 915 MHz de frequência

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
}

void loop() {
  // Tentar receber um pacote
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // Recebeu um pacote
    Serial.print("Pacote recebido: ");
    
    // Prepara para ler os floats
    float temperatura = 0.0;
    float umidade = 0.0;

    // Leitura dos dados float transmitidos
    if (LoRa.available()) {
      temperatura = LoRa.parseFloat();
      umidade = LoRa.parseFloat();
    }

    // Exibe os valores recebidos
    Serial.print("Temperatura: ");
    Serial.print(temperatura);
    Serial.print(" C");

    Serial.print("Umidade: ");
    Serial.print(umidade);
    Serial.print("%");
  }
  delay(1000);
}
