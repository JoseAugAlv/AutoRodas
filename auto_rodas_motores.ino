// ======================
// JOYSTICK
// ======================

const int eixoX = A0;
const int eixoY = A1;
const int botao = 2;

// ======================
// IBT-2 ESQUERDA
// ======================

const int RPWM1 = 3;
const int LPWM1 = 6;
const int REN1 = 5;
const int LEN1 = 7;

// ======================
// IBT-2 DIREITA
// ======================

const int RPWM2 = 11;
const int LPWM2 = 10;
const int REN2 = 9;
const int LEN2 = 8;

// ======================
// CONFIGURAÇÕES
// ======================

const int POTENCIA = 100;

// ACELERAÇÃO PARA FRENTE (suave e sincronizada)
const int TAXA_ACELERACAO_FRENTE = 1;    // Quanto maior, mais rápido acelera (1-10)
const int INTERVALO_ACELERACAO = 20;     // ms entre cada incremento

// ======================
// VARIÁVEIS
// ======================

int pwmEsquerdo = 0;
int pwmDireito = 0;
int pwmAtual = 0;  // PWM ÚNICO para ambos os motores (sincronizado)

unsigned long tempoAnterior = 0;

// ======================
// SETUP
// ======================

void setup() {

  Serial.begin(9600);
  pinMode(botao, INPUT_PULLUP);
  
  pinMode(RPWM1, OUTPUT);
  pinMode(LPWM1, OUTPUT);
  pinMode(REN1, OUTPUT);
  pinMode(LEN1, OUTPUT);

  pinMode(RPWM2, OUTPUT);
  pinMode(LPWM2, OUTPUT);
  pinMode(REN2, OUTPUT);
  pinMode(LEN2, OUTPUT);

  digitalWrite(REN1, HIGH);
  digitalWrite(LEN1, HIGH);
  digitalWrite(REN2, HIGH);
  digitalWrite(LEN2, HIGH);

  parar();

  Serial.println("=== CONTROLE COM ACELERAÇÃO SINCRONIZADA ===");
  Serial.print("Potência: ");
  Serial.println(POTENCIA);
  Serial.println("Motores aceleram juntos em TODAS as direções!");
  Serial.println();
}

// ======================
// LOOP
// ======================

void loop() {

  int x = analogRead(eixoX);
  int y = analogRead(eixoY);

  // FREIO (BOTÃO)

  if (digitalRead(botao) == LOW) {
    Serial.println(">>> FREIO");
    freio();
    delay(50);
    return;
  }

  // CALCULAR PWM DOS MOTORES

  calcularPWM(x, y);

  // APLICAR ACELERAÇÃO SINCRONIZADA

  aplicarAceleracaoSincronizada();

  // EXECUTAR MOVIMENTO

  executarMovimento();

  // DEBUG

  static unsigned long ultimoDebug = 0;
  if (millis() - ultimoDebug > 300) {
    Serial.print("D:");
    Serial.print(pwmDireito);
    Serial.print(" E:");
    Serial.println(pwmEsquerdo);
    ultimoDebug = millis();
  }

  delay(20);
}

// ======================
// CALCULAR PWM
// ======================

void calcularPWM(int x, int y) {
  
  // --- CONVERTER JOYSTICK PARA PERCENTUAL ---
  
  // Eixo Y: Frente (+100) a Trás (-100)
  int valorY = 0;
  if (y > 562) {
    valorY = map(y, 562, 1023, 0, 100);
  } else if (y < 462) {
    valorY = map(y, 461, 0, 0, -100);
  }
  
  // Eixo X: Direita (+100) a Esquerda (-100)
  int valorX = 0;
  if (x > 562) {
    valorX = map(x, 562, 1023, 0, 100);
  } else if (x < 462) {
    valorX = map(x, 461, 0, 0, -100);
  }
  
  // --- CORREÇÃO: INVERTE OS LADOS ---
  valorX = -valorX;
  
  // --- CALCULAR PERCENTUAL DE CADA MOTOR ---
  int esquerdoPercent = valorY + valorX;
  int direitoPercent = valorY - valorX;
  
  esquerdoPercent = constrain(esquerdoPercent, -100, 100);
  direitoPercent = constrain(direitoPercent, -100, 100);
  
  // --- CONVERTER PERCENTUAL PARA PWM ---
  
  // Motor Esquerdo
  if (esquerdoPercent > 0) {
    pwmEsquerdo = map(esquerdoPercent, 0, 100, 0, POTENCIA);
  } else if (esquerdoPercent < 0) {
    pwmEsquerdo = -map(abs(esquerdoPercent), 0, 100, 0, POTENCIA);
  } else {
    pwmEsquerdo = 0;
  }
  
  // Motor Direito
  if (direitoPercent > 0) {
    pwmDireito = map(direitoPercent, 0, 100, 0, POTENCIA);
  } else if (direitoPercent < 0) {
    pwmDireito = -map(abs(direitoPercent), 0, 100, 0, POTENCIA);
  } else {
    pwmDireito = 0;
  }
}

// ======================
// APLICAR ACELERAÇÃO SINCRONIZADA (CORRIGIDO)
// ======================

void aplicarAceleracaoSincronizada() {
  
  unsigned long tempoAtual = millis();
  
  // Verifica se algum motor está em FRENTE (valor positivo)
  bool temFrente = (pwmEsquerdo > 0 || pwmDireito > 0);
  
  // Verifica se algum motor está em RÉ (valor negativo)
  bool temRe = (pwmEsquerdo < 0 || pwmDireito < 0);
  
  // --- SÓ ACELERA SE TIVER COMPONENTE DE FRENTE E NÃO TIVER RÉ ---
  if (temFrente && !temRe) {
    // Está indo para FRENTE (puro ou com curva) - ACELERAÇÃO SUAVE E SINCRONIZADA
    if (tempoAtual - tempoAnterior >= INTERVALO_ACELERACAO) {
      
      // Calcula o maior PWM desejado entre os dois motores (em valor absoluto)
      int pwmMaxDesejado = max(abs(pwmEsquerdo), abs(pwmDireito));
      
      // Incrementa o PWM atual (sincronizado)
      if (pwmAtual < pwmMaxDesejado) {
        pwmAtual += TAXA_ACELERACAO_FRENTE;
        if (pwmAtual > pwmMaxDesejado) pwmAtual = pwmMaxDesejado;
        tempoAnterior = tempoAtual;
      }
      
      // Aplica o PWM atual em ambos os motores (respeitando a direção original)
      // Mantém a proporção entre os motores
      if (pwmEsquerdo > 0) {
        // Mantém a proporção do esquerdo em relação ao máximo
        int proporcaoEsquerdo = (pwmEsquerdo * 100) / pwmMaxDesejado;
        pwmEsquerdo = (pwmAtual * proporcaoEsquerdo) / 100;
        if (pwmEsquerdo < 1 && pwmAtual > 0) pwmEsquerdo = 1;
      }
      
      if (pwmDireito > 0) {
        int proporcaoDireito = (pwmDireito * 100) / pwmMaxDesejado;
        pwmDireito = (pwmAtual * proporcaoDireito) / 100;
        if (pwmDireito < 1 && pwmAtual > 0) pwmDireito = 1;
      }
    }
  } else if (temRe) {
    // Está em RÉ - PARADA INSTANTÂNEA (sem aceleração)
    pwmAtual = 0;
    // Mantém os valores negativos originais
  } else {
    // Está parado - PARADA INSTANTÂNEA
    pwmAtual = 0;
    pwmEsquerdo = 0;
    pwmDireito = 0;
  }
}

// ======================
// EXECUTAR MOVIMENTO
// ======================

void executarMovimento() {
  
  // --- MOTOR ESQUERDO ---
  if (pwmEsquerdo > 0) {
    analogWrite(RPWM1, abs(pwmEsquerdo));
    analogWrite(LPWM1, 0);
  } else if (pwmEsquerdo < 0) {
    analogWrite(RPWM1, 0);
    analogWrite(LPWM1, abs(pwmEsquerdo));
  } else {
    analogWrite(RPWM1, 0);
    analogWrite(LPWM1, 0);
  }
  
  // --- MOTOR DIREITO ---
  if (pwmDireito > 0) {
    analogWrite(RPWM2, abs(pwmDireito));
    analogWrite(LPWM2, 0);
  } else if (pwmDireito < 0) {
    analogWrite(RPWM2, 0);
    analogWrite(LPWM2, abs(pwmDireito));
  } else {
    analogWrite(RPWM2, 0);
    analogWrite(LPWM2, 0);
  }
}

// ======================
// PARAR
// ======================

void parar() {
  pwmEsquerdo = 0;
  pwmDireito = 0;
  pwmAtual = 0;
  
  analogWrite(RPWM1, 0);
  analogWrite(LPWM1, 0);
  analogWrite(RPWM2, 0);
  analogWrite(LPWM2, 0);
}

// ======================
// FREIO
// ======================

void freio() {
  analogWrite(RPWM1, 255);
  analogWrite(LPWM1, 255);
  analogWrite(RPWM2, 255);
  analogWrite(LPWM2, 255);
}