#include <DHT.h>
#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>

/*Defines do projeto*/
/*GPIO do modulo WiFi LoRa 32 que o pino de comunicao do sensor esta ligado*/
#define DHTPIN 13 /*(GPIO 13)*/

#define SCK_LORA 5
#define MISO_LORA 19
#define MOSI_LORA 27
#define RESET_PIN_LORA 14
#define SS_PIN_LORA 18
#define HIGH_GAIN_LORA 20 /*dBm*/
#define BAND 915E6 /*915MHz de frequencia*/

/*A biblioteca serve para os sensores DHT11, 22 e 21*/
#define DHTTYPE DHT22

/*Variaveis e objetos globais*/
/*Objetos para comunicacao com sensor DHT22*/
DHT dht(DHTPIN, DHTTYPE);

/*Funcao: inicia comunicacao com chip LoRa*/
/*Parametros: nenhum*/
/*Retorno: true: comunicacao ok*/
/*false: falha na comunicacao*/

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

void sendLoRaData(float Temperatura, float Umidade) {
  LoRa.beginPacket();
  LoRa.print(Temperatura);
  LoRa.print(Umidade);
  LoRa.endPacket();
  Serial.println(Temperatura);
  Serial.println(Umidade);
}

void setup() {
  Serial.begin(115200);
  while(!Serial);/*Tenta, até obter sucesso, comunicacao com o chip LoRa*/
  dht.begin();
  while(init_comunicacao_lora() == false);
}

void loop() {
  float temperatura = dht.readTemperature();
  float umidade = dht.readHumidity();
  if (isnan(temperatura) || isnan(umidade)) {
    Serial.println("Falha ao ler do sensor DHT!");
  } else {
    sendLoRaData(temperatura, umidade);
  }
  // Aguardar um intervalo antes de ler novamente
  delay(60000); // Intervalo de 60 segundos
}

