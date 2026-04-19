#include DHT.h
#define DHTPIN 3
#define DHTTYPE DHT11


DHT dht(DHTPIN, DHTTYPE);

void setup() {
 
  Serial.begin(9600);
  Serial.println(F("JS_Automação e Serviços eletricos!"));
  delay(2000);
  dht.begin();
}

void loop() {
  
  float h = dht.readHumidity();
  // Temperature em Celsius (default)
  float t = dht.readTemperature();

  // Verifique se alguma leitura falhou e tenta novamente.
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Falha de leitura do sensor DHT!"));
    return;
  }

  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print(F("Umidade: "));
  Serial.print(h);
  Serial.print("%");
  Serial.print(F(" Temperatura: "));
  Serial.print(t);
  Serial.print(F("°C "));

  Serial.println();

  delay(500);
  Serial.println(millis());

}
