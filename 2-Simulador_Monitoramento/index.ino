// ================================================
// Simulador de Monitoramento Ambiental Completo
// Este é o segundo estágio do projeto, que simula o monitoramento
// de múltiplos parâmetros ambientais (nível de água, temperatura e pressão)
// 
// Objetivo: Demonstrar o funcionamento do sistema completo de monitoramento
// sem a parte de comunicação
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
// Configuração inicial do sistema
// ================================================
void setup() {
  // Configuração dos pinos de saída
  pinMode(PINO_BUZZER, OUTPUT);
  pinMode(PINO_LED_ALERTA, OUTPUT);

  // Inicialização da comunicação serial
  Serial.begin(9600);
  Serial.println("===================================================");
  Serial.println("Simulador de Monitoramento Ambiental Completo");
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
}

// ================================================
// Loop principal do programa
// ================================================
void loop() {
  // Leitura e conversão dos valores dos sensores
  float nivelAguaAtual =
      map(analogRead(PINO_NIVEL_AGUA), 0, 1023, NIVEL_MIN_MAP, NIVEL_MAX_MAP);
  float temperaturaAtual =
      map(analogRead(PINO_TEMPERATURA), 0, 1023, TEMP_MIN_MAP, TEMP_MAX_MAP);
  float pressaoAtmAtual = map(analogRead(PINO_PRESSAO_ATM), 0, 1023,
                              PRESSAO_MIN_MAP, PRESSAO_MAX_MAP);

  // Exibição dos valores lidos no monitor serial
  Serial.print("Nivel Agua: ");
  Serial.print(nivelAguaAtual);
  Serial.print(" cm | ");
  Serial.print("Temperatura: ");
  Serial.print(temperaturaAtual);
  Serial.print(" C | ");
  Serial.print("Pressao Atm: ");
  Serial.print(pressaoAtmAtual);
  Serial.println(" hPa");

  // Verificação das condições de alerta
  bool condicaoDeAlerta = (nivelAguaAtual >= LIMITE_CRITICO_NIVEL_AGUA) ||
                          (temperaturaAtual >= LIMITE_CRITICO_TEMPERATURA) ||
                          (pressaoAtmAtual <= LIMITE_CRITICO_PRESSAO_ATM);

  // Ativação dos alertas (buzzer e LED)
  digitalWrite(PINO_BUZZER, condicaoDeAlerta ? HIGH : LOW);
  digitalWrite(PINO_LED_ALERTA, condicaoDeAlerta ? HIGH : LOW);

  // Exibição de mensagem de alerta quando necessário
  if (condicaoDeAlerta) {
    Serial.println(">>> ALERTA CRITICO DETECTADO! <<<");
  }
  Serial.println("---");

  delay(5000);  // Aguarda 5 segundos antes da próxima leitura
} 