// ================================================
// Simulador Básico de Nível de Água
// Este é o primeiro estágio do projeto, focado apenas na simulação
// da medição de nível de água usando um sensor ultrassônico
// Link para simulador:
// https://www.tinkercad.com/things/8Rkj0baa5Ob/editel?sharecode=5CnylevofOCObB0QpEy4gNQz9WdM8xPbh2icTz0exec
// Objetivo: Demonstrar o funcionamento básico da medição de nível
// ================================================

// Definição dos pinos do sensor ultrassônico
const int trigPin = 9;  // Pino de disparo do sensor
const int echoPin = 10; // Pino de recebimento do eco

// Definição dos pinos de alerta
const int buzzerPin = 8; // Buzzer para alerta sonoro
const int ledPin = 7;    // LED para alerta visual

// Configurações do sistema
const float limiteDistancia =
    30.0; // Distância limite para ativar alerta (em cm)
const float FATOR_CONVERSAO_CM =
    0.034; // Fator para converter tempo em distância

// Variáveis para medição
long duracao;    // Duração do eco em microsegundos
float distancia; // Distância calculada em centímetros

// ================================================
// Configuração inicial do sistema
// ================================================
void setup() {
  // Configuração dos pinos
  pinMode(trigPin, OUTPUT);   // Pino de disparo como saída
  pinMode(echoPin, INPUT);    // Pino de eco como entrada
  pinMode(buzzerPin, OUTPUT); // Buzzer como saída
  pinMode(ledPin, OUTPUT);    // LED como saída

  // Inicialização da comunicação serial
  Serial.begin(9600);
  Serial.println("================================================");
  Serial.println("Simulador Básico de Nível de Água");
  Serial.println("================================================");
  Serial.print("Status: Inicializado.\n");
  Serial.print("Limite para alerta (distancia > X cm): ");
  Serial.print(limiteDistancia);
  Serial.println(" cm");
  Serial.println("------------------------------------------------");
}

// ================================================
// Loop principal do programa
// ================================================
void loop() {
  // Envio do pulso ultrassônico
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Medição do tempo de eco
  duracao = pulseIn(echoPin, HIGH);

  // Cálculo da distância
  distancia = duracao * FATOR_CONVERSAO_CM / 2;

  // Exibição das informações no monitor serial
  Serial.print("Distancia: ");
  Serial.print(distancia);
  Serial.println(" cm");

  // Verificação do nível de água e ativação dos alertas
  if (distancia > 0 && distancia > limiteDistancia) {
    // Nível alto detectado - ativa alertas
    digitalWrite(buzzerPin, HIGH); // Ativa buzzer
    digitalWrite(ledPin, HIGH);    // Ativa LED
  } else {
    // Nível normal - desativa alertas
    digitalWrite(buzzerPin, LOW); // Desativa buzzer
    digitalWrite(ledPin, LOW);    // Desativa LED
  }

  delay(500); // Aguarda 500ms antes da próxima medição
}
