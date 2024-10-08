#include <Arduino.h>

// Configuração inicial
void setup() {
  // Inicializa a comunicação serial a 115200 bps
  Serial.begin(115200);
  // Aguarda a inicialização da porta serial
  while (!Serial) {
    ; // Espera pela porta serial
  }
}

// Loop principal
void loop() {
  // Envia a mensagem "Olá Mundo" pela serial
  Serial.println("Olá Mundo");
  // Aguarda 1 segundo antes de enviar novamente
  delay(1000);
}
