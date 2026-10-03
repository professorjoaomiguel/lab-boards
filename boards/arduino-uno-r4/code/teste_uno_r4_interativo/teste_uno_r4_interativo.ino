/*
 * =============================================================================
 *  Teste INTERATIVO (com ajuda do usuário) da placa Arduino UNO R4
 * =============================================================================
 *
 *  Autor: Prof. Joao Miguel Roehe (@professorjoaomiguel)
 *  Licença: MIT (SPDX-License-Identifier: MIT) — ver LICENSE-CODE na raiz
 *
 *  O QUE ESTE SKETCH FAZ
 *  ---------------------
 *  Complementa o teste automático (../teste_uno_r4_automatico) com o que
 *  só uma pessoa consegue conferir: ver um LED acender, ouvir o buzzer,
 *  girar o potenciômetro, cobrir o LDR, medir com o multímetro. O sketch
 *  guia passo a passo pelo Monitor Serial, e cada passo vira um resultado
 *  (OK, FALHA ou PULADO) no resumo do final.
 *
 *  Sempre:
 *    1. led_l ........ o LED "L" (D13) pisca? (resposta s/n)
 *    2. tensao_5v .... medir o pino 5V com o multímetro e digitar o valor
 *  Com o Shield 9 em 1 encaixado:
 *    3. botoes ....... apertar SW1 e depois SW2 (detectado sozinho)
 *    4. pot_adc ...... girar o potenciômetro até os dois extremos; mostra
 *                      a mesma leitura em 10, 12 e 14 bits
 *    5. rgb_pwm ...... ver o LED RGB variar o brilho (resposta s/n)
 *    6. buzzer ....... ouvir a escala musical (resposta s/n)
 *    7. ldr .......... cobrir o LDR e depois iluminar (detectado sozinho)
 *    8. dht11 ........ soprar no DHT11: a umidade sobe (detectado sozinho)
 *    9. ir ........... apertar um botão do controle remoto (detectado sozinho)
 *  Sem shield:
 *   10. dac .......... o A0 gera metade da alimentação; medir e digitar
 *
 *  COMO USAR
 *  ---------
 *  1. Grave o sketch. Abra o Monitor Serial com final de linha "Nova linha"
 *     (Newline); a velocidade não importa no UNO R4 (USB nativa).
 *     Ou: python scripts/serial_placa.py interativo --porta COM8
 *  2. Siga as instruções. Em cada pergunta, digite e envie:
 *       s = sim, funcionou      n = não funcionou      p = pular o passo
 *  3. No fim aparece o resumo. Envie "r" para recomeçar.
 *
 *  As linhas de resultado seguem o mesmo formato do teste automático
 *  (RESULTADO;<teste>;<estado>;<detalhe> e FIM;ok=..;falha=..;...), para
 *  que o scripts/serial_placa.py registre o resultado.
 *
 *  Repositório: https://github.com/professorjoaomiguel/lab-boards
 * =============================================================================
 */

#if !defined(ARDUINO_ARCH_RENESAS_UNO)
#error "Este sketch é só para o Arduino UNO R4 (Minima ou WiFi)."
#endif

const char *VERSAO = "1";

// Pinos do Shield 9 em 1 (ver shields/uno-shield-9in1/README.md)
const uint8_t PINO_SW1          = 2;
const uint8_t PINO_SW2          = 3;
const uint8_t PINO_DHT11        = 4;
const uint8_t PINO_BUZZER       = 5;
const uint8_t PINO_IR           = 6;
const uint8_t PINO_RGB_VERMELHO = 9;
const uint8_t PINO_RGB_AZUL     = 10;
const uint8_t PINO_RGB_VERDE    = 11;
const uint8_t PINO_LED_VERMELHO = 12;
const uint8_t PINO_LED_AZUL     = 13;
const uint8_t PINO_POT          = A0;
const uint8_t PINO_LDR          = A1;

// =============================================================================
//  RESULTADOS
// =============================================================================
unsigned int totalOk, totalFalha, totalAviso, totalPulado;

void resultado(const char *teste, const char *estado, const String &detalhe) {
  Serial.print("RESULTADO;");
  Serial.print(teste);
  Serial.print(';');
  Serial.print(estado);
  Serial.print(';');
  Serial.println(detalhe);

  if (strcmp(estado, "OK") == 0) totalOk++;
  else if (strcmp(estado, "FALHA") == 0) totalFalha++;
  else if (strcmp(estado, "AVISO") == 0) totalAviso++;
  else if (strcmp(estado, "PULADO") == 0) totalPulado++;
}

void titulo(const char *texto) {
  Serial.println();
  Serial.println("# ------------------------------------------------------------");
  Serial.print("# ");
  Serial.println(texto);
  Serial.println("# ------------------------------------------------------------");
}

// =============================================================================
//  ENTRADA DO USUÁRIO
// =============================================================================

// Lê uma linha enviada pelo Monitor Serial (até o Enter). Espera para
// sempre: num teste interativo, quem dita o ritmo é a pessoa.
String lerLinha() {
  String linha;
  while (true) {
    if (Serial.available()) {
      char c = Serial.read();
      if (c == '\n') break;
      if (c != '\r') linha += c;
    }
  }
  linha.trim();
  return linha;
}

// Mostra a pergunta e espera s, n ou p. Devolve 's', 'n' ou 'p'.
char perguntar(const char *pergunta) {
  while (true) {
    Serial.print("> ");
    Serial.print(pergunta);
    Serial.println(" (s = sim, n = não, p = pular)");
    String resposta = lerLinha();
    resposta.toLowerCase();
    if (resposta == "s" || resposta == "n" || resposta == "p") {
      return resposta[0];
    }
    Serial.println("# Resposta não entendida. Digite s, n ou p e envie.");
  }
}

// Registra o resultado de um passo respondido com s/n/p.
void registrarResposta(const char *teste, char resposta, const String &detalhe) {
  if (resposta == 's') resultado(teste, "OK", detalhe);
  else if (resposta == 'n') resultado(teste, "FALHA", detalhe);
  else resultado(teste, "PULADO", "pulado pelo usuário");
}

// Pede um número (ex: tensão lida no multímetro). Aceita vírgula ou ponto.
// Devolve false se o usuário enviou "p" para pular.
bool perguntarNumero(const char *pergunta, float &valor) {
  while (true) {
    Serial.print("> ");
    Serial.print(pergunta);
    Serial.println(" (ou p para pular)");
    String resposta = lerLinha();
    resposta.toLowerCase();
    if (resposta == "p") return false;
    resposta.replace(',', '.');
    if (resposta.length() > 0 && (isDigit(resposta[0]) || resposta[0] == '.')) {
      valor = resposta.toFloat();
      return true;
    }
    Serial.println("# Número não entendido. Ex: 4.85");
  }
}

// Espera o Enter para começar um passo; "p" pula. Devolve false se pulou.
bool prontoParaComecar() {
  Serial.println("> Envie Enter para começar (ou p para pular).");
  String resposta = lerLinha();
  resposta.toLowerCase();
  return resposta != "p";
}

// =============================================================================
//  PASSOS SEMPRE FEITOS
// =============================================================================
void passoLedL() {
  titulo("1. LED L da placa (D13)");
  Serial.println("# O LED 'L' (perto do conector USB) vai piscar 5 vezes.");
  Serial.println("# Com o shield, o LED azul de 3 mm do shield pisca junto.");
  pinMode(LED_BUILTIN, OUTPUT);
  for (uint8_t i = 0; i < 5; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(300);
    digitalWrite(LED_BUILTIN, LOW);
    delay(300);
  }
  registrarResposta("led_l", perguntar("O LED L piscou 5 vezes?"), "LED L (D13)");
}

void passoTensao5v() {
  titulo("2. Tensão do pino 5V (multímetro)");
  Serial.println("# Multímetro em V DC (escala 20V): ponta preta no GND, vermelha");
  Serial.println("# no pino 5V do header. Na USB, o UNO R4 costuma dar ~4,7V a 5,0V");
  Serial.println("# (o datasheet cita a queda no diodo de proteção da USB).");
  float volts;
  if (!perguntarNumero("Quantos volts você mediu no pino 5V?", volts)) {
    resultado("tensao_5v", "PULADO", "pulado pelo usuário");
    return;
  }
  String detalhe = String(volts, 2) + " V medidos";
  if (volts >= 4.5 && volts <= 5.25) {
    resultado("tensao_5v", "OK", detalhe);
  } else {
    resultado("tensao_5v", "FALHA", detalhe + " (esperado entre 4,5 e 5,25 V)");
  }
}

// =============================================================================
//  PASSOS COM O SHIELD
// =============================================================================

// Espera um botão (ativo em LOW, pull-up de 10 kΩ no shield) ser apertado.
bool esperarBotao(uint8_t pino, unsigned long limiteMs) {
  unsigned long inicio = millis();
  while (millis() - inicio < limiteMs) {
    if (digitalRead(pino) == LOW) {
      delay(30);  // debounce: confirma que continua apertado
      if (digitalRead(pino) == LOW) return true;
    }
  }
  return false;
}

void passoBotoes() {
  titulo("3. Botões SW1 (D2) e SW2 (D3)");
  pinMode(PINO_SW1, INPUT);  // o shield já tem pull-up externo
  pinMode(PINO_SW2, INPUT);
  if (!prontoParaComecar()) {
    resultado("botoes", "PULADO", "pulado pelo usuário");
    return;
  }
  Serial.println("> Aperte SW1 (você tem 10 s)...");
  bool sw1 = esperarBotao(PINO_SW1, 10000);
  Serial.println(sw1 ? "# SW1 detectado." : "# SW1 não detectado.");
  delay(300);
  Serial.println("> Agora aperte SW2 (10 s)...");
  bool sw2 = esperarBotao(PINO_SW2, 10000);
  Serial.println(sw2 ? "# SW2 detectado." : "# SW2 não detectado.");

  String detalhe = String("SW1 ") + (sw1 ? "ok" : "não detectado") +
                   ", SW2 " + (sw2 ? "ok" : "não detectado");
  resultado("botoes", (sw1 && sw2) ? "OK" : "FALHA", detalhe);
}

// O ADC do RA4M1 vai até 14 bits. O mesmo valor de tensão aparece como
// 0–1023 (10 bits), 0–4095 (12 bits) ou 0–16383 (14 bits): mais bits, mais
// degraus entre 0V e a referência.
void passoPotenciometro() {
  titulo("4. Potenciômetro (A0) e resolução do ADC");
  Serial.println("# Em 20 s, gire o potenciômetro devagar até um extremo e depois");
  Serial.println("# até o outro. Durante o teste, o LED vermelho do RGB acompanha.");
  if (!prontoParaComecar()) {
    resultado("pot_adc", "PULADO", "pulado pelo usuário");
    return;
  }
  analogReadResolution(14);
  long minimo = 16383, maximo = 0;
  unsigned long inicio = millis(), ultimoPrint = 0;
  while (millis() - inicio < 20000) {
    long v14 = analogRead(PINO_POT);
    minimo = min(minimo, v14);
    maximo = max(maximo, v14);
    analogWrite(PINO_RGB_VERMELHO, v14 >> 6);  // 14 bits -> 8 bits do PWM
    if (millis() - ultimoPrint > 500) {
      ultimoPrint = millis();
      Serial.print("# 10 bits: ");
      Serial.print(v14 >> 4);
      Serial.print("\t12 bits: ");
      Serial.print(v14 >> 2);
      Serial.print("\t14 bits: ");
      Serial.println(v14);
    }
  }
  analogWrite(PINO_RGB_VERMELHO, 0);
  analogReadResolution(10);

  // Extremos: abaixo de 2% e acima de 98% da escala.
  bool ok = minimo < 16383L * 2 / 100 && maximo > 16383L * 98 / 100;
  resultado("pot_adc", ok ? "OK" : "FALHA",
            "faixa vista " + String(minimo) + " a " + String(maximo) + " de 16383");
}

void passoRgb() {
  titulo("5. LED RGB com PWM (D9, D10, D11)");
  Serial.println("# Cada cor vai acender e apagar devagar, na ordem: vermelho (D9),");
  Serial.println("# azul (D10), verde (D11). Ordem confirmada neste shield.");
  const uint8_t pinos[3] = {PINO_RGB_VERMELHO, PINO_RGB_AZUL, PINO_RGB_VERDE};
  for (uint8_t i = 0; i < 3; i++) {
    for (int b = 0; b <= 255; b += 5) { analogWrite(pinos[i], b); delay(8); }
    for (int b = 255; b >= 0; b -= 5) { analogWrite(pinos[i], b); delay(8); }
    analogWrite(pinos[i], 0);
  }
  registrarResposta("rgb_pwm",
                    perguntar("Viu vermelho, azul e verde variando o brilho suavemente?"),
                    "D9 vermelho, D10 azul, D11 verde");
}

// =============================================================================
//  MELODIA PARA O BUZZER: "Nokia Tune"
// =============================================================================
//
//  Trecho da "Gran Vals" de Francisco Tárrega (1902, domínio público), o
//  toque famoso dos celulares Nokia. Cada nota é uma frequência (Hz) e uma
//  duração em colcheias (1 = colcheia, 2 = semínima, 4 = mínima).
//  Entre uma nota e outra há uma pausa curta: sem ela, duas notas seguidas
//  soariam "grudadas".
const unsigned int NOKIA_NOTAS[13] = {
  659, 587, 370, 415,   // mi5 ré5 fá#4 sol#4
  554, 494, 294, 330,   // dó#5 si4 ré4 mi4
  494, 440, 277, 330,   // si4 lá4 dó#4 mi4
  440                   // lá4
};
const uint8_t NOKIA_DURACOES[13] = {1, 1, 2, 2, 1, 1, 2, 2, 1, 1, 2, 2, 4};
const unsigned int COLCHEIA_MS = 150;  // andamento: 200 semínimas por minuto

void tocarNokia(uint8_t pino) {
  for (uint8_t i = 0; i < 13; i++) {
    unsigned int duracao = NOKIA_DURACOES[i] * COLCHEIA_MS;
    tone(pino, NOKIA_NOTAS[i]);
    delay(duracao * 9 / 10);  // 90% do tempo soando...
    noTone(pino);
    delay(duracao / 10);      // ...e 10% de silêncio entre as notas
  }
}

void passoBuzzer() {
  titulo("6. Buzzer (D5)");
  Serial.println("# Vai tocar a escala dó-ré-mi-fá-sol-lá-si-dó e depois o \"Nokia Tune\".");
  const unsigned int notas[8] = {262, 294, 330, 349, 392, 440, 494, 523};
  for (uint8_t i = 0; i < 8; i++) {
    tone(PINO_BUZZER, notas[i]);
    delay(250);
  }
  noTone(PINO_BUZZER);
  delay(500);
  tocarNokia(PINO_BUZZER);
  registrarResposta("buzzer", perguntar("Ouviu a escala com 8 notas diferentes e reconheceu a melodia?"),
                    "escala e Nokia Tune com tone()");
}

void passoLdr() {
  titulo("7. LDR - luminosidade (A1)");
  Serial.println("# Em 15 s: cubra o LDR com o dedo e depois aponte a lanterna do");
  Serial.println("# celular para ele. A leitura sobe com a luz (medido no shield).");
  if (!prontoParaComecar()) {
    resultado("ldr", "PULADO", "pulado pelo usuário");
    return;
  }
  int minimo = 1023, maximo = 0;
  unsigned long inicio = millis(), ultimoPrint = 0;
  while (millis() - inicio < 15000) {
    int v = analogRead(PINO_LDR);
    minimo = min(minimo, v);
    maximo = max(maximo, v);
    if (millis() - ultimoPrint > 500) {
      ultimoPrint = millis();
      Serial.print("# A1 = ");
      Serial.println(v);
    }
  }
  // Coberto x iluminado deve dar uma diferença de centenas.
  bool ok = (maximo - minimo) >= 200;
  resultado("ldr", ok ? "OK" : "FALHA",
            "faixa vista " + String(minimo) + " a " + String(maximo) + " de 1023");
}

// DHT11: mesma leitura do teste automático (ver o comentário completo do
// protocolo em shields/uno-shield-9in1/code).
uint16_t contarNivel(uint8_t nivel) {
  const uint16_t LIMITE = F_CPU / 1000;
  uint16_t contagem = 0;
  while (digitalRead(PINO_DHT11) == nivel) {
    if (++contagem >= LIMITE) return 0xFFFF;
  }
  return contagem;
}

// Devolve true e preenche umidade se a leitura for válida.
bool lerUmidadeDht11(float &umidade) {
  uint8_t dados[5] = {0, 0, 0, 0, 0};
  uint16_t ciclos[80];
  pinMode(PINO_DHT11, OUTPUT);
  digitalWrite(PINO_DHT11, LOW);
  delay(20);
  pinMode(PINO_DHT11, INPUT_PULLUP);
  delayMicroseconds(55);

  noInterrupts();
  if (contarNivel(LOW) == 0xFFFF || contarNivel(HIGH) == 0xFFFF) {
    interrupts();
    return false;
  }
  for (uint8_t i = 0; i < 80; i += 2) {
    ciclos[i]     = contarNivel(LOW);
    ciclos[i + 1] = contarNivel(HIGH);
  }
  interrupts();

  for (uint8_t bit = 0; bit < 40; bit++) {
    if (ciclos[2 * bit] == 0xFFFF || ciclos[2 * bit + 1] == 0xFFFF) return false;
    dados[bit / 8] <<= 1;
    if (ciclos[2 * bit + 1] > ciclos[2 * bit]) dados[bit / 8] |= 1;
  }
  uint8_t soma = dados[0] + dados[1] + dados[2] + dados[3];
  // A 1ª leitura depois de ligar vem zerada (o DHT11 entrega a medição
  // anterior, que ainda não existe) e passaria na soma: descartar.
  if (soma != dados[4] || (dados[0] == 0 && dados[2] == 0)) return false;
  umidade = dados[0] + dados[1] * 0.1;
  return true;
}

void passoDht11() {
  titulo("8. DHT11 - umidade (D4)");
  Serial.println("# Primeiro, uma leitura de referência. Depois, quando pedir, sopre");
  Serial.println("# devagar no sensor (bafo, como para embaçar um vidro) por 3 s.");
  if (!prontoParaComecar()) {
    resultado("dht11", "PULADO", "pulado pelo usuário");
    return;
  }
  float referencia = 0;
  bool ok = false;
  for (uint8_t i = 0; i < 3 && !ok; i++) {
    delay(1200);  // intervalo mínimo entre leituras do DHT11
    ok = lerUmidadeDht11(referencia);
  }
  if (!ok) {
    resultado("dht11", "FALHA", "o sensor não respondeu");
    return;
  }
  Serial.print("# Umidade de referência: ");
  Serial.print(referencia, 0);
  Serial.println(" %");
  Serial.println("> Sopre no DHT11 agora (o teste olha por 20 s).");

  float maximo = referencia;
  unsigned long inicio = millis();
  while (millis() - inicio < 20000 && maximo < referencia + 5) {
    delay(1500);
    float u;
    if (lerUmidadeDht11(u)) {
      maximo = max(maximo, u);
      Serial.print("# Umidade: ");
      Serial.print(u, 0);
      Serial.println(" %");
    }
  }
  String detalhe = "de " + String(referencia, 0) + " % até " + String(maximo, 0) + " %";
  resultado("dht11", (maximo >= referencia + 5) ? "OK" : "FALHA", detalhe);
}

void passoIr() {
  titulo("9. Receptor infravermelho (D6)");
  Serial.println("# Aponte um controle remoto (TV, ar-condicionado, kit Arduino) para");
  Serial.println("# o receptor e aperte qualquer botão. O teste espera 15 s.");
  if (!prontoParaComecar()) {
    resultado("ir", "PULADO", "pulado pelo usuário");
    return;
  }
  pinMode(PINO_IR, INPUT);
  // Em repouso a saída do receptor fica em HIGH e vai para LOW enquanto
  // recebe luz IR modulada. Basta ver um LOW de pelo menos 2 ms: o início
  // de um comando NEC, por exemplo, é um LOW de 9 ms. Ruído dá pulsos
  // bem mais curtos.
  unsigned long inicio = millis();
  bool recebeu = false;
  while (!recebeu && millis() - inicio < 15000) {
    if (digitalRead(PINO_IR) == LOW) {
      unsigned long t0 = micros();
      while (digitalRead(PINO_IR) == LOW && micros() - t0 < 20000) {}
      if (micros() - t0 >= 2000) recebeu = true;
    }
  }
  if (recebeu) {
    digitalWrite(PINO_LED_AZUL, HIGH);
    delay(300);
    digitalWrite(PINO_LED_AZUL, LOW);
  }
  resultado("ir", recebeu ? "OK" : "FALHA",
            recebeu ? "sinal recebido" : "nenhum sinal em 15 s (pilha do controle?)");
}

// =============================================================================
//  PASSO SEM SHIELD
// =============================================================================
void passoDac() {
  titulo("10. DAC no A0 (multímetro)");
  Serial.println("# O A0 do UNO R4 é uma saída analógica de verdade (DAC de 12 bits).");
  Serial.println("# Ele vai gerar METADE da alimentação analógica (~2,4 V a 2,5 V).");
  Serial.println("# Meça com o multímetro entre A0 e GND. Nada mais ligado no A0!");
  float v5;
  if (!perguntarNumero("Antes: quantos volts no pino 5V?", v5)) {
    resultado("dac", "PULADO", "pulado pelo usuário");
    return;
  }
  analogWriteResolution(12);
  analogWrite(DAC, 2048);
  float va0;
  bool mediu = perguntarNumero("Quantos volts no A0 agora?", va0);
  analogWrite(DAC, 0);
  if (!mediu) {
    resultado("dac", "PULADO", "pulado pelo usuário");
    return;
  }
  float esperado = v5 / 2;
  String detalhe = String(va0, 2) + " V (esperado ~" + String(esperado, 2) + " V)";
  // 5% de tolerância: erro do DAC somado ao do multímetro.
  resultado("dac", fabs(va0 - esperado) <= esperado * 0.05 ? "OK" : "FALHA", detalhe);
}

// =============================================================================
//  ROTEIRO
// =============================================================================

// Mesmo truque do teste automático: o shield tem pull-ups externos em D2,
// D3, D4 e D6. Descarrega cada pino e vê se ele sobe sozinho.
bool shieldPresente() {
  const uint8_t pinos[4] = {PINO_SW1, PINO_SW2, PINO_DHT11, PINO_IR};
  uint8_t altos = 0;
  for (uint8_t i = 0; i < 4; i++) {
    pinMode(pinos[i], OUTPUT);
    digitalWrite(pinos[i], LOW);
    delayMicroseconds(100);
    pinMode(pinos[i], INPUT);
    delayMicroseconds(200);
    if (digitalRead(pinos[i]) == HIGH) altos++;
  }
  return altos >= 3;
}

void rodarTestes() {
  totalOk = totalFalha = totalAviso = totalPulado = 0;
  Serial.println();
  Serial.print("INICIO;teste_uno_r4_interativo;");
  Serial.println(VERSAO);
  Serial.println("# Teste interativo do UNO R4. Responda s (sim), n (não) ou p (pular).");
  Serial.println("# Use final de linha \"Nova linha\" no Monitor Serial.");

  bool detectado = shieldPresente();
  Serial.print("# Shield 9 em 1 ");
  Serial.println(detectado ? "DETECTADO." : "NÃO detectado.");
  char confirma = perguntar(detectado ? "O Shield 9 em 1 está encaixado?"
                                      : "A placa está SEM shield e sem nada nos pinos?");
  bool comShield = detectado ? (confirma == 's') : (confirma != 's');
  if (confirma == 'p') {
    Serial.println("# Sem confirmação: só os passos que não usam os pinos.");
  }

  passoLedL();
  passoTensao5v();

  if (comShield && confirma != 'p') {
    pinMode(PINO_BUZZER, OUTPUT);
    pinMode(PINO_LED_VERMELHO, OUTPUT);
    passoBotoes();
    passoPotenciometro();
    passoRgb();
    passoBuzzer();
    passoLdr();
    passoDht11();
    passoIr();
  } else if (!detectado && confirma == 's') {
    passoDac();
  }

  Serial.println();
  Serial.print("FIM;ok=");
  Serial.print(totalOk);
  Serial.print(";falha=");
  Serial.print(totalFalha);
  Serial.print(";aviso=");
  Serial.print(totalAviso);
  Serial.print(";pulado=");
  Serial.println(totalPulado);
  Serial.println(totalFalha == 0 ? "# Nenhuma falha." : "# Há FALHAS: veja as linhas RESULTADO.");
  Serial.println("# Envie \"r\" para recomeçar.");
}

void setup() {
  Serial.begin(115200);
}

// Como no teste automático: começa sempre que um programa abre a porta.
bool conectadoAntes = false;

void loop() {
  bool conectado = Serial;
  if (conectado && !conectadoAntes) {
    delay(300);
    rodarTestes();
  }
  conectadoAntes = conectado;

  if (conectado && Serial.available()) {
    char c = Serial.read();
    if (c == 'r' || c == 'R') rodarTestes();
  }
}
