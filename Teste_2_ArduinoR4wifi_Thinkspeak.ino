// A ORDEM DAS BIBLIOTECAS É ESSENCIAL
#include "ArduinoGraphics.h"
#include "Arduino_LED_Matrix.h"
#include "WiFiS3.h"
#include "ThingSpeak.h"
#include "DHT.h"

// --- Configurações de Rede e ThingSpeak ---
char ssid[] = "Familia Santos";
char pass[] = "John261091";
unsigned long myChannelNumber = 3019244;
const char * myWriteAPIKey = "MBHZNJO2ID7G2MOT";

// --- Configurações do Hardware ---
#define DHTPIN 2
#define DHTTYPE DHT22
#define LED_VERDE 4
#define LED_AZUL 3
#define LED_VERMELHO 5

DHT dht(DHTPIN, DHTTYPE);
WiFiClient client;
ArduinoLEDMatrix matrix;

void setup() {
  Serial.begin(115200);
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_AZUL, OUTPUT);
  pinMode(LED_VERMELHO, OUTPUT);
  
  dht.begin();
  matrix.begin(); // Inicializa a matriz de LED

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
  // 1. Leitura do sensor
  float Temperatura = dht.readTemperature();
  float Umidade = dht.readHumidity();

  if (isnan(Temperatura) || isnan(Umidade)) {
    Serial.println("Erro na leitura do DHT!");
    return;
  }

  // 2. Alerta de Temperatura nos LEDs externos
  digitalWrite(LED_VERDE, LOW);
  digitalWrite(LED_AZUL, LOW);
  digitalWrite(LED_VERMELHO, LOW);

  if (Temperatura < 33.0) {
    digitalWrite(LED_VERDE, HIGH);
  } else if (Temperatura >= 33.0 && Temperatura < 36.0) {
    digitalWrite(LED_AZUL, HIGH);
  } else {
    digitalWrite(LED_VERMELHO, HIGH);
  }

  // 3. Exibição na Matriz de LED (Texto Scroll)
  String msg = "  T:" + String(Temperatura, 1) + "C U:" + String(Umidade, 0) + "%  ";
  
  matrix.beginDraw();
  matrix.stroke(0xFFFFFFFF);
  matrix.textScrollSpeed(60); // Ajuste a velocidade aqui
  matrix.textFont(Font_5x7);
  matrix.beginText(0, 1, 0xFFFFFF);
  matrix.print(msg);
  matrix.endText(SCROLL_LEFT);
  matrix.endDraw();

  // 4. ENVIO PARA O THINGSPEAK
  ThingSpeak.setField(1, Temperatura);
  ThingSpeak.setField(2, Umidade);
  
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  
  if(x == 200){
    Serial.println("Dados enviados ao ThingSpeak.");
  } else {
    Serial.println("Erro HTTP: " + String(x));
  }

  // 5. Espera 1 minuto para a próxima atualização
  delay(60000); 
}
