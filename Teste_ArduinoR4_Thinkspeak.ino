#include "WiFiS3.h" // Biblioteca WiFi específica do Uno R4
#include "ThingSpeak.h"
#include "DHT.h"

// --- Configurações de Rede e ThingSpeak ---
char ssid[] = "Familia Santos";
char pass[] = "John261091";
unsigned long myChannelNumber = 3019244;
const char * myWriteAPIKey = "MBHZNJO2ID7G2MOT";

// --- Configurações do Hardware ---
#define DHTPIN 2
#define DHTTYPE DHT11 
#define LED_VERDE 4
#define LED_AZUL 3
#define LED_VERMELHO 5

// Temperatura para ativar o alerta
#define LIMITE_VERDE 30.0 
#define LIMITE_AZUL 33.0 
#define LIMITE_VERMELHO 36.0



DHT dht(DHTPIN, DHTTYPE);
//Arduino LEDMatrix matrix;
WiFiClient client;

void setup() {
  Serial.begin(115200);
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_AZUL, OUTPUT);
  pinMode(LED_VERMELHO, OUTPUT);
  
  dht.begin();

  // Conexão Wi-Fi
  Serial.print("Conectando ao WiFi...");
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConectado!");
  
  ThingSpeak.begin(client);
}

void loop() {
  // Leitura do sensor
  float temp = dht.readTemperature();
  float umid = dht.readHumidity();

  if (isnan(temp) || isnan(umid)) {
    Serial.println("Erro na leitura do DHT!");
    return;
  }

  // 1. Alerta de Temperatura Elevada
  digitalWrite(LED_VERDE, LOW);
  digitalWrite(LED_AZUL, LOW);
  digitalWrite(LED_VERMELHO, LOW);

  if (temp < LIMITE_AZUL) {
    digitalWrite(LED_VERDE, HIGH); // Temperatura normal
  } else if (temp >= LIMITE_AZUL && temp < LIMITE_VERMELHO) {
    digitalWrite(LED_AZUL, HIGH); // Atenção
  } else {
    digitalWrite(LED_VERMELHO, HIGH); // Perigo, muito quente
  }

  // 3. Envio para o ThingSpeak
  ThingSpeak.setField(1, temp);
  ThingSpeak.setField(2, umid);
  
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  
  if(x == 200){
    Serial.println("Canais atualizados no ThingSpeak.");
  } else {
    Serial.println("Erro HTTP: " + String(x));
  }

  // O ThingSpeak aceita atualizações a cada 15-20 segundos
  delay(15000); 
}
