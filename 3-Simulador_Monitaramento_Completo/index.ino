#include <HTTPClient.h>
#include <WiFi.h>
#include <WiFiClient.h>


const char *WIFI_SSID = "SEU_WIFI_SSID";
const char *WIFI_SENHA = "SUA_WIFI_SENHA";
const char *ENDPOINT_URL =
    "SEU_ENDPOINT_URL"; // Ex: "http://seuservidor.com/api/dados"

const int PINO_TRIG_HCSR04 = 5;       // Exemplo: GPIO5
const int PINO_ECHO_HCSR04 = 4;       // Exemplo: GPIO4
const int PINO_BUZZER_LOCAL = 14;     // Exemplo: GPIO14
const int PINO_LED_ALERTA_LOCAL = 12; // Exemplo: GPIO12

const float VELOCIDADE_SOM_CM_US = 0.0343;
const float LIMITE_DISTANCIA_RISCO_CM = 50.0;

unsigned long tempoUltimoEnvio = 0;
const unsigned long intervaloEnvio = 300000; // 5 minutos
// const unsigned long intervaloEnvio = 30000; // Para testes: 30 segundos

// ================================================
// Sistema Completo de Monitoramento Ambiental
// Este é o terceiro estágio do projeto, que implementa o sistema
// completo de monitoramento com preparação para comunicação
// 
// Objetivo: Sistema completo com todos os sensores e estrutura
// preparada para futura implementação de comunicação
// ================================================

// Definição dos pinos dos sensores analógicos
const int PINO_NIVEL_AGUA = A0;    // Sensor de nível de água
const int PINO_TEMPERATURA = A1;   // Sensor de temperatura
const int PINO_PRESSAO_ATM = A2;   // Sensor de pressão atmosférica

// Definição dos pinos de alerta
const int PINO_BUZZER = 4;         // Buzzer para alerta sonoro
const int PINO_LED_ALERTA = 2;     // LED para alerta visual

// Limites críticos para ativação dos alertas
const float LIMITE_CRITICO_NIVEL_AGUA = 75.0;     // Nível máximo de água em cm
const float LIMITE_CRITICO_TEMPERATURA = 35.0;    // Temperatura máxima em °C
const float LIMITE_CRITICO_PRESSAO_ATM = 980.0;   // Pressão mínima em hPa

// Faixas de valores para mapeamento dos sensores
const float NIVEL_MIN_MAP = 0.0;      // Valor mínimo para nível de água (cm)
const float NIVEL_MAX_MAP = 100.0;    // Valor máximo para nível de água (cm)
const float TEMP_MIN_MAP = -10.0;     // Temperatura mínima (°C)
const float TEMP_MAX_MAP = 50.0;      // Temperatura máxima (°C)
const float PRESSAO_MIN_MAP = 950.0;  // Pressão atmosférica mínima (hPa)
const float PRESSAO_MAX_MAP = 1050.0; // Pressão atmosférica máxima (hPa)

// ================================================
// Função para preparação dos dados
// Estrutura preparada para futura implementação de comunicação
// ================================================
void prepararDados(float nivel, float temp, float pressao, bool alerta) {
  // Montagem do pacote de dados (estrutura preparada para JSON)
  String pacote = "{";
  pacote += "\"nivel\":" + String(nivel, 1) + ",";
  pacote += "\"temperatura\":" + String(temp, 1) + ",";
  pacote += "\"pressao\":" + String(pressao, 1) + ",";
  pacote += "\"alerta\":" + (alerta ? "true" : "false");
  pacote += "}";

  // Exibição dos dados no monitor serial
  Serial.println("[Sistema] Dados preparados: " + pacote);

  // Exibição de mensagem específica para alertas
  if (alerta) {
    Serial.println("[Sistema] ALERTA CRITICO detectado!");
  } else {
    Serial.println("[Sistema] Dados registrados com sucesso.");
  }
}

void conectarWiFi() {
  Serial.print("Conectando a ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_SENHA);
  int tentativas = 0;
  while (WiFi.status() != WL_CONNECTED && tentativas < 20) {
    delay(500);
    Serial.print(".");
    tentativas++;
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi conectado!");
    Serial.print("Endereco IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nFalha ao conectar no WiFi.");
  }
}

void enviarDadosWiFi(float nivelCm, bool alertaAtivo) {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;

    String pacoteJson = "{";
    pacoteJson += "\"nivel_agua_cm\":" + String(nivelCm, 2) + ",";
    pacoteJson += "\"alerta_local_ativo\":" + (alertaAtivo ? "true" : "false");
    pacoteJson += "}";

    Serial.println("Enviando dados para o servidor...");
    Serial.println(pacoteJson);

    http.begin(client, ENDPOINT_URL);
    http.addHeader("Content-Type", "application/json");

    int httpResponseCode = http.POST(pacoteJson);

    if (httpResponseCode > 0) {
      Serial.print("Codigo de resposta HTTP: ");
      Serial.println(httpResponseCode);
      String payload = http.getString();
      Serial.println("Resposta do servidor:");
      Serial.println(payload);
    } else {
      Serial.print("Erro no envio HTTP: ");
      Serial.println(httpResponseCode);
      Serial.print("Erro ESP32 HTTP: ");
      Serial.println(http.errorToString(httpResponseCode).c_str());
    }
    http.end();
  } else {
    Serial.println("WiFi desconectado. Nao foi possivel enviar os dados.");
  }
}

// ================================================
// Configuração inicial do sistema
// ================================================
void setup() {
  // Configuração dos pinos de saída
  pinMode(PINO_BUZZER, OUTPUT);
  pinMode(PINO_LED_ALERTA, OUTPUT);

  // Inicialização da comunicação serial
  Serial.begin(9600);
  Serial.println("===================================================");
  Serial.println("Sistema Completo de Monitoramento Ambiental");
  Serial.println("Status: Sistema Inicializado");
  Serial.println("---------------------------------------------------");
  Serial.print("Limite Nivel Agua: >= ");
  Serial.print(LIMITE_CRITICO_NIVEL_AGUA);
  Serial.println(" cm");
  Serial.print("Limite Temperatura: >= ");
  Serial.print(LIMITE_CRITICO_TEMPERATURA);
  Serial.println(" C");
  Serial.print("Limite Pressao Atm.: <= ");
  Serial.print(LIMITE_CRITICO_PRESSAO_ATM);
  Serial.println(" hPa");
  Serial.println("===================================================");

  pinMode(PINO_TRIG_HCSR04, OUTPUT);
  pinMode(PINO_ECHO_HCSR04, INPUT);
  pinMode(PINO_BUZZER_LOCAL, OUTPUT);
  pinMode(PINO_LED_ALERTA_LOCAL, OUTPUT);

  digitalWrite(PINO_BUZZER_LOCAL, LOW);
  digitalWrite(PINO_LED_ALERTA_LOCAL, LOW);

  conectarWiFi();
  tempoUltimoEnvio = millis();
}

// ================================================
// Loop principal do programa
// ================================================
void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    // Tenta reconectar periodicamente
    // Uma lógica mais robusta poderia ser implementada aqui, como tentar a cada
    // X segundos. Por simplicidade, vamos tentar reconectar antes de uma
    // tentativa de envio ou em intervalos fixos.
    unsigned long currentMillis = millis();
    if (currentMillis % 30000 < 1000) {
      Serial.println("WiFi desconectado. Tentando reconectar...");
      WiFi.disconnect(); // Força desconexão para limpar estado anterior
      delay(100);
      conectarWiFi();
    }
  }

  long duracaoEchoUs;
  float distanciaCm;

  digitalWrite(PINO_TRIG_HCSR04, LOW);
  delayMicroseconds(2);
  digitalWrite(PINO_TRIG_HCSR04, HIGH);
  delayMicroseconds(10);
  digitalWrite(PINO_TRIG_HCSR04, LOW);

  duracaoEchoUs = pulseIn(PINO_ECHO_HCSR04, HIGH, 30000);

  if (duracaoEchoUs == 0) {
    distanciaCm = -1.0;
  } else {
    distanciaCm = (duracaoEchoUs * VELOCIDADE_SOM_CM_US) / 2.0;
  }

  bool condicaoDeAlertaLocal = false;

  if (distanciaCm > 0 && distanciaCm < LIMITE_DISTANCIA_RISCO_CM) {
    condicaoDeAlertaLocal = true;
  }

  Serial.println("--- Medicao Local HC-SR04 ---");
  if (distanciaCm < 0) {
    Serial.println("Distancia: Fora de alcance / Erro na leitura do sensor");
  } else {
    Serial.print("Distancia medida: ");
    Serial.print(distanciaCm, 2);
    Serial.println(" cm");
  }

  if (condicaoDeAlertaLocal) {
    digitalWrite(PINO_BUZZER_LOCAL, HIGH);
    digitalWrite(PINO_LED_ALERTA_LOCAL, HIGH);
    Serial.println(">>> ALERTA LOCAL DE NIVEL ALTO ATIVADO! <<<");
  } else {
    digitalWrite(PINO_BUZZER_LOCAL, LOW);
    digitalWrite(PINO_LED_ALERTA_LOCAL, LOW);
  }

  // Leitura e conversão dos valores dos sensores
  float nivelAguaAtual =
      map(analogRead(PINO_NIVEL_AGUA), 0, 1023, NIVEL_MIN_MAP, NIVEL_MAX_MAP);
  float temperaturaAtual =
      map(analogRead(PINO_TEMPERATURA), 0, 1023, TEMP_MIN_MAP, TEMP_MAX_MAP);
  float pressaoAtmAtual = map(analogRead(PINO_PRESSAO_ATM), 0, 1023,
                              PRESSAO_MIN_MAP, PRESSAO_MAX_MAP);

  // Verificação das condições de alerta
  bool condicaoDeAlerta = (nivelAguaAtual >= LIMITE_CRITICO_NIVEL_AGUA) ||
                          (temperaturaAtual >= LIMITE_CRITICO_TEMPERATURA) ||
                          (pressaoAtmAtual <= LIMITE_CRITICO_PRESSAO_ATM);

  // Ativação dos alertas locais (buzzer e LED)
  digitalWrite(PINO_BUZZER, condicaoDeAlerta ? HIGH : LOW);
  digitalWrite(PINO_LED_ALERTA, condicaoDeAlerta ? HIGH : LOW);

  // Exibição dos valores lidos no monitor serial
  Serial.println("--- Leitura Local ---");
  Serial.print("Nivel Agua: ");
  Serial.print(nivelAguaAtual, 1);
  Serial.print(" cm | ");
  Serial.print("Temperatura: ");
  Serial.print(temperaturaAtual, 1);
  Serial.print(" C | ");
  Serial.print("Pressao Atm: ");
  Serial.print(pressaoAtmAtual, 1);
  Serial.println(" hPa");

  // Exibição de alerta local quando necessário
  if (condicaoDeAlerta) {
    Serial.println(">>> ALERTA LOCAL ATIVADO! <<<");
  }

  // Preparação dos dados para futura implementação de comunicação
  Serial.println("--- Preparação de Dados ---");
  prepararDados(nivelAguaAtual, temperaturaAtual, pressaoAtmAtual,
                condicaoDeAlerta);
  Serial.println("===================================================");

  unsigned long currentMillisLoop = millis();
  if (currentMillisLoop - tempoUltimoEnvio >= intervaloEnvio ||
      tempoUltimoEnvio == 0) {
    Serial.println("--- Envio de Dados via WiFi ---");
    if (WiFi.status() == WL_CONNECTED) {
      enviarDadosWiFi(distanciaCm, condicaoDeAlertaLocal);
    } else {
      Serial.println("WiFi nao conectado. Envio de dados abortado.");
    }
    tempoUltimoEnvio = currentMillisLoop;
  }

  delay(5000);  // Aguarda 5 segundos antes da próxima leitura
}