#include <TinyGPS++.h>
#include <SoftwareSerial.h>

TinyGPSPlus gps;
SoftwareSerial ss(4, 3); // RX e TX, ajuste conforme sua ligação

void setup() {
  Serial.begin(9600);
  ss.begin(9600); // Velocidade padrão do GPS Neo-6M
  Serial.println("Iniciando GPS...");
}

void loop() {
  // Verifica se há dados disponíveis no GPS
  while (ss.available() > 0) {
    gps.encode(ss.read());
  }

  // Se houver uma localização válida, exibe na Serial
  if (gps.location.isUpdated()) {
    Serial.print("Latitude: ");
    Serial.println(gps.location.lat(), 6);
    Serial.print("Longitude: ");
    Serial.println(gps.location.lng(), 6);
    Serial.print("Altitude (m): ");
    Serial.println(gps.altitude.meters());
    Serial.print("Satélites: ");
    Serial.println(gps.satellites.value());
    Serial.println();
  }
}
