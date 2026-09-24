/*
 * =============================================================================
 *  Teste dos periféricos do Shield Multifunção 9 em 1 (formato UNO)
 * =============================================================================
 *
 *  Placas suportadas: Arduino UNO R3 e Arduino UNO R4 (Minima e WiFi).
 *  NÃO use este shield em placas de 3,3V (ex: ESP32): ele é de 5V e pode
 *  queimar as portas do microcontrolador. Ver ../../README.md.
 *
 *  O QUE ESTE SKETCH FAZ
 *  ---------------------
 *  Mostra um menu no Monitor Serial com um teste para cada periférico do
 *  shield. Cada teste diz o que deve acontecer e o que observar, para que
 *  o aluno confirme se aquele periférico está funcionando.
 *
 *    Pino | Periférico
 *    -----+----------------------------------------------
 *    D2   | Botão SW1
 *    D3   | Botão SW2
 *    D4   | Sensor de temperatura e umidade DHT11
 *    D5   | Buzzer
 *    D6   | Receptor infravermelho (IR)
 *    D9   | LED RGB (uma das cores)
 *    D10  | LED RGB (uma das cores)
 *    D11  | LED RGB (uma das cores)
 *    D12  | LED vermelho de 3 mm (LED2 na placa)
 *    D13  | LED azul de 3 mm (LED1 na placa) — também é o LED_BUILTIN
 *    A0   | Potenciômetro
 *    A1   | LDR (sensor de luminosidade)
 *    A2   | Sensor de temperatura LM35
 *
 *  COMO USAR
 *  ---------
 *  1. Encaixe o shield na placa UNO R3 ou UNO R4 e ligue a placa na USB.
 *  2. Na IDE do Arduino, escolha a placa certa (Ferramentas > Placa) e a
 *     porta COM, e grave este sketch.
 *  3. Abra o Monitor Serial e configure:
 *       - velocidade: 9600 baud
 *       - final de linha: "Nova linha" (Newline)
 *     O final de linha importa: vários testes pedem "pressione Enter", e com
 *     "Sem final de linha" o Enter sozinho não envia nada para a placa.
 *  4. Digite o número do teste e pressione Enter.
 *
 *  NENHUMA BIBLIOTECA EXTERNA É NECESSÁRIA
 *  ---------------------------------------
 *  O DHT11 e o receptor IR são lidos diretamente neste arquivo, sem
 *  bibliotecas. Há dois motivos:
 *    - O aluno grava e testa sem instalar nada.
 *    - A biblioteca IRremote usa o Timer2 no UNO R3. O Timer2 também gera
 *      o PWM do pino D11 (uma cor do LED RGB) e a função tone() do buzzer.
 *      Usar IRremote junto com esses periféricos faria um deles parar de
 *      funcionar. Lendo o IR "na mão", nenhum timer é usado.
 *
 *  AJUSTES QUE PODEM SER NECESSÁRIOS
 *  ---------------------------------
 *  Nem todo lote do shield usa os mesmos componentes. Os próprios testes
 *  ajudam a descobrir os valores certos das constantes da seção
 *  "AJUSTES" logo abaixo:
 *    - RGB_ANODO_COMUM     -> teste 2 (LED RGB)
 *    - BUZZER_NIVEL_LIGADO -> teste 4 (Buzzer)
 *    - TENSAO_REFERENCIA   -> teste 7 (LM35)
 *
 *  Repositório: https://github.com/professorjoaomiguel/lab-boards
 * =============================================================================
 */

// =============================================================================
//  PINOS
//  Os números seguem a serigrafia impressa no shield.
// =============================================================================
const uint8_t PINO_SW1          = 2;
const uint8_t PINO_SW2          = 3;
const uint8_t PINO_DHT11        = 4;
const uint8_t PINO_BUZZER       = 5;
const uint8_t PINO_IR           = 6;
const uint8_t PINO_RGB_D9       = 9;
const uint8_t PINO_RGB_D10      = 10;
const uint8_t PINO_RGB_D11      = 11;
const uint8_t PINO_LED_VERMELHO = 12;
const uint8_t PINO_LED_AZUL     = 13;
const uint8_t PINO_POT          = A0;
const uint8_t PINO_LDR          = A1;
const uint8_t PINO_LM35         = A2;

// Os três pinos do LED RGB, na ordem em que o teste 2 os acende.
// A cor de cada pino (vermelho, verde ou azul) depende do LED montado no
// shield. O teste 2 serve justamente para descobrir essa correspondência.
const uint8_t PINOS_RGB[3] = {PINO_RGB_D9, PINO_RGB_D10, PINO_RGB_D11};

// =============================================================================
//  AJUSTES
// =============================================================================

// Velocidade da comunicação serial. Deve ser igual à do Monitor Serial.
const unsigned long VELOCIDADE_SERIAL = 9600;

// Tipo do LED RGB:
//   false = cátodo comum: o pino em HIGH acende a cor (caso mais comum).
//   true  = ânodo comum: o pino em LOW acende a cor.
// Se no teste 2 o LED ficar ACESO quando o texto disser "apagado" (e
// apagado quando disser "aceso"), troque este valor.
const bool RGB_ANODO_COMUM = false;

// Nível do pino D5 que faz o buzzer tocar:
//   HIGH = buzzer acionado por transistor NPN (ou ligado direto no pino).
//   LOW  = buzzer acionado por transistor PNP (ex: S8550), comum nesses
//          shields.
// Se o buzzer ficar apitando sem parar logo que a placa liga, este valor
// está trocado. O teste 4 mostra qual é o certo.
const uint8_t BUZZER_NIVEL_LIGADO = HIGH;

// Tensão de referência do conversor analógico-digital (ADC), em volts.
// Por padrão é a tensão de alimentação da placa (5V). Na prática ela varia:
//   - UNO R3 na USB: entre 4,7V e 5,0V, dependendo do cabo e da porta USB.
//   - UNO R4 na USB: cerca de 4,7V (o próprio datasheet cita a queda no
//     diodo de proteção).
// Para leituras mais precisas (principalmente do LM35), meça com um
// multímetro a tensão entre os pinos 5V e GND da placa e coloque aqui.
const float TENSAO_REFERENCIA = 5.0;

// Valor máximo lido pelo ADC. Com 10 bits de resolução (padrão tanto no
// UNO R3 quanto no UNO R4), analogRead() retorna de 0 a 1023.
const float ADC_MAXIMO = 1023.0;

// =============================================================================
//  FUNÇÕES AUXILIARES: SERIAL
// =============================================================================
//
//  Sobre o F("..."): no UNO R3, o ATmega328P tem só 2 KB de RAM. Sem o F(),
//  cada texto entre aspas é copiado para a RAM quando a placa liga, e um
//  sketch com muitas mensagens como este esgotaria a memória. Com F(), o
//  texto fica apenas na memória flash (32 KB). No UNO R4 o F() não é
//  necessário, mas também não atrapalha.

// Descarta tudo o que chegou pela serial (por exemplo, o '\n' do Enter).
// O pequeno atraso dá tempo para o resto da linha digitada chegar.
void limparSerial() {
  delay(10);
  while (Serial.available() > 0) {
    Serial.read();
  }
}

// Retorna true se o usuário enviou algo pelo Monitor Serial.
// Usada nos testes contínuos para saber quando voltar ao menu.
bool usuarioPediuParar() {
  if (Serial.available() > 0) {
    limparSerial();
    return true;
  }
  return false;
}

// Fica parado até o usuário pressionar Enter no Monitor Serial.
void esperarEnter() {
  Serial.println(F("   (pressione Enter para continuar)"));
  while (Serial.available() == 0) {
    // espera
  }
  limparSerial();
}

// Desenha uma barra de progresso no Monitor Serial, ex: [#######-------]
// fracao vai de 0.0 (vazia) a 1.0 (cheia).
void imprimirBarra(float fracao) {
  const uint8_t LARGURA = 30;
  uint8_t cheios = (uint8_t)(constrain(fracao, 0.0, 1.0) * LARGURA + 0.5);
  Serial.print('[');
  for (uint8_t i = 0; i < LARGURA; i++) {
    Serial.print(i < cheios ? '#' : '-');
  }
  Serial.print(']');
}

void imprimirTitulo(const __FlashStringHelper *titulo) {
  Serial.println();
  Serial.println(F("------------------------------------------------------------"));
  Serial.println(titulo);
  Serial.println(F("------------------------------------------------------------"));
}

// =============================================================================
//  FUNÇÕES AUXILIARES: SAÍDAS
// =============================================================================

// Acende uma cor do LED RGB com o brilho indicado (0 = apagado, 255 =
// máximo). A conta "255 - brilho" inverte o sinal no LED de ânodo comum,
// em que o pino em LOW é que acende.
void rgbEscrever(uint8_t pino, uint8_t brilho) {
  analogWrite(pino, RGB_ANODO_COMUM ? 255 - brilho : brilho);
}

void rgbApagar() {
  for (uint8_t i = 0; i < 3; i++) {
    rgbEscrever(PINOS_RGB[i], 0);
  }
}

// Liga ou desliga o buzzer respeitando BUZZER_NIVEL_LIGADO.
void buzzerLigar(bool ligar) {
  uint8_t nivelDesligado = (BUZZER_NIVEL_LIGADO == HIGH) ? LOW : HIGH;
  digitalWrite(PINO_BUZZER, ligar ? BUZZER_NIVEL_LIGADO : nivelDesligado);
}

// Apaga todas as saídas. Chamado ao voltar para o menu.
void desligarTudo() {
  noTone(PINO_BUZZER);   // noTone() deixa o pino em LOW...
  buzzerLigar(false);    // ...então ele é recolocado no nível "desligado".
  rgbApagar();
  digitalWrite(PINO_LED_VERMELHO, LOW);
  digitalWrite(PINO_LED_AZUL, LOW);
}

// =============================================================================
//  FUNÇÕES AUXILIARES: ENTRADAS ANALÓGICAS
// =============================================================================

// Faz a média de várias leituras para reduzir o ruído do ADC.
float lerAnalogicoMedio(uint8_t pino) {
  const uint8_t AMOSTRAS = 16;
  unsigned long soma = 0;
  for (uint8_t i = 0; i < AMOSTRAS; i++) {
    soma += analogRead(pino);
  }
  return (float)soma / AMOSTRAS;
}

// Converte uma leitura do ADC (0 a 1023) para tensão em volts.
float adcParaVolts(float leitura) {
  return leitura * TENSAO_REFERENCIA / ADC_MAXIMO;
}

// =============================================================================
//  LEITURA DO DHT11 (sem biblioteca)
// =============================================================================
//
//  O DHT11 usa um protocolo de um fio só, que carrega os dados nos dois
//  sentidos:
//
//   1. A placa puxa a linha para LOW por pelo menos 18 ms ("acorde!") e
//      depois a solta. Quem mantém a linha em HIGH é um resistor de
//      pull-up.
//   2. O sensor responde com LOW por ~80 µs e HIGH por ~80 µs.
//   3. O sensor envia 40 bits. Cada bit começa com LOW de ~50 µs, seguido
//      de um HIGH cuja duração define o valor:
//        HIGH de ~26 µs  -> bit 0
//        HIGH de ~70 µs  -> bit 1
//   4. Os 40 bits formam 5 bytes:
//        [0] umidade (parte inteira)    [1] umidade (parte decimal)
//        [2] temperatura (parte inteira) [3] temperatura (parte decimal)
//        [4] soma de verificação (checksum) = soma dos 4 primeiros bytes
//
//  Em vez de medir os tempos em microssegundos, contamos quantas voltas o
//  laço dá em cada nível. Comparar a contagem do HIGH com a do LOW (sempre
//  ~50 µs) diz se o bit é 0 ou 1. Isso funciona igual no UNO R3 (16 MHz) e
//  no UNO R4 (48 MHz), sem depender da velocidade do processador. É a mesma
//  técnica da biblioteca da Adafruit.
//
//  As interrupções ficam desligadas durante os ~5 ms da leitura, para que
//  nada (como o contador do millis()) atrapalhe a contagem.

// Códigos de retorno de lerDht11()
const uint8_t DHT_OK            = 0;
const uint8_t DHT_SEM_RESPOSTA  = 1;
const uint8_t DHT_ERRO_LEITURA  = 2;
const uint8_t DHT_ERRO_CHECKSUM = 3;

// Valor que indica que o laço desistiu de esperar (timeout).
const uint16_t DHT_TIMEOUT = 0xFFFF;

// Conta quantas voltas o pino fica no nível indicado.
// Desiste depois de um limite que equivale a bem mais de 1 ms.
// (F_CPU / 1000 = 16000 no UNO R3 e 48000 no UNO R4: cabe em 16 bits.)
uint16_t contarNivel(uint8_t nivel) {
  const uint16_t LIMITE = F_CPU / 1000;
  uint16_t contagem = 0;
  while (digitalRead(PINO_DHT11) == nivel) {
    if (++contagem >= LIMITE) {
      return DHT_TIMEOUT;
    }
  }
  return contagem;
}

uint8_t lerDht11(float &umidade, float &temperatura) {
  uint8_t dados[5] = {0, 0, 0, 0, 0};
  uint16_t ciclos[80];  // para cada bit: [LOW, HIGH]

  // Passo 1: sinal de início (LOW por 20 ms).
  pinMode(PINO_DHT11, OUTPUT);
  digitalWrite(PINO_DHT11, LOW);
  delay(20);

  // Solta a linha. O pull-up interno garante o HIGH mesmo se o resistor do
  // shield faltar. Esperamos 55 µs para cair no meio da resposta do sensor.
  pinMode(PINO_DHT11, INPUT_PULLUP);
  delayMicroseconds(55);

  noInterrupts();

  // Passo 2: resposta do sensor (LOW e depois HIGH de ~80 µs).
  if (contarNivel(LOW) == DHT_TIMEOUT || contarNivel(HIGH) == DHT_TIMEOUT) {
    interrupts();
    return DHT_SEM_RESPOSTA;
  }

  // Passo 3: guarda a duração de cada nível dos 40 bits. A conversão para
  // bits fica para depois, para não atrasar a contagem.
  for (uint8_t i = 0; i < 80; i += 2) {
    ciclos[i]     = contarNivel(LOW);
    ciclos[i + 1] = contarNivel(HIGH);
  }

  interrupts();

  // Converte as contagens em bits: HIGH mais longo que o LOW -> bit 1.
  for (uint8_t bit = 0; bit < 40; bit++) {
    uint16_t tempoLow  = ciclos[2 * bit];
    uint16_t tempoHigh = ciclos[2 * bit + 1];
    if (tempoLow == DHT_TIMEOUT || tempoHigh == DHT_TIMEOUT) {
      return DHT_ERRO_LEITURA;
    }
    dados[bit / 8] <<= 1;          // abre espaço para o próximo bit...
    if (tempoHigh > tempoLow) {
      dados[bit / 8] |= 1;         // ...e grava 1 se for o caso
    }
  }

  // Passo 4: confere a soma de verificação.
  uint8_t soma = dados[0] + dados[1] + dados[2] + dados[3];
  if (soma != dados[4]) {
    return DHT_ERRO_CHECKSUM;
  }

  umidade = dados[0] + dados[1] * 0.1;
  // No byte [3], o bit 7 indica temperatura negativa e os bits 0–3 são o
  // décimo de grau (usado pelas versões mais novas do DHT11).
  temperatura = dados[2] + (dados[3] & 0x0F) * 0.1;
  if (dados[3] & 0x80) {
    temperatura = -temperatura;
  }
  return DHT_OK;
}

// =============================================================================
//  LEITURA DO RECEPTOR INFRAVERMELHO (sem biblioteca)
// =============================================================================
//
//  O receptor IR do shield (tipo VS1838B) já faz todo o trabalho de
//  filtrar a luz infravermelha modulada em 38 kHz. Na saída dele (pino D6):
//    - HIGH = nada recebido (repouso)
//    - LOW  = recebendo o "pisca" de 38 kHz do controle remoto
//
//  A maioria dos controles baratos (inclusive os que vêm em kits Arduino)
//  usa o protocolo NEC:
//
//    LOW 9 ms + HIGH 4,5 ms       -> início de um comando ("líder")
//    32 bits, cada um:
//      LOW ~560 µs + HIGH ~560 µs -> bit 0
//      LOW ~560 µs + HIGH ~1690 µs -> bit 1
//    LOW 9 ms + HIGH 2,25 ms      -> "repetição" (botão mantido apertado)
//
//  Os 32 bits são enviados do bit menos significativo para o mais
//  significativo e formam 4 bytes:
//    endereço, endereço invertido, comando, comando invertido
//  O byte invertido serve para conferir se a recepção foi correta.
//
//  Controles de outros protocolos (Sony, RC5, TVs em geral) também são
//  detectados, mas não decodificados: o teste só informa que chegou sinal.

// Mede por quanto tempo (µs) o pino do IR fica no nível indicado.
// Retorna 0 se passar do tempo limite (timeoutUs).
unsigned long medirNivelIr(uint8_t nivel, unsigned long timeoutUs) {
  unsigned long inicio = micros();
  while (digitalRead(PINO_IR) == nivel) {
    if (micros() - inicio > timeoutUs) {
      return 0;
    }
  }
  return micros() - inicio;
}

// Espera a linha ficar em repouso (HIGH) por 20 ms seguidos, descartando o
// resto de um sinal que não conseguimos decodificar.
void esperarIrRepouso() {
  unsigned long inicio = millis();
  while (millis() - inicio < 20) {
    if (digitalRead(PINO_IR) == LOW) {
      inicio = millis();
    }
  }
}

// Resultados possíveis de lerIr()
const uint8_t IR_NADA       = 0;  // nenhum sinal
const uint8_t IR_NEC        = 1;  // comando NEC decodificado em "codigo"
const uint8_t IR_REPETICAO  = 2;  // botão NEC mantido apertado
const uint8_t IR_DESCONHECIDO = 3;  // sinal recebido, mas não é NEC

uint8_t lerIr(uint32_t &codigo, unsigned long &tempoLider) {
  if (digitalRead(PINO_IR) == HIGH) {
    return IR_NADA;
  }

  tempoLider = medirNivelIr(LOW, 15000);
  unsigned long espaco = medirNivelIr(HIGH, 6000);

  bool liderNec = (tempoLider > 8000 && tempoLider < 10000);

  if (liderNec && espaco > 2000 && espaco < 2600) {
    esperarIrRepouso();
    return IR_REPETICAO;
  }

  if (!liderNec || espaco < 4000 || espaco > 5000) {
    esperarIrRepouso();
    return IR_DESCONHECIDO;
  }

  codigo = 0;
  for (uint8_t i = 0; i < 32; i++) {
    unsigned long pulso = medirNivelIr(LOW, 1000);
    unsigned long pausa = medirNivelIr(HIGH, 2500);
    if (pulso == 0 || pausa == 0) {
      esperarIrRepouso();
      return IR_DESCONHECIDO;
    }
    if (pausa > 1000) {
      codigo |= (uint32_t)1 << i;  // bit menos significativo primeiro
    }
  }
  esperarIrRepouso();
  return IR_NEC;
}

// =============================================================================
//  TESTE 1: LEDs de 3 mm (D12 vermelho, D13 azul)
// =============================================================================
void testeLeds() {
  imprimirTitulo(F("TESTE 1: LEDs vermelho (D12) e azul (D13)"));
  Serial.println(F("O que observar: cada LED deve piscar 3 vezes, um de cada vez,"));
  Serial.println(F("e depois os dois juntos."));
  Serial.println(F("Obs.: o LED azul (D13) também é o LED embutido da placa"));
  Serial.println(F("(LED_BUILTIN), então o LED 'L' da placa pisca junto com ele."));

  Serial.println(F("-> LED vermelho (D12) piscando..."));
  for (uint8_t i = 0; i < 3; i++) {
    digitalWrite(PINO_LED_VERMELHO, HIGH);
    delay(300);
    digitalWrite(PINO_LED_VERMELHO, LOW);
    delay(300);
  }

  Serial.println(F("-> LED azul (D13) piscando..."));
  for (uint8_t i = 0; i < 3; i++) {
    digitalWrite(PINO_LED_AZUL, HIGH);
    delay(300);
    digitalWrite(PINO_LED_AZUL, LOW);
    delay(300);
  }

  Serial.println(F("-> Os dois juntos por 1 segundo..."));
  digitalWrite(PINO_LED_VERMELHO, HIGH);
  digitalWrite(PINO_LED_AZUL, HIGH);
  delay(1000);
  digitalWrite(PINO_LED_VERMELHO, LOW);
  digitalWrite(PINO_LED_AZUL, LOW);

  Serial.println(F("Resultado: se algum LED não acendeu, confira se o shield está"));
  Serial.println(F("bem encaixado e se o LED não está queimado ou invertido."));
}

// =============================================================================
//  TESTE 2: LED RGB (D9, D10, D11)
// =============================================================================
void testeRgb() {
  imprimirTitulo(F("TESTE 2: LED RGB (D9, D10, D11)"));

  // Parte A: descobrir qual cor está em cada pino.
  Serial.println(F("Parte A: um pino de cada vez. Anote a cor de cada pino."));
  for (uint8_t i = 0; i < 3; i++) {
    rgbApagar();
    rgbEscrever(PINOS_RGB[i], 255);
    Serial.print(F("-> D"));
    Serial.print(PINOS_RGB[i]);
    Serial.println(F(" aceso. Qual cor apareceu?"));
    esperarEnter();
  }

  // Parte B: conferir se é cátodo comum ou ânodo comum.
  rgbApagar();
  Serial.println(F("Parte B: o LED RGB deve estar APAGADO agora."));
  Serial.println(F("   Se ele estiver aceso (branco), o LED é de ânodo comum:"));
  Serial.println(F("   altere RGB_ANODO_COMUM para true no início do código."));
  esperarEnter();

  // Parte C: todas as cores juntas formam o branco.
  Serial.println(F("Parte C: as três cores juntas (deve ficar branco)."));
  for (uint8_t i = 0; i < 3; i++) {
    rgbEscrever(PINOS_RGB[i], 255);
  }
  delay(1500);
  rgbApagar();

  // Parte D: PWM. Os pinos D9, D10 e D11 aceitam analogWrite(), que
  // controla o brilho ligando e desligando o pino muito rápido. Com isso
  // cada cor pode ter 256 níveis de brilho (0 a 255).
  Serial.println(F("Parte D: brilho variando (PWM) em cada cor. O brilho deve"));
  Serial.println(F("   subir e descer suavemente, sem saltos."));
  for (uint8_t i = 0; i < 3; i++) {
    Serial.print(F("-> D"));
    Serial.println(PINOS_RGB[i]);
    for (int brilho = 0; brilho <= 255; brilho += 5) {
      rgbEscrever(PINOS_RGB[i], brilho);
      delay(10);
    }
    for (int brilho = 255; brilho >= 0; brilho -= 5) {
      rgbEscrever(PINOS_RGB[i], brilho);
      delay(10);
    }
  }
  rgbApagar();

  Serial.println(F("Resultado: registre no README do shield qual cor fica em cada pino."));
}

// =============================================================================
//  TESTE 3: Botões SW1 (D2) e SW2 (D3)
// =============================================================================
//
//  O botão só funciona como entrada se, com ele solto, o pino ficar num
//  nível definido (e não "flutuando"). Quem define esse nível é um resistor
//  de pull-up (solto = HIGH, apertado = LOW) ou de pull-down (solto = LOW,
//  apertado = HIGH). O shield tem resistores próprios ao lado dos botões.
//
//  Para não depender de saber qual dos dois foi usado, o teste lê o nível
//  dos botões soltos no início ("repouso") e considera apertado o nível
//  oposto. Por isso, NÃO aperte os botões no início do teste.
//
//  Debounce: ao apertar, os contatos metálicos do botão "trepidam" por
//  alguns milissegundos, gerando vários HIGH/LOW seguidos. O teste só
//  aceita uma mudança depois que o nível fica estável por 30 ms.

void testeBotoes() {
  imprimirTitulo(F("TESTE 3: Botões SW1 (D2) e SW2 (D3)"));
  Serial.println(F("NÃO aperte os botões agora: lendo o nível de repouso..."));
  delay(500);

  // Sem INPUT_PULLUP: o shield já tem resistores externos.
  pinMode(PINO_SW1, INPUT);
  pinMode(PINO_SW2, INPUT);

  const uint8_t pinos[2] = {PINO_SW1, PINO_SW2};
  uint8_t repouso[2];
  uint8_t estadoEstavel[2];
  uint8_t ultimaLeitura[2];
  unsigned long ultimaMudanca[2] = {0, 0};
  unsigned int contagem[2] = {0, 0};
  const unsigned long TEMPO_DEBOUNCE = 30;

  for (uint8_t b = 0; b < 2; b++) {
    repouso[b] = digitalRead(pinos[b]);
    estadoEstavel[b] = repouso[b];
    ultimaLeitura[b] = repouso[b];
    Serial.print(F("SW"));
    Serial.print(b + 1);
    Serial.print(F(" em repouso = "));
    if (repouso[b] == HIGH) {
      Serial.println(F("HIGH (pull-up: apertado vai para LOW)"));
    } else {
      Serial.println(F("LOW (pull-down: apertado vai para HIGH)"));
    }
  }

  Serial.println(F("Agora aperte e solte SW1 e SW2. Cada aperto aparece aqui"));
  Serial.println(F("e acende um LED: SW1 = vermelho, SW2 = azul."));
  Serial.println(F("Pressione Enter para voltar ao menu."));

  const uint8_t leds[2] = {PINO_LED_VERMELHO, PINO_LED_AZUL};

  while (!usuarioPediuParar()) {
    for (uint8_t b = 0; b < 2; b++) {
      uint8_t leitura = digitalRead(pinos[b]);

      // Qualquer mudança reinicia o cronômetro do debounce.
      if (leitura != ultimaLeitura[b]) {
        ultimaLeitura[b] = leitura;
        ultimaMudanca[b] = millis();
      }

      // Só aceita o novo nível depois de estável por TEMPO_DEBOUNCE.
      if (millis() - ultimaMudanca[b] > TEMPO_DEBOUNCE && leitura != estadoEstavel[b]) {
        estadoEstavel[b] = leitura;
        bool apertado = (leitura != repouso[b]);
        digitalWrite(leds[b], apertado ? HIGH : LOW);

        Serial.print(F("SW"));
        Serial.print(b + 1);
        if (apertado) {
          contagem[b]++;
          Serial.print(F(" APERTADO  (total: "));
          Serial.print(contagem[b]);
          Serial.println(F(")"));
        } else {
          Serial.println(F(" solto"));
        }
      }
    }
  }

  Serial.println(F("Resultado: cada aperto deve contar exatamente 1. Se um único"));
  Serial.println(F("aperto contar várias vezes, o botão está com mau contato."));
}

// =============================================================================
//  TESTE 4: Buzzer (D5)
// =============================================================================
//
//  Existem dois tipos de buzzer, e o teste ajuda a identificar qual está no
//  shield:
//    - ATIVO: tem um oscilador interno. Basta ligá-lo (nível fixo) para
//      apitar, sempre na mesma frequência.
//    - PASSIVO: é só um alto-falante pequeno. Com nível fixo ele não apita
//      (só dá um "clique"); precisa de uma onda quadrada, gerada por tone().
//
//  O buzzer é acionado por um transistor. Dependendo do transistor (NPN ou
//  PNP), ele liga com o pino em HIGH ou em LOW. A parte A descobre isso.

void testeBuzzer() {
  imprimirTitulo(F("TESTE 4: Buzzer (D5)"));

  Serial.println(F("Parte A: nível fixo no pino, 1 segundo em cada nível."));
  Serial.println(F("-> Pino D5 em HIGH..."));
  digitalWrite(PINO_BUZZER, HIGH);
  delay(1000);
  Serial.println(F("-> Pino D5 em LOW..."));
  digitalWrite(PINO_BUZZER, LOW);
  delay(1000);
  buzzerLigar(false);

  Serial.println(F("   Em qual nível o buzzer apitou?"));
  Serial.println(F("   - HIGH: use BUZZER_NIVEL_LIGADO = HIGH (é um buzzer ATIVO)."));
  Serial.println(F("   - LOW : use BUZZER_NIVEL_LIGADO = LOW  (é um buzzer ATIVO)."));
  Serial.println(F("   - Só um clique nos dois: é um buzzer PASSIVO."));
  Serial.print(F("   Valor atual no código: BUZZER_NIVEL_LIGADO = "));
  Serial.println(BUZZER_NIVEL_LIGADO == HIGH ? F("HIGH") : F("LOW"));
  esperarEnter();

  // Parte B: tone() gera uma onda quadrada na frequência pedida. No buzzer
  // passivo, cada frequência vira uma nota diferente. No ativo, o som sai
  // "rouco" e quase igual em todas as notas.
  Serial.println(F("Parte B: escala musical com tone() (dó, ré, mi, fá, sol, lá, si, dó)."));
  const unsigned int notas[8] = {262, 294, 330, 349, 392, 440, 494, 523};
  for (uint8_t i = 0; i < 8; i++) {
    tone(PINO_BUZZER, notas[i]);
    delay(250);
  }
  noTone(PINO_BUZZER);
  buzzerLigar(false);  // noTone() deixa o pino em LOW; volta ao "desligado"

  Serial.println(F("   Buzzer PASSIVO: as notas soam claramente diferentes."));
  Serial.println(F("   Buzzer ATIVO: som rouco, quase igual em todas as notas."));
  Serial.println(F("Resultado: registre no README do shield se o buzzer é ativo ou"));
  Serial.println(F("passivo e em qual nível ele liga."));
}

// =============================================================================
//  TESTE 5: Potenciômetro (A0)
// =============================================================================
//
//  O potenciômetro é um divisor de tensão ajustável: o terminal do meio
//  (cursor) entrega uma tensão entre 0V e 5V conforme a posição do eixo. O
//  ADC converte essa tensão em um número de 0 a 1023.

void testePotenciometro() {
  imprimirTitulo(F("TESTE 5: Potenciômetro (A0)"));
  Serial.println(F("Gire o potenciômetro de um extremo ao outro, devagar."));
  Serial.println(F("A leitura deve ir de ~0 a ~1023, sem saltos."));
  Serial.println(F("O brilho do LED RGB (D9) acompanha o potenciômetro."));
  Serial.println(F("Pressione Enter para voltar ao menu."));

  int minimo = 1023;
  int maximo = 0;

  while (!usuarioPediuParar()) {
    int leitura = analogRead(PINO_POT);
    minimo = min(minimo, leitura);
    maximo = max(maximo, leitura);

    // map() converte a faixa 0–1023 do ADC para a faixa 0–255 do PWM.
    rgbEscrever(PINO_RGB_D9, map(leitura, 0, 1023, 0, 255));

    Serial.print(F("A0 = "));
    Serial.print(leitura);
    Serial.print(F("\t"));
    Serial.print(adcParaVolts(leitura), 2);
    Serial.print(F(" V\t"));
    imprimirBarra(leitura / ADC_MAXIMO);
    Serial.print(F("  faixa vista: "));
    Serial.print(minimo);
    Serial.print(F(" a "));
    Serial.println(maximo);
    delay(200);
  }
  rgbApagar();

  Serial.print(F("Resultado: faixa vista de "));
  Serial.print(minimo);
  Serial.print(F(" a "));
  Serial.print(maximo);
  Serial.println(F(". O esperado é chegar perto de 0 e de 1023."));
}

// =============================================================================
//  TESTE 6: LDR — sensor de luminosidade (A1)
// =============================================================================
//
//  O LDR é um resistor cuja resistência cai quando recebe luz. No shield,
//  ele forma um divisor de tensão com um resistor fixo. Conforme o lado em
//  que o LDR está no divisor, a leitura SOBE ou DESCE com mais luz — o teste
//  mostra qual é o caso deste shield.

void testeLdr() {
  imprimirTitulo(F("TESTE 6: LDR - sensor de luminosidade (A1)"));
  Serial.println(F("1. Deixe o LDR na luz ambiente."));
  Serial.println(F("2. Cubra o LDR com o dedo (escuro)."));
  Serial.println(F("3. Aponte a lanterna do celular para ele (claro)."));
  Serial.println(F("Pressione Enter para voltar ao menu."));

  int minimo = 1023;
  int maximo = 0;

  while (!usuarioPediuParar()) {
    int leitura = (int)lerAnalogicoMedio(PINO_LDR);
    minimo = min(minimo, leitura);
    maximo = max(maximo, leitura);

    Serial.print(F("A1 = "));
    Serial.print(leitura);
    Serial.print(F("\t"));
    Serial.print(adcParaVolts(leitura), 2);
    Serial.print(F(" V\t"));
    imprimirBarra(leitura / ADC_MAXIMO);
    Serial.println();
    delay(250);
  }

  Serial.print(F("Resultado: faixa vista de "));
  Serial.print(minimo);
  Serial.print(F(" a "));
  Serial.print(maximo);
  Serial.println(F("."));
  Serial.println(F("A diferença entre escuro e claro deve ser grande (centenas)."));
  Serial.println(F("Anote se a leitura SOBE ou DESCE com mais luz."));
}

// =============================================================================
//  TESTE 7: LM35 — sensor de temperatura (A2)
// =============================================================================
//
//  O LM35 entrega 10 mV para cada grau Celsius: 0,25 V = 25 °C.
//  Logo: temperatura (°C) = tensão (V) × 100.
//
//  Com o ADC de 10 bits e referência de 5V, cada passo do ADC vale
//  5 V / 1023 ≈ 4,9 mV, ou seja, cerca de 0,5 °C. Por isso a leitura "pula"
//  de meio em meio grau. A precisão também depende de TENSAO_REFERENCIA:
//  se a placa estiver recebendo 4,7V da USB e o código usar 5,0V, a
//  temperatura aparece ~6% acima da real.

void testeLm35() {
  imprimirTitulo(F("TESTE 7: LM35 - temperatura (A2)"));
  Serial.println(F("A temperatura deve ficar próxima à do ambiente (20 a 30 °C)."));
  Serial.println(F("Segure o LM35 entre os dedos: a temperatura deve subir."));
  Serial.print(F("TENSAO_REFERENCIA no código: "));
  Serial.print(TENSAO_REFERENCIA, 2);
  Serial.println(F(" V (meça a tensão do pino 5V e ajuste, se preciso)."));
  Serial.println(F("Pressione Enter para voltar ao menu."));

  while (!usuarioPediuParar()) {
    float leitura = lerAnalogicoMedio(PINO_LM35);
    float volts = adcParaVolts(leitura);
    float celsius = volts * 100.0;

    Serial.print(F("A2 = "));
    Serial.print(leitura, 1);
    Serial.print(F("\t"));
    Serial.print(volts * 1000.0, 0);
    Serial.print(F(" mV\t"));
    Serial.print(celsius, 1);
    Serial.println(F(" °C"));
    delay(1000);
  }

  Serial.println(F("Resultado: compare com a temperatura do DHT11 (teste 8)."));
  Serial.println(F("Diferenças de 1 a 3 °C entre os dois sensores são normais."));
}

// =============================================================================
//  TESTE 8: DHT11 — temperatura e umidade (D4)
// =============================================================================
void imprimirErroDht(uint8_t erro) {
  switch (erro) {
    case DHT_SEM_RESPOSTA:
      Serial.println(F("ERRO: o DHT11 não respondeu. Confira o encaixe do shield."));
      break;
    case DHT_ERRO_LEITURA:
      Serial.println(F("ERRO: a transmissão foi interrompida no meio."));
      break;
    case DHT_ERRO_CHECKSUM:
      Serial.println(F("ERRO: soma de verificação não confere (dado corrompido)."));
      break;
  }
}

void testeDht11() {
  imprimirTitulo(F("TESTE 8: DHT11 - temperatura e umidade (D4)"));
  Serial.println(F("Uma leitura a cada 2 segundos (o DHT11 não aceita leituras"));
  Serial.println(F("mais rápidas que 1 por segundo)."));
  Serial.println(F("Sopre no sensor: a umidade deve subir em poucos segundos."));
  Serial.println(F("Pressione Enter para voltar ao menu."));

  unsigned long ultimaLeitura = 0;
  bool primeira = true;

  while (!usuarioPediuParar()) {
    if (primeira || millis() - ultimaLeitura >= 2000) {
      primeira = false;
      ultimaLeitura = millis();

      float umidade, temperatura;
      uint8_t resultado = lerDht11(umidade, temperatura);
      if (resultado == DHT_OK) {
        Serial.print(F("Umidade: "));
        Serial.print(umidade, 0);
        Serial.print(F(" %\tTemperatura: "));
        Serial.print(temperatura, 1);
        Serial.println(F(" °C"));
      } else {
        imprimirErroDht(resultado);
      }
    }
  }

  Serial.println(F("Resultado: umidade entre 20% e 90% e temperatura ambiente."));
  Serial.println(F("Um erro isolado é normal; erros seguidos indicam problema."));
}

// =============================================================================
//  TESTE 9: Receptor infravermelho (D6)
// =============================================================================
void testeIr() {
  imprimirTitulo(F("TESTE 9: Receptor infravermelho (D6)"));
  Serial.println(F("Aponte um controle remoto para o receptor e aperte botões."));
  Serial.println(F("Cada sinal recebido pisca o LED azul (D13)."));
  Serial.println(F("Controles NEC (a maioria dos de kits Arduino) mostram o código"));
  Serial.println(F("de cada botão; outros controles mostram só 'sinal recebido'."));
  Serial.println(F("Pressione Enter para voltar ao menu."));

  while (!usuarioPediuParar()) {
    uint32_t codigo = 0;
    unsigned long tempoLider = 0;
    uint8_t resultado = lerIr(codigo, tempoLider);

    if (resultado == IR_NADA) {
      continue;
    }

    digitalWrite(PINO_LED_AZUL, HIGH);

    if (resultado == IR_NEC) {
      uint8_t endereco         = codigo & 0xFF;
      uint8_t enderecoInvertido = (codigo >> 8) & 0xFF;
      uint8_t comando          = (codigo >> 16) & 0xFF;
      uint8_t comandoInvertido = (codigo >> 24) & 0xFF;

      Serial.print(F("NEC  código bruto: 0x"));
      Serial.print(codigo, HEX);
      Serial.print(F("\tendereço: 0x"));
      Serial.print(endereco, HEX);
      Serial.print(F("\tcomando: 0x"));
      Serial.print(comando, HEX);

      // O comando invertido deve ser o comando com todos os bits trocados.
      // Se não for, algum bit chegou errado (luz ambiente forte, controle
      // longe ou pilha fraca).
      if ((uint8_t)(comando ^ comandoInvertido) != 0xFF) {
        Serial.print(F("\t(ERRO: comando invertido não confere)"));
      }
      // Em alguns controles o endereço tem 16 bits (NEC estendido), e o
      // segundo byte não é o inverso do primeiro. Isso não é erro.
      if ((uint8_t)(endereco ^ enderecoInvertido) != 0xFF) {
        Serial.print(F("\t(endereço de 16 bits: 0x"));
        Serial.print((codigo & 0xFFFF), HEX);
        Serial.print(F(")"));
      }
      Serial.println();
    } else if (resultado == IR_REPETICAO) {
      Serial.println(F("NEC  repetição (botão mantido apertado)"));
    } else {
      Serial.print(F("Sinal recebido, mas não é NEC (primeiro pulso: "));
      Serial.print(tempoLider);
      Serial.println(F(" µs)"));
    }

    delay(50);
    digitalWrite(PINO_LED_AZUL, LOW);
  }

  Serial.println(F("Resultado: o mesmo botão deve gerar sempre o mesmo código."));
}

// =============================================================================
//  PAINEL AO VIVO: todos os sensores ao mesmo tempo
// =============================================================================
void painelAoVivo() {
  imprimirTitulo(F("PAINEL AO VIVO"));
  Serial.println(F("Todas as entradas, atualizadas a cada meio segundo."));
  Serial.println(F("Pressione Enter para voltar ao menu."));
  Serial.println(F("SW1  SW2  Pot   LDR   LM35(°C)  DHT11(°C)  Umid(%)"));

  pinMode(PINO_SW1, INPUT);
  pinMode(PINO_SW2, INPUT);

  float dhtTemperatura = 0;
  float dhtUmidade = 0;
  bool dhtValido = false;
  unsigned long ultimoDht = 0;
  bool primeiroDht = true;

  while (!usuarioPediuParar()) {
    // O DHT11 só pode ser lido a cada ~2 s; entre uma leitura e outra, o
    // painel mostra o último valor válido.
    if (primeiroDht || millis() - ultimoDht >= 2000) {
      primeiroDht = false;
      ultimoDht = millis();
      dhtValido = (lerDht11(dhtUmidade, dhtTemperatura) == DHT_OK);
    }

    Serial.print(digitalRead(PINO_SW1) == HIGH ? F("HIGH ") : F("LOW  "));
    Serial.print(digitalRead(PINO_SW2) == HIGH ? F("HIGH ") : F("LOW  "));
    Serial.print(analogRead(PINO_POT));
    Serial.print(F("\t"));
    Serial.print((int)lerAnalogicoMedio(PINO_LDR));
    Serial.print(F("\t"));
    Serial.print(adcParaVolts(lerAnalogicoMedio(PINO_LM35)) * 100.0, 1);
    Serial.print(F("\t  "));
    if (dhtValido) {
      Serial.print(dhtTemperatura, 1);
      Serial.print(F("\t     "));
      Serial.println(dhtUmidade, 0);
    } else {
      Serial.println(F("erro\t     erro"));
    }
    delay(500);
  }
}

// =============================================================================
//  TODOS OS TESTES EM SEQUÊNCIA
// =============================================================================
void testarTodos() {
  imprimirTitulo(F("TODOS OS TESTES EM SEQUÊNCIA"));
  Serial.println(F("Nos testes contínuos, pressione Enter para ir ao próximo."));
  testeLeds();
  testeRgb();
  testeBotoes();
  testeBuzzer();
  testePotenciometro();
  testeLdr();
  testeLm35();
  testeDht11();
  testeIr();
  imprimirTitulo(F("FIM DOS TESTES"));
}

// =============================================================================
//  MENU
// =============================================================================
void imprimirMenu() {
  Serial.println();
  Serial.println(F("============================================================"));
  Serial.println(F("  TESTE DO SHIELD MULTIFUNÇÃO 9 EM 1"));
  Serial.println(F("============================================================"));
  Serial.println(F("  1 - LEDs vermelho (D12) e azul (D13)"));
  Serial.println(F("  2 - LED RGB (D9, D10, D11)"));
  Serial.println(F("  3 - Botões SW1 (D2) e SW2 (D3)"));
  Serial.println(F("  4 - Buzzer (D5)"));
  Serial.println(F("  5 - Potenciômetro (A0)"));
  Serial.println(F("  6 - LDR / luminosidade (A1)"));
  Serial.println(F("  7 - LM35 / temperatura (A2)"));
  Serial.println(F("  8 - DHT11 / temperatura e umidade (D4)"));
  Serial.println(F("  9 - Receptor infravermelho (D6)"));
  Serial.println(F("  0 - Todos os testes em sequência"));
  Serial.println(F("  p - Painel ao vivo (todas as entradas)"));
  Serial.println(F("------------------------------------------------------------"));
  Serial.println(F("Digite a opção e pressione Enter:"));
}

// =============================================================================
//  SETUP E LOOP
// =============================================================================
void setup() {
  // O buzzer é configurado PRIMEIRO, e o nível "desligado" é escrito ANTES
  // do pinMode(OUTPUT). Um pino recém-configurado como saída começa em LOW:
  // se o buzzer ligar em LOW (transistor PNP), ele daria um apito ao ligar
  // a placa.
  buzzerLigar(false);
  pinMode(PINO_BUZZER, OUTPUT);

  pinMode(PINO_LED_VERMELHO, OUTPUT);
  pinMode(PINO_LED_AZUL, OUTPUT);
  for (uint8_t i = 0; i < 3; i++) {
    pinMode(PINOS_RGB[i], OUTPUT);
  }
  pinMode(PINO_SW1, INPUT);
  pinMode(PINO_SW2, INPUT);
  pinMode(PINO_IR, INPUT);
  pinMode(PINO_DHT11, INPUT_PULLUP);
  desligarTudo();

  Serial.begin(VELOCIDADE_SERIAL);

  // No UNO R4, a serial passa pela USB nativa, que leva um instante para
  // ficar pronta; sem essa espera, as primeiras mensagens se perdem. No UNO
  // R3, "Serial" é sempre verdadeiro e a espera termina na hora. O limite
  // de 3 s evita travar a placa se ela estiver sem computador.
  unsigned long inicio = millis();
  while (!Serial && millis() - inicio < 3000) {
    // espera
  }

  imprimirMenu();
}

void loop() {
  if (Serial.available() == 0) {
    return;
  }

  char opcao = Serial.read();

  // Ignora os caracteres de final de linha enviados pelo Monitor Serial.
  if (opcao == '\n' || opcao == '\r' || opcao == ' ') {
    return;
  }
  limparSerial();

  switch (opcao) {
    case '1': testeLeds();          break;
    case '2': testeRgb();           break;
    case '3': testeBotoes();        break;
    case '4': testeBuzzer();        break;
    case '5': testePotenciometro(); break;
    case '6': testeLdr();           break;
    case '7': testeLm35();          break;
    case '8': testeDht11();         break;
    case '9': testeIr();            break;
    case '0': testarTodos();        break;
    case 'p':
    case 'P': painelAoVivo();       break;
    default:
      Serial.print(F("Opção inválida: "));
      Serial.println(opcao);
      break;
  }

  desligarTudo();
  imprimirMenu();
}
