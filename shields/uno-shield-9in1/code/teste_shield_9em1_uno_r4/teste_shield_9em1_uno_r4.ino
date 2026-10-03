/*
 * =============================================================================
 *  Teste dos periféricos do Shield Multifunção 9 em 1 — versão UNO R4
 * =============================================================================
 *
 *  Autor: Prof. Joao Miguel Roehe (@professorjoaomiguel)
 *  Licença: MIT (SPDX-License-Identifier: MIT) — ver LICENSE-CODE na raiz
 *
 *  Placa: Arduino UNO R4 (Minima e WiFi). Para o UNO R3, use a versão
 *  ../teste_shield_9em1_uno_r3. Diferença: a leitura do LM35 liga a
 *  descarga do capacitor de amostragem do ADC (ver lerLm35()).
 *  NÃO use este shield em placas de 3,3V (ex: ESP32): ele é de 5V e pode
 *  queimar as portas do microcontrolador. Ver ../../README.md.
 *
 *  SINAL DE FIRMWARE GRAVADO: no menu, o LED "L" (D13) pisca duas vezes
 *  rápidas a cada 2 s.
 *
 *  TESTE CONJUNTO (placa + shield): informa sobre as duas. No início
 *  mostra a placa (modelo e tensão de referência); a opção "a" do menu
 *  roda um teste automático do shield com RESUMO, e os testes 1 a 9 são
 *  guiados (ver, ouvir, mexer). Para testar só a placa, use os sketches da
 *  pasta da placa (boards/.../code).
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
 *    D9   | LED RGB — vermelho
 *    D10  | LED RGB — azul
 *    D11  | LED RGB — verde
 *    D12  | LED vermelho de 3 mm (LED2 na placa) — padrão; a cor pode variar
 *    D13  | LED azul de 3 mm (LED1 na placa) — idem; também é o LED_BUILTIN
 *    A0   | Potenciômetro
 *    A1   | LDR (sensor de luminosidade)
 *    A2   | Sensor de temperatura LM35
 *
 *  COMO USAR
 *  ---------
 *  1. Encaixe o shield no UNO R4 e ligue a placa na USB.
 *  2. Na IDE do Arduino, escolha a placa certa (Ferramentas > Placa) e a
 *     porta COM, e grave este sketch.
 *  3. Abra o Monitor Serial e configure:
 *       - velocidade: 115200 baud (a mesma dos testes da placa)
 *       - final de linha: qualquer opção funciona
 *  4. Digite o número do teste e envie. Quando um teste pedir para
 *     continuar ou voltar ao menu, envie "c" ou aperte o SW1 do shield
 *     (exceto no teste dos botões e no painel, em que só vale o "c"). Com "Nova linha", só o Enter
 *     também serve; com "Sem final de linha", o Enter com a caixa vazia
 *     não envia nada, por isso as instruções pedem a letra "c".
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
 *  REFERÊNCIAS
 *  -----------
 *  O shield é um clone do Keyestudio Easy Module Shield V1 (KS0183):
 *    https://wiki.keyestudio.com/Ks0183_keyestudio_Multi-purpose_Shield_V1
 *
 *  Repositório: https://github.com/professorjoaomiguel/lab-boards
 * =============================================================================
 */

#if !defined(ARDUINO_ARCH_RENESAS_UNO)
#error "Esta versão é para o UNO R4. Para o UNO R3, use teste_shield_9em1_uno_r3."
#endif

// =============================================================================
//  PINOS
//  Os números seguem a serigrafia impressa no shield.
// =============================================================================
const uint8_t PINO_SW1          = 2;
const uint8_t PINO_SW2          = 3;
const uint8_t PINO_DHT11        = 4;
const uint8_t PINO_BUZZER       = 5;
const uint8_t PINO_IR           = 6;
const uint8_t PINO_RGB_VERMELHO = 9;
const uint8_t PINO_RGB_AZUL     = 10;
const uint8_t PINO_RGB_VERDE    = 11;
// LEDs de 3 mm: no padrão, D12 é VERMELHO e D13 é AZUL. Mas há variação de
// montagem entre lotes, por isso o nome é o pino, e não a cor.
const uint8_t PINO_LED_D12      = 12;
const uint8_t PINO_LED_D13      = 13;
const uint8_t PINO_POT          = A0;
const uint8_t PINO_LDR          = A1;
const uint8_t PINO_LM35         = A2;

// Cores do LED RGB: CONFIRMADAS em teste na placa (D9 = vermelho,
// D10 = azul, D11 = verde, ativo em HIGH).
// Atenção: a documentação da Keyestudio e da RoboticX diz D10 = verde e
// D11 = azul, o contrário do que foi medido. Clones podem trocar a posição
// do LED; se as cores do seu shield não baterem, o teste 2 mostra a cor
// real de cada pino.
//
// Os três pinos, na ordem em que o teste 2 os acende (vermelho, verde, azul):
const uint8_t PINOS_RGB[3] = {PINO_RGB_VERMELHO, PINO_RGB_VERDE, PINO_RGB_AZUL};

// =============================================================================
//  AJUSTES
// =============================================================================

// Velocidade da comunicação serial. Deve ser igual à do Monitor Serial.
const char VERSAO[] = "2";
const unsigned long VELOCIDADE_SERIAL = 115200;

// Tipo do LED RGB:
//   false = cátodo comum: o pino em HIGH acende a cor. CONFIRMADO neste
//           shield (e também indicado pela Keyestudio e pela RoboticX).
//   true  = ânodo comum: o pino em LOW acende a cor.
// Se no teste 2 o LED ficar ACESO quando o texto disser "apagado" (e
// apagado quando disser "aceso"), troque este valor.
const bool RGB_ANODO_COMUM = false;

// Nível do pino D5 que liga o transistor do buzzer:
//   HIGH = transistor NPN. É o caso deste shield: o multímetro mostrou um
//          NPN (2026-10-01), e com LOW como "ligado" um shield apitou sem
//          parar, porque o sketch deixava o D5 em HIGH para "desligar"
//          (UNO R4, 2026-10-03).
//   LOW  = transistor PNP (ex: S8550). É o que diz a Keyestudio, fabricante
//          do projeto original (no código dela, LOW = som), mas não bate
//          com os shields medidos aqui.
// Com o valor errado, o transistor fica conduzindo o tempo todo: um buzzer
// ativo apita sem parar, e um passivo fica com corrente contínua passando
// pela bobina (esquenta à toa). O teste 4 ajuda a confirmar.
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
//  ENTRADA PELO SHIELD: SW1 = continuar / voltar ao menu
// =============================================================================
//
//  Além do Monitor Serial, o aluno pode responder no próprio shield,
//  apertando o SW1. A serial continua valendo sempre. Um SW1 que já está em
//  LOW sem ninguém apertar (travado ou em curto) é desativado como entrada:
//  senão ele "responderia" sozinho.
bool sw1Ativo = false;

void prepararBotoes() {
  pinMode(PINO_SW1, INPUT);
  delay(2);
  sw1Ativo = digitalRead(PINO_SW1) == HIGH;  // solto = HIGH (pull-up do shield)
}

// Devolve true se o SW1 foi apertado E solto. Só conta depois de soltar,
// para um aperto não valer duas vezes seguidas.
bool sw1Clicado() {
  if (!sw1Ativo || digitalRead(PINO_SW1) == HIGH) return false;
  delay(30);  // debounce: confirma que continua apertado
  if (digitalRead(PINO_SW1) == HIGH) return false;
  while (digitalRead(PINO_SW1) == LOW) {
    // espera soltar
  }
  delay(30);
  return true;
}

// Sinal de "firmware de teste gravado": enquanto espera no menu, o LED do
// D13 (o LED "L" da placa, e o azul do shield) pisca duas vezes rápidas a
// cada 2 s, um "tum-tum" diferente do Blink comum (1 s aceso, 1 s apagado).
void sinalizarEspera() {
  unsigned long t = millis() % 2000;
  bool aceso = t < 100 || (t >= 250 && t < 350);
  digitalWrite(PINO_LED_D13, aceso ? HIGH : LOW);
}

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
//
// Funciona com qualquer opção de final de linha do Monitor Serial: os 20 ms
// de espera bastam para chegar o resto da mensagem, inclusive o "\r\n" da
// opção "Ambos, NL e CR" (a 115200 baud, cada caractere leva ~0,1 ms).
void limparSerial() {
  delay(20);
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
  return sw1Clicado();
}

// Igual à anterior, mas sem o SW1: para os testes em que o próprio botão
// é o que está sendo testado (teste 3 e painel ao vivo).
bool usuarioPediuPararSoSerial() {
  if (Serial.available() > 0) {
    limparSerial();
    return true;
  }
  return false;
}

void imprimirComoContinuar() {
  if (sw1Ativo) {
    Serial.println(F("   (envie c ou aperte SW1 para continuar)"));
  } else {
    Serial.println(F("   (envie c para continuar)"));
  }
}

// Fica parado até o usuário enviar algo pelo Monitor Serial ou apertar SW1.
void esperarContinuar() {
  imprimirComoContinuar();
  while (!usuarioPediuParar()) {
    // espera
  }
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
  digitalWrite(PINO_LED_D12, LOW);
  digitalWrite(PINO_LED_D13, LOW);
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

// Lê o LM35 (média de 16 leituras).
//
// O capacitor de amostragem do ADC do RA4M1 guarda a tensão do último pino
// lido, e o LM35 quase não consegue absorver corrente: depois de ler o LDR
// (~4,5V), o A2 chegou a marcar 0,77 V com 0,253 V reais no multímetro. O
// registrador ADDISCR descarrega esse capacitor antes de cada conversão e
// resolve. Explicação completa no README do UNO R4, seção "ADC: leitura
// errada de sensores que não absorvem corrente".
float lerLm35() {
  R_ADC0->ADDISCR = 0x0F;
  return lerAnalogicoMedio(PINO_LM35);
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

  // O DHT11 responde com a medição ANTERIOR. Na 1ª leitura depois de ligar
  // ainda não há medição, e ele envia 5 bytes zerados, que "passam" na soma
  // de verificação (0+0+0+0 = 0). Visto num UNO R4: tratar como erro.
  // Também já veio 0 % e 0,4 °C (bytes 0, 0, 0, 4, 4), que somam certo mas
  // são impossíveis: o DHT11 mede de 20 a 90 % de umidade. Por isso,
  // umidade 0 = leitura inválida.
  if (dados[0] == 0) {
    return DHT_ERRO_LEITURA;
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
  imprimirTitulo(F("TESTE 1: LEDs de 3 mm (D12 e D13)"));
  Serial.println(F("O que observar: cada LED deve piscar 3 vezes, um de cada vez,"));
  Serial.println(F("e depois os dois juntos."));
  Serial.println(F("Padrão: D12 vermelho e D13 azul. Há variação de montagem:"));
  Serial.println(F("anote se no seu shield as cores vieram trocadas."));
  Serial.println(F("Obs.: o LED do D13 também é o LED embutido da placa"));
  Serial.println(F("(LED_BUILTIN), então o LED 'L' da placa pisca junto com ele."));

  Serial.println(F("-> LED do D12 piscando (vermelho, no padrão)..."));
  for (uint8_t i = 0; i < 3; i++) {
    digitalWrite(PINO_LED_D12, HIGH);
    delay(300);
    digitalWrite(PINO_LED_D12, LOW);
    delay(300);
  }

  Serial.println(F("-> LED do D13 piscando (azul, no padrão)..."));
  for (uint8_t i = 0; i < 3; i++) {
    digitalWrite(PINO_LED_D13, HIGH);
    delay(300);
    digitalWrite(PINO_LED_D13, LOW);
    delay(300);
  }

  Serial.println(F("-> Os dois juntos por 1 segundo..."));
  digitalWrite(PINO_LED_D12, HIGH);
  digitalWrite(PINO_LED_D13, HIGH);
  delay(1000);
  digitalWrite(PINO_LED_D12, LOW);
  digitalWrite(PINO_LED_D13, LOW);

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
  // Cor esperada em cada pino (confirmada em teste na placa).
  const __FlashStringHelper *corEsperada[3] = {F("vermelho"), F("verde"), F("azul")};
  for (uint8_t i = 0; i < 3; i++) {
    rgbApagar();
    rgbEscrever(PINOS_RGB[i], 255);
    Serial.print(F("-> D"));
    Serial.print(PINOS_RGB[i]);
    Serial.print(F(" aceso. Qual cor apareceu? (esperado: "));
    Serial.print(corEsperada[i]);
    Serial.println(F(")"));
    esperarContinuar();
  }

  // Parte B: conferir se é cátodo comum ou ânodo comum.
  rgbApagar();
  Serial.println(F("Parte B: o LED RGB deve estar APAGADO agora."));
  Serial.println(F("   Se ele estiver aceso (branco), o LED é de ânodo comum:"));
  Serial.println(F("   altere RGB_ANODO_COMUM para true no início do código."));
  esperarContinuar();

  // Parte C: todas as cores juntas formam o branco.
  Serial.println(F("Parte C: as três cores juntas. O LED deve parecer branco"));
  Serial.println(F("   (ou quase: num LED RGB barato, uma cor pode puxar mais)."));
  for (uint8_t i = 0; i < 3; i++) {
    rgbEscrever(PINOS_RGB[i], 255);
  }
  esperarContinuar();
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

  // Parte E: o potenciômetro (A0) vira o "botão de volume" do brilho.
  // analogRead() vai de 0 a 1023; dividir por 4 (>> 2) dá 0 a 255, a faixa
  // do analogWrite().
  Serial.println(F("Parte E: gire o potenciômetro (A0). O brilho das três cores"));
  Serial.println(F("   juntas deve acompanhar o giro, de apagado a máximo."));
  imprimirComoContinuar();
  while (!usuarioPediuParar()) {
    uint8_t brilho = analogRead(PINO_POT) >> 2;
    for (uint8_t i = 0; i < 3; i++) {
      rgbEscrever(PINOS_RGB[i], brilho);
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
//  Os exemplos da RoboticX tratam o botão apertado como HIGH, o que indica
//  pull-down. Ainda assim, para não depender disso, o teste lê o nível
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
      // Nos shields medidos, os botões têm pull-up: LOW sem ninguém apertar
      // indica botão travado ou em curto com o GND (visto num shield, no
      // SW2). Nesse caso o aperto não aparece no teste.
      Serial.println(F("LOW. ATENÇÃO: nos shields medidos, solto = HIGH. Se você não"));
      Serial.println(F("   está apertando, o botão está travado ou em curto com o GND."));
    }
  }

  Serial.println(F("Agora aperte e solte SW1 e SW2. Cada aperto aparece aqui, e o"));
  Serial.println(F("LED fica aceso enquanto o botão estiver apertado:"));
  Serial.println(F("SW1 (D2) -> LED azul (D13)   e   SW2 (D3) -> LED vermelho (D12)."));
  Serial.println(F("(cores do padrão; se o seu shield veio com as cores trocadas, vale o pino)"));
  Serial.println(F("Envie c para voltar ao menu."));

  // SW1 acende o LED azul e SW2 o vermelho (no padrão: azul = D13, vermelho = D12).
  const uint8_t leds[2] = {PINO_LED_D13, PINO_LED_D12};

  while (!usuarioPediuPararSoSerial()) {
    for (uint8_t b = 0; b < 2; b++) {
      uint8_t leitura = digitalRead(pinos[b]);

      // O LED segue o botão NA HORA, sem esperar o debounce: assim ele
      // acende no instante do aperto. O debounce abaixo só serve para
      // contar e escrever cada aperto uma vez.
      digitalWrite(leds[b], leitura != repouso[b] ? HIGH : LOW);

      // Qualquer mudança reinicia o cronômetro do debounce.
      if (leitura != ultimaLeitura[b]) {
        ultimaLeitura[b] = leitura;
        ultimaMudanca[b] = millis();
      }

      // Só aceita o novo nível depois de estável por TEMPO_DEBOUNCE.
      if (millis() - ultimaMudanca[b] > TEMPO_DEBOUNCE && leitura != estadoEstavel[b]) {
        estadoEstavel[b] = leitura;
        bool apertado = (leitura != repouso[b]);

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
//  shield. A Keyestudio diz PASSIVO, mas o shield testado em 2026-10-03
//  tinha um ATIVO (apitou sem parar com o D5 fixo em HIGH):
//    - ATIVO: tem um oscilador interno. Basta ligá-lo (nível fixo) para
//      apitar, sempre na mesma frequência.
//    - PASSIVO: é só um alto-falante pequeno. Com nível fixo ele não apita
//      (só dá um "clique"); precisa de uma onda quadrada, gerada por tone().
//
//  O buzzer é acionado por um transistor. Dependendo do transistor (NPN ou
//  PNP), ele liga com o pino em HIGH ou em LOW. Com buzzer ativo, a parte A
//  mostra o nível que liga. Com buzzer passivo, nenhum nível fixo apita
//  (só dá um clique na troca); nesse caso vale o transistor medido com
//  multímetro neste shield: NPN, HIGH liga.

// =============================================================================
//  MELODIA: "Nokia Tune"
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
  Serial.println(F("   - HIGH: use BUZZER_NIVEL_LIGADO = HIGH (é um buzzer ATIVO;"));
  Serial.println(F("     foi o caso no shield testado)."));
  Serial.println(F("   - LOW : use BUZZER_NIVEL_LIGADO = LOW  (é um buzzer ATIVO)."));
  Serial.println(F("   - Só um clique nos dois: é um buzzer PASSIVO. Mantenha"));
  Serial.println(F("     BUZZER_NIVEL_LIGADO = HIGH (transistor NPN medido)."));
  Serial.print(F("   Valor atual no código: BUZZER_NIVEL_LIGADO = "));
  Serial.println(BUZZER_NIVEL_LIGADO == HIGH ? F("HIGH") : F("LOW"));
  esperarContinuar();

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
  // Parte C: uma melodia conhecida. No buzzer passivo, as notas saem
  // limpas. No ativo, o tone() só liga e desliga o apito interno dele: o
  // ritmo aparece, mas as notas saem misturadas com o apito próprio.
  Serial.println(F("Parte C: melodia \"Nokia Tune\" (Tárrega, Gran Vals)."));
  tocarNokia(PINO_BUZZER);
  buzzerLigar(false);

  // Parte D: o potenciômetro (A0) escolhe a frequência do tone(). A nota só
  // é trocada quando a frequência muda mais de 20 Hz: chamar tone() o tempo
  // todo, mesmo sem mudança, faria o som "engasgar".
  Serial.println(F("Parte D: gire o potenciômetro (A0): a frequência do tone() vai"));
  Serial.println(F("   de 100 Hz a 5000 Hz. No buzzer passivo, a nota acompanha o giro;"));
  Serial.println(F("   no ativo (o dos shields testados), o apito próprio sai picotado."));
  imprimirComoContinuar();
  unsigned int frequenciaAtual = 0;
  unsigned long ultimoPrint = 0;
  while (!usuarioPediuParar()) {
    unsigned int frequencia = map(analogRead(PINO_POT), 0, 1023, 100, 5000);
    if (abs((int)frequencia - (int)frequenciaAtual) > 20) {
      tone(PINO_BUZZER, frequencia);
      frequenciaAtual = frequencia;
    }
    if (millis() - ultimoPrint > 500) {
      ultimoPrint = millis();
      Serial.print(F("   "));
      Serial.print(frequenciaAtual);
      Serial.println(F(" Hz"));
    }
  }
  noTone(PINO_BUZZER);
  buzzerLigar(false);

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
  Serial.println(F("O brilho do vermelho do LED RGB (D9) acompanha o potenciômetro."));
  Serial.println(F("Envie c (ou aperte SW1) para voltar ao menu."));

  int minimo = 1023;
  int maximo = 0;

  while (!usuarioPediuParar()) {
    int leitura = analogRead(PINO_POT);
    minimo = min(minimo, leitura);
    maximo = max(maximo, leitura);

    // map() converte a faixa 0–1023 do ADC para a faixa 0–255 do PWM.
    rgbEscrever(PINO_RGB_VERMELHO, map(leitura, 0, 1023, 0, 255));

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
//  que o LDR está no divisor, a leitura SOBE ou DESCE com mais luz. Segundo a
//  Keyestudio, neste shield ela SOBE com mais luz; o teste confirma.

void testeLdr() {
  imprimirTitulo(F("TESTE 6: LDR - sensor de luminosidade (A1)"));
  Serial.println(F("1. Deixe o LDR na luz ambiente."));
  Serial.println(F("2. Cubra o LDR com o dedo (escuro)."));
  Serial.println(F("3. Aponte a lanterna do celular para ele (claro)."));
  Serial.println(F("Envie c (ou aperte SW1) para voltar ao menu."));

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
  Serial.println(F("Anote se a leitura SOBE ou DESCE com mais luz (esperado: sobe)."));
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
  Serial.println(F("Envie c (ou aperte SW1) para voltar ao menu."));

  while (!usuarioPediuParar()) {
    float leitura = lerLm35();
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
  Serial.println(F("Envie c (ou aperte SW1) para voltar ao menu."));

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
  Serial.println(F("Cada sinal recebido pisca o LED do D13."));
  Serial.println(F("Controles NEC (a maioria dos de kits Arduino) mostram o código"));
  Serial.println(F("de cada botão; outros controles mostram só 'sinal recebido'."));
  Serial.println(F("Envie c (ou aperte SW1) para voltar ao menu."));

  while (!usuarioPediuParar()) {
    uint32_t codigo = 0;
    unsigned long tempoLider = 0;
    uint8_t resultado = lerIr(codigo, tempoLider);

    if (resultado == IR_NADA) {
      continue;
    }

    digitalWrite(PINO_LED_D13, HIGH);

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
    digitalWrite(PINO_LED_D13, LOW);
  }

  Serial.println(F("Resultado: o mesmo botão deve gerar sempre o mesmo código."));
}

// =============================================================================
//  PAINEL AO VIVO: todos os sensores ao mesmo tempo
// =============================================================================
void painelAoVivo() {
  imprimirTitulo(F("PAINEL AO VIVO"));
  Serial.println(F("Todas as entradas, atualizadas a cada meio segundo."));
  Serial.println(F("Envie c para voltar ao menu."));
  Serial.println(F("SW1  SW2  Pot   LDR   LM35(°C)  DHT11(°C)  Umid(%)"));

  pinMode(PINO_SW1, INPUT);
  pinMode(PINO_SW2, INPUT);

  float dhtTemperatura = 0;
  float dhtUmidade = 0;
  bool dhtValido = false;
  unsigned long ultimoDht = 0;
  bool primeiroDht = true;

  while (!usuarioPediuPararSoSerial()) {
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
    Serial.print(adcParaVolts(lerLm35()) * 100.0, 1);
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
//  RESULTADOS: linha para o script + resumo legível
// =============================================================================
const uint8_t OK = 0, FALHA = 1, AVISO = 2, PULADO = 3, INFO = 4;
unsigned int totalOk, totalFalha, totalAviso, totalPulado;

// Cada resultado sai na hora numa linha "RESULTADO;..." e fica guardado
// para o RESUMO do final. O nome do teste fica na flash (só o ponteiro é
// guardado); o detalhe é copiado para um buffer fixo.
struct Registro {
  const __FlashStringHelper *nome;
  uint8_t estado;
  char detalhe[32];
};
const uint8_t MAX_REGISTROS = 14;
Registro registros[MAX_REGISTROS];
uint8_t totalRegistros = 0;

void zerarResultados() {
  totalOk = totalFalha = totalAviso = totalPulado = 0;
  totalRegistros = 0;
}

const __FlashStringHelper *textoEstado(uint8_t estado) {
  switch (estado) {
    case OK:     return F("OK");
    case FALHA:  return F("FALHA");
    case AVISO:  return F("AVISO");
    case PULADO: return F("PULADO");
    default:     return F("INFO");
  }
}

// id: identificador curto para o script (ex: "dht11").
// nome: nome legível para o resumo (ex: "DHT11").
void resultado(const __FlashStringHelper *id, const __FlashStringHelper *nome,
               uint8_t estado, const char *detalhe) {
  Serial.print(F("RESULTADO;"));
  Serial.print(id);
  Serial.print(';');
  Serial.print(textoEstado(estado));
  Serial.print(';');
  Serial.println(detalhe);

  if (totalRegistros < MAX_REGISTROS) {
    Registro &r = registros[totalRegistros++];
    r.nome = nome;
    r.estado = estado;
    strncpy(r.detalhe, detalhe, sizeof(r.detalhe) - 1);
    r.detalhe[sizeof(r.detalhe) - 1] = '\0';
  }

  if (estado == OK) totalOk++;
  else if (estado == FALHA) totalFalha++;
  else if (estado == AVISO) totalAviso++;
  else if (estado == PULADO) totalPulado++;
}

// Caracteres que aparecem na tela de um texto da flash. Letras acentuadas
// ocupam 2 bytes em UTF-8; só o 1º conta (alinha os pontinhos do resumo).
uint8_t larguraNaTela(const __FlashStringHelper *texto) {
  const char *p = (const char *)texto;
  uint8_t largura = 0;
  for (char c = pgm_read_byte(p); c; c = pgm_read_byte(++p)) {
    if ((c & 0xC0) != 0x80) largura++;
  }
  return largura;
}

void imprimirResumo() {
  Serial.println();
  Serial.println(F("=============================================================="));
  Serial.println(F("  RESUMO"));
  Serial.println(F("=============================================================="));
  for (uint8_t i = 0; i < totalRegistros; i++) {
    switch (registros[i].estado) {
      case OK:     Serial.print(F("  [  OK  ] ")); break;
      case FALHA:  Serial.print(F("  [FALHA!] ")); break;
      case AVISO:  Serial.print(F("  [AVISO ] ")); break;
      case PULADO: Serial.print(F("  [pulado] ")); break;
      default:     Serial.print(F("  [ info ] ")); break;
    }
    Serial.print(registros[i].nome);
    Serial.print(' ');
    for (uint8_t p = larguraNaTela(registros[i].nome); p < 26; p++) Serial.print('.');
    Serial.print(' ');
    Serial.println(registros[i].detalhe);
  }
  Serial.println(F("--------------------------------------------------------------"));
  Serial.print(F("  OK: "));
  Serial.print(totalOk);
  Serial.print(F("   FALHA: "));
  Serial.print(totalFalha);
  Serial.print(F("   AVISO: "));
  Serial.print(totalAviso);
  Serial.print(F("   pulados: "));
  Serial.println(totalPulado);
  if (totalFalha > 0) {
    Serial.println(F("  Resultado: há FALHAS. Veja as linhas marcadas com [FALHA!]."));
  } else if (totalAviso > 0) {
    Serial.println(F("  Resultado: sem falhas, mas confira as linhas com [AVISO ]."));
  } else {
    Serial.println(F("  Resultado: tudo certo."));
  }
  Serial.println(F("=============================================================="));
}

// Resumo para o aluno e, por último, a linha FIM para o script.
void imprimirFim() {
  imprimirResumo();
  Serial.print(F("FIM;ok="));
  Serial.print(totalOk);
  Serial.print(F(";falha="));
  Serial.print(totalFalha);
  Serial.print(F(";aviso="));
  Serial.print(totalAviso);
  Serial.print(F(";pulado="));
  Serial.println(totalPulado);
  Serial.println(F("# Envie c para rodar de novo."));
}

// "D7", "A3"... (nome do pino como na serigrafia)
void nomePino(uint8_t pino, char *destino) {
  if (pino >= A0) {
    destino[0] = 'A';
    destino[1] = '0' + (pino - A0);
    destino[2] = '\0';
  } else {
    snprintf(destino, 4, "D%u", pino);
  }
}

// Acrescenta um texto ao fim do buffer, sem passar do tamanho.
void acrescentar(char *buffer, size_t tamanho, const char *texto) {
  strncat(buffer, texto, tamanho - strlen(buffer) - 1);
}

// =============================================================================
//  A PLACA: este é um teste CONJUNTO (placa + shield)
// =============================================================================

// ID único do RA4M1 (no Minima, é também o número de série da USB).
void idUnico(char *destino) {
  const bsp_unique_id_t *id = R_BSP_UniqueIdGet();
  for (uint8_t i = 0; i < 4; i++) {
    snprintf(destino + 8 * i, 9, "%08lX", (unsigned long)id->unique_id_words[i]);
  }
}

const char *modeloDaPlaca() {
#if defined(ARDUINO_MINIMA)
  return "UNO R4 Minima";
#elif defined(ARDUINO_UNOWIFIR4)
  return "UNO R4 WiFi";
#else
  return "UNO R4 (modelo desconhecido)";
#endif
}

// AVCC (referência do ADC) medida pela placa, em mV. O UNO R4 Minima mede
// a própria AVCC com analogReference() sem argumento; o WiFi não: supõe 5V.
long referenciaMilivolts() {
  float avcc = analogReference();
  if (isnan(avcc) || avcc < 3.0) return 5000;
  return lround(avcc * 1000);
}

// Mostra qual placa está sob o shield (no início e no teste automático).
void imprimirPlaca() {
  char id[33];
  idUnico(id);
  Serial.print(F("Placa: "));
  Serial.print(modeloDaPlaca());
  Serial.print(F(", ID "));
  Serial.print(id);
  Serial.print(F(", AVCC "));
  Serial.print(referenciaMilivolts());
  Serial.println(F(" mV"));
}

void resultadosDaPlaca() {
  char id[33], detalhe[32];
  idUnico(id);
  Serial.print(F("ID_UNICO;"));
  Serial.println(id);
  resultado(F("placa"), F("Placa"), INFO, modeloDaPlaca());
  long avcc = referenciaMilivolts();
  snprintf(detalhe, sizeof(detalhe), "%ld mV", avcc);
  resultado(F("avcc"), F("Referência do ADC"), (avcc >= 4500 && avcc <= 5250) ? OK : AVISO, detalhe);
}

// =============================================================================
//  TESTE AUTOMÁTICO DO SHIELD (opção "a" do menu)
// =============================================================================
//
//  Confere sozinho o que dá para medir sem ninguém olhar, e termina com um
//  RESUMO. As linhas RESULTADO;... e FIM;... seguem o formato que o
//  scripts/serial_placa.py lê (opção --shield). O que só uma pessoa vê ou
//  ouve (cor dos LEDs, som do buzzer) fica nos testes 1 a 9 do menu.

void testeAutomatico() {
  zerarResultados();
  Serial.println();
  Serial.print(F("INICIO;teste_shield_9em1_uno_r4;"));
  Serial.println(VERSAO);
  resultadosDaPlaca();

  // Botões e receptor IR em repouso: com os pull-ups do shield, HIGH.
  pinMode(PINO_SW1, INPUT);
  pinMode(PINO_SW2, INPUT);
  pinMode(PINO_IR, INPUT);
  delay(5);
  bool sw1 = digitalRead(PINO_SW1) == HIGH;
  bool sw2 = digitalRead(PINO_SW2) == HIGH;
  resultado(F("botoes"), F("Botões SW1 e SW2"), (sw1 && sw2) ? OK : AVISO,
            (sw1 && sw2) ? "soltos em HIGH (ativos em LOW)"
            : (!sw1 && !sw2) ? "SW1 e SW2 em LOW sem apertar"
            : !sw1 ? "SW1 (D2) em LOW sem apertar"
                   : "SW2 (D3) em LOW sem apertar");
  bool ir = digitalRead(PINO_IR) == HIGH;
  resultado(F("ir"), F("Receptor infravermelho"), ir ? OK : AVISO,
            ir ? "D6 em HIGH (repouso)" : "D6 em LOW (sinal ou defeito)");

  // Saídas: escreve HIGH e LOW e lê de volta (pega pino em curto). Os LEDs
  // piscam rapidamente.
  const uint8_t saidas[5] = {PINO_RGB_VERMELHO, PINO_RGB_AZUL, PINO_RGB_VERDE,
                             PINO_LED_D12, PINO_LED_D13};
  char falhas[32] = "";
  char nome[4];
  for (uint8_t i = 0; i < 5; i++) {
    // pinMode() devolve o pino ao modo GPIO. No UNO R4, depois de um
    // analogWrite() (o menu apaga o RGB assim), o pino fica com o timer do
    // PWM e o digitalWrite() deixa de controlá-lo: sem isto, D9 a D11
    // falhavam aqui.
    pinMode(saidas[i], OUTPUT);
    digitalWrite(saidas[i], HIGH);
    delay(80);
    bool alto = digitalRead(saidas[i]) == HIGH;
    digitalWrite(saidas[i], LOW);
    delayMicroseconds(50);
    bool baixo = digitalRead(saidas[i]) == LOW;
    if (!alto || !baixo) {
      nomePino(saidas[i], nome);
      acrescentar(falhas, sizeof(falhas), nome);
      acrescentar(falhas, sizeof(falhas), " ");
    }
  }
  if (falhas[0] == '\0') {
    resultado(F("saidas"), F("LEDs D9 a D13"), OK, "D9 a D13 seguem HIGH/LOW");
  } else {
    resultado(F("saidas"), F("LEDs D9 a D13"), FALHA, falhas);
  }

  // DHT11: até 3 tentativas (a 1ª depois de ligar vem zerada).
  float umidade = 0, temperaturaDht = 0;
  uint8_t erro = DHT_SEM_RESPOSTA;
  for (uint8_t t = 0; t < 3 && erro != DHT_OK; t++) {
    if (t > 0) delay(1200);
    erro = lerDht11(umidade, temperaturaDht);
  }
  char texto[8], detalhe[32];
  bool dhtOk = erro == DHT_OK;
  if (dhtOk) {
    char u[6];
    dtostrf(temperaturaDht, 1, 1, texto);
    dtostrf(umidade, 1, 0, u);
    snprintf(detalhe, sizeof(detalhe), "%s °C, %s %%", texto, u);
    bool plausivel = temperaturaDht >= 0 && temperaturaDht <= 50 && umidade >= 5 && umidade <= 95;
    resultado(F("dht11"), F("DHT11"), plausivel ? OK : AVISO, detalhe);
  } else {
    resultado(F("dht11"), F("DHT11"), FALHA,
              erro == DHT_SEM_RESPOSTA ? "não respondeu"
              : erro == DHT_ERRO_CHECKSUM ? "soma de verificação errada"
                                          : "transmissão interrompida");
  }

  // LM35 (10 mV por °C), com a referência do ADC medida pela placa.
  long mv = map(lround(lerLm35()), 0, 1023, 0, referenciaMilivolts());
  float celsius = mv / 10.0;
  bool lm35Ok = celsius >= 10 && celsius <= 40;
  dtostrf(celsius, 1, 1, texto);
  snprintf(detalhe, sizeof(detalhe), "%s °C", texto);
  resultado(F("lm35"), F("LM35"), lm35Ok ? OK : AVISO, detalhe);

  int ldr = (int)lerAnalogicoMedio(PINO_LDR);
  bool ldrOk = ldr > 10 && ldr < 1013;
  snprintf(detalhe, sizeof(detalhe), ldrOk ? "%d de 1023" : "%d de 1023 (saturado)", ldr);
  resultado(F("ldr"), F("LDR (luz)"), ldrOk ? OK : AVISO, detalhe);

  snprintf(detalhe, sizeof(detalhe), "%d de 1023", (int)lerAnalogicoMedio(PINO_POT));
  resultado(F("pot"), F("Potenciômetro"), INFO, detalhe);

  if (dhtOk && lm35Ok) {
    float diferenca = fabs(celsius - temperaturaDht);
    dtostrf(diferenca, 1, 1, texto);
    snprintf(detalhe, sizeof(detalhe), "LM35 - DHT11 = %s °C", texto);
    resultado(F("temperatura"), F("LM35 x DHT11"), diferenca <= 5 ? OK : AVISO, detalhe);
  } else {
    resultado(F("temperatura"), F("LM35 x DHT11"), PULADO, "sem leitura válida");
  }

  imprimirFim();
}

// =============================================================================
//  TODOS OS TESTES EM SEQUÊNCIA
// =============================================================================
void testarTodos() {
  imprimirTitulo(F("TODOS OS TESTES EM SEQUÊNCIA"));
  Serial.println(F("Nos testes contínuos, envie c para ir ao próximo."));
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
  Serial.println(F("  1 - LEDs de 3 mm (D12 e D13)"));
  Serial.println(F("  2 - LED RGB (D9, D10, D11)"));
  Serial.println(F("  3 - Botões SW1 (D2) e SW2 (D3)"));
  Serial.println(F("  4 - Buzzer (D5)"));
  Serial.println(F("  5 - Potenciômetro (A0)"));
  Serial.println(F("  6 - LDR / luminosidade (A1)"));
  Serial.println(F("  7 - LM35 / temperatura (A2)"));
  Serial.println(F("  8 - DHT11 / temperatura e umidade (D4)"));
  Serial.println(F("  9 - Receptor infravermelho (D6)"));
  Serial.println(F("  0 - Todos os testes em sequência"));
  Serial.println(F("  a - Teste automático do shield (com resumo)"));
  Serial.println(F("  p - Painel ao vivo (todas as entradas)"));
  Serial.println(F("------------------------------------------------------------"));
  Serial.println(F("Digite a opção e envie:"));
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

  pinMode(PINO_LED_D12, OUTPUT);
  pinMode(PINO_LED_D13, OUTPUT);
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

  Serial.println();
  imprimirPlaca();
  imprimirMenu();
}

void loop() {
  if (Serial.available() == 0) {
    sinalizarEspera();
    return;
  }

  char opcao = Serial.read();

  // Ignora os caracteres de final de linha enviados pelo Monitor Serial.
  if (opcao == '\n' || opcao == '\r' || opcao == ' ') {
    return;
  }
  limparSerial();
  digitalWrite(PINO_LED_D13, LOW);  // encerra o sinal de espera
  prepararBotoes();

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
    case 'a':
    case 'A': testeAutomatico();    break;
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
