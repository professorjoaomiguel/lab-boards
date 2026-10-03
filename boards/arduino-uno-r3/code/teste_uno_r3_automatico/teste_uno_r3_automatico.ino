/*
 * =============================================================================
 *  Teste AUTOMÁTICO da placa Arduino UNO R3 (ATmega328P)
 * =============================================================================
 *
 *  Autor: Prof. Joao Miguel Roehe (@professorjoaomiguel)
 *  Licença: MIT (SPDX-License-Identifier: MIT) — ver LICENSE-CODE na raiz
 *
 *  O QUE ESTE SKETCH FAZ
 *  ---------------------
 *  Roda uma bateria de testes SEM precisar de ninguém apertando botões e
 *  termina com um RESUMO no Monitor Serial, um teste por linha. Serve para
 *  conferir rapidamente se uma placa está boa.
 *
 *  Testes da placa (sempre rodam):
 *    - chip ......... assinatura do microcontrolador (ATmega328P = 1E 95 0F)
 *    - relogio ...... millis() e micros() andam juntos e no ritmo certo
 *    - eeprom ....... grava, lê e restaura o último byte da EEPROM
 *    - vcc .......... tensão de alimentação, medida pela própria placa
 *    - gpio ......... cada pino livre: pull-up interno e saída HIGH/LOW
 *
 *  Testes do Shield 9 em 1 (só rodam se o shield for detectado):
 *    - botoes, ir ... nível de repouso de D2, D3 (botões) e D6 (receptor IR)
 *    - saidas ....... LEDs D12/D13 e LED RGB D9-D11 (escreve e lê de volta)
 *    - dht11 ........ leitura com soma de verificação e valores plausíveis
 *    - lm35, ldr .... leituras dentro da faixa esperada
 *    - pot .......... posição atual do potenciômetro (só informa)
 *    - temperatura .. LM35 e DHT11 concordam (diferença de até 5 °C)
 *
 *  O UNO R3 não tem RTC, DAC, segunda serial nem ID único no chip: esses
 *  testes do UNO R4 não existem aqui.
 *
 *  COMO USAR
 *  ---------
 *  1. Para os testes da placa: NADA ligado aos pinos. Para os testes do
 *     shield: o Shield 9 em 1 encaixado (sem mais nada).
 *  2. Grave o sketch (placa "Arduino Uno").
 *  3. Abra o Monitor Serial em 115200 baud (qualquer opção de final de
 *     linha). Envie "c" para começar. Envie "c" de novo para repetir.
 *
 *  Ou, pela linha de comando:
 *    python scripts/serial_placa.py auto --porta COM10
 *
 *  ATENÇÃO: RODE COM A PLACA SOZINHA OU SÓ COM O SHIELD 9 EM 1
 *  ------------------------------------------------------------
 *  Sem o shield, o teste "gpio" liga D2 a D13 e A1 a A5 como saída (HIGH e
 *  LOW). Um módulo, protoboard ou outro shield ligado nesses pinos pode
 *  receber esses sinais e se danificar. Um pino que já está sendo puxado
 *  para LOW por algo externo não é ligado como saída, mas essa proteção
 *  não pega todos os casos.
 *
 *  FORMATO DA SAÍDA
 *  ----------------
 *  O mesmo do teste do UNO R4, para o scripts/serial_placa.py:
 *    INICIO;teste_uno_r3_automatico;<versão>
 *    RESULTADO;<teste>;<OK|FALHA|AVISO|PULADO|INFO>;<detalhe>
 *    FIM;ok=<n>;falha=<n>;aviso=<n>;pulado=<n>
 *
 *  MEMÓRIA
 *  -------
 *  O ATmega328P tem só 2 KB de RAM. Por isso os textos ficam na flash
 *  (F("...")), os detalhes são montados em buffers de tamanho fixo (sem a
 *  classe String) e o resumo guarda no máximo 16 resultados.
 *
 *  Repositório: https://github.com/professorjoaomiguel/lab-boards
 * =============================================================================
 */

#if !defined(ARDUINO_AVR_UNO)
#error "Este sketch é só para o Arduino UNO R3. Para o UNO R4, use teste_uno_r4_automatico."
#endif

#include <EEPROM.h>
#include <avr/boot.h>

const char VERSAO[] = "1";
const unsigned long VELOCIDADE_SERIAL = 115200;

// =============================================================================
//  PINOS DO SHIELD 9 EM 1 (ver shields/uno-shield-9in1/README.md)
// =============================================================================
const uint8_t PINO_SW1          = 2;
const uint8_t PINO_SW2          = 3;
const uint8_t PINO_DHT11        = 4;
const uint8_t PINO_IR           = 6;
const uint8_t PINO_RGB_VERMELHO = 9;
const uint8_t PINO_RGB_AZUL     = 10;
const uint8_t PINO_RGB_VERDE    = 11;
// LEDs de 3 mm: a COR muda conforme a versão do shield (numa, D12 é
// vermelho e D13 azul; noutra, o contrário). Por isso o nome é o pino.
const uint8_t PINO_LED_D12      = 12;
const uint8_t PINO_LED_D13      = 13;
const uint8_t PINO_POT          = A0;
const uint8_t PINO_LDR          = A1;
const uint8_t PINO_LM35         = A2;

// Pinos do teste "gpio". D0/D1 ficam de fora: são a serial da USB.
// O D13 fica fora da parte do pull-up: o LED "L" da placa está nele (em
// clones, às vezes ligado direto) e pode puxar o pino para baixo.
const uint8_t PINOS_SEM_SHIELD[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, A1, A2, A3, A4, A5};
const uint8_t PINOS_COM_SHIELD[] = {7, 8, A3, A4, A5};  // livres no shield

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
const uint8_t MAX_REGISTROS = 16;
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
//  ENTRADA PELO MONITOR SERIAL (aceita qualquer opção de final de linha)
// =============================================================================

// Descarta o '\r'/'\n' que sobrou (opção "Ambos, NL e CR").
void descartarFimDeLinha() {
  delay(20);
  while (Serial.peek() == '\r' || Serial.peek() == '\n') {
    Serial.read();
  }
}

// Lê o comando enviado. A mensagem termina no '\r' ou '\n' ou, com "Sem
// final de linha", quando passam 200 ms sem chegar nada. Devolve a 1ª
// letra em minúscula, ou '\0' se a linha veio vazia (só Enter).
char lerComando() {
  char primeira = '\0';
  unsigned long ultimoCaractere = millis();
  bool recebeuAlgo = false;
  while (true) {
    if (Serial.available()) {
      char c = Serial.read();
      if (c == '\n' || c == '\r') {
        descartarFimDeLinha();
        break;
      }
      if (!recebeuAlgo && c != ' ') {
        primeira = tolower(c);
        recebeuAlgo = true;
      }
      ultimoCaractere = millis();
    } else if (recebeuAlgo && millis() - ultimoCaractere > 200) {
      break;
    }
  }
  return primeira;
}

void imprimirBoasVindas() {
  Serial.println();
  Serial.println(F("=============================================================="));
  Serial.println(F("  TESTE AUTOMÁTICO DO UNO R3"));
  Serial.println(F("=============================================================="));
  Serial.println(F("ATENÇÃO: a placa deve estar SOZINHA ou só com o Shield 9 em 1."));
  Serial.println(F("O teste liga pinos como saída: tire módulos, fios e protoboard."));
  Serial.println(F("> Envie c para começar (ou só Enter, se o Monitor estiver em \"Nova linha\")."));
}

// =============================================================================
//  TESTES DA PLACA
// =============================================================================

// A assinatura é gravada de fábrica em cada modelo de chip AVR. No
// ATmega328P é 1E 95 0F; no ATmega328PB (algumas placas compatíveis) é
// 1E 95 16, e no ATmega328 (sem "P") é 1E 95 14.
void testeChip() {
  Serial.println(F("# Assinatura do microcontrolador"));
  uint8_t a = boot_signature_byte_get(0x00);
  uint8_t b = boot_signature_byte_get(0x02);
  uint8_t c = boot_signature_byte_get(0x04);
  char detalhe[32];
  snprintf(detalhe, sizeof(detalhe), "%02X %02X %02X", a, b, c);
  if (a == 0x1E && b == 0x95 && c == 0x0F) {
    acrescentar(detalhe, sizeof(detalhe), " = ATmega328P");
    resultado(F("chip"), F("Microcontrolador"), OK, detalhe);
  } else {
    acrescentar(detalhe, sizeof(detalhe), " (não é ATmega328P)");
    resultado(F("chip"), F("Microcontrolador"), AVISO, detalhe);
  }
}

void testeRelogio() {
  Serial.println(F("# Relógio: millis() e micros() durante 1 segundo"));
  unsigned long m0 = millis();
  unsigned long u0 = micros();
  delay(1000);
  unsigned long dm = millis() - m0;
  unsigned long du = micros() - u0;
  char detalhe[32];
  snprintf(detalhe, sizeof(detalhe), "%lu ms e %lu us em 1 s", dm, du);
  bool ok = dm >= 999 && dm <= 1002 && du >= 995000UL && du <= 1005000UL;
  resultado(F("relogio"), F("millis() e micros()"), ok ? OK : FALHA, detalhe);
}

// Usa o último endereço (1023) e devolve o valor original no fim.
void testeEeprom() {
  Serial.println(F("# EEPROM: grava, lê e restaura o último byte"));
  int endereco = EEPROM.length() - 1;
  uint8_t original = EEPROM.read(endereco);
  uint8_t teste = original ^ 0xFF;
  EEPROM.write(endereco, teste);
  uint8_t lido = EEPROM.read(endereco);
  EEPROM.write(endereco, original);
  uint8_t restaurado = EEPROM.read(endereco);
  char detalhe[32];
  snprintf(detalhe, sizeof(detalhe), "%u bytes, endereço %d", EEPROM.length(), endereco);
  resultado(F("eeprom"), F("EEPROM"), (lido == teste && restaurado == original) ? OK : FALHA,
            detalhe);
}

// Mede o Vcc sem multímetro: o ADC compara a referência interna de 1,1V
// (bandgap) com o próprio Vcc. Vcc = 1,1 V x 1023 / leitura.
// Cuidado: o bandgap varia de 1,0 a 1,2V entre chips (datasheet), então o
// valor tem até ~10% de erro. Serve para ver se a USB está fraca, não
// substitui o multímetro.
// Esta medição usa o Vcc como referência (não mexe no pino AREF), por isso
// é segura mesmo se o AREF do shield estiver ligado a algo.
long medirVccMilivolts() {
  ADMUX = _BV(REFS0) | 0x0E;  // referência = AVcc; canal = bandgap 1,1V
  delay(3);                   // o bandgap precisa de um tempo para firmar
  ADCSRA |= _BV(ADSC);        // 1ª conversão: descartada
  while (ADCSRA & _BV(ADSC)) {}
  ADCSRA |= _BV(ADSC);
  while (ADCSRA & _BV(ADSC)) {}
  long leitura = ADC;
  // Depois do canal interno, a 1ª troca para um pino externo deixa as
  // leituras desse pino altas por dezenas de ms (medido no UNO R3 com o
  // LM35: 328 mV logo após a troca, e os 239 mV certos 100 ms depois). Por
  // isso o multiplexador volta já para um pino externo (A0) e espera aqui,
  // e as leituras seguintes não sofrem mais com isso.
  analogRead(A0);
  delay(100);
  return 1100L * 1023L / leitura;
}

long vccMilivolts = 5000;  // usado nas contas do LM35

void testeVcc() {
  Serial.println(F("# Vcc medido pela referência interna (bandgap)"));
  vccMilivolts = medirVccMilivolts();
  char detalhe[32];
  snprintf(detalhe, sizeof(detalhe), "%ld mV (erro de até ~10%%)", vccMilivolts);
  bool ok = vccMilivolts >= 4500 && vccMilivolts <= 5250;
  resultado(F("vcc"), F("Alimentação (Vcc)"), ok ? OK : AVISO, detalhe);
}

// Pull-up interno e saída HIGH/LOW em cada pino da lista. Um pino que já
// está sendo puxado para LOW por algo externo não é ligado como saída.
void testeGpio(const uint8_t *pinos, uint8_t quantidade) {
  Serial.println(F("# GPIO: pull-up interno e saída em cada pino livre"));
  char falhas[32] = "";
  char nome[4];
  for (uint8_t i = 0; i < quantidade; i++) {
    uint8_t pino = pinos[i];
    pinMode(pino, INPUT_PULLUP);
    delay(2);
    if (digitalRead(pino) != HIGH) {
      pinMode(pino, INPUT);
      nomePino(pino, nome);
      acrescentar(falhas, sizeof(falhas), nome);
      acrescentar(falhas, sizeof(falhas), "(pull-up) ");
      continue;
    }
    pinMode(pino, OUTPUT);
    digitalWrite(pino, LOW);
    delayMicroseconds(50);
    bool lowOk = digitalRead(pino) == LOW;
    digitalWrite(pino, HIGH);
    delayMicroseconds(50);
    bool highOk = digitalRead(pino) == HIGH;
    digitalWrite(pino, LOW);
    pinMode(pino, INPUT);
    if (!lowOk || !highOk) {
      nomePino(pino, nome);
      acrescentar(falhas, sizeof(falhas), nome);
      acrescentar(falhas, sizeof(falhas), "(saída) ");
    }
  }
  if (falhas[0] == '\0') {
    char detalhe[32];
    snprintf(detalhe, sizeof(detalhe), "%u pinos OK", quantidade);
    resultado(F("gpio"), F("Pinos livres (GPIO)"), OK, detalhe);
  } else {
    resultado(F("gpio"), F("Pinos livres (GPIO)"), FALHA, falhas);
  }
}

// =============================================================================
//  DETECÇÃO DO SHIELD (mesmo método do teste do UNO R4)
// =============================================================================
//  O shield tem pull-ups externos para o 5V em D2, D3, D4 e D6. Cada pino
//  é descarregado (saída LOW) e vira entrada sem pull-up: com o shield ele
//  sobe para HIGH; sem o shield fica perto de 0V.
uint8_t contarPullupsDoShield() {
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
  return altos;
}

// =============================================================================
//  DHT11 (sem biblioteca; protocolo explicado no sketch do shield)
// =============================================================================
const uint8_t DHT_OK = 0, DHT_SEM_RESPOSTA = 1, DHT_ERRO_LEITURA = 2, DHT_ERRO_CHECKSUM = 3;
const uint16_t DHT_TIMEOUT = 0xFFFF;

uint16_t contarNivel(uint8_t nivel) {
  const uint16_t LIMITE = F_CPU / 1000;
  uint16_t contagem = 0;
  while (digitalRead(PINO_DHT11) == nivel) {
    if (++contagem >= LIMITE) return DHT_TIMEOUT;
  }
  return contagem;
}

uint8_t lerDht11(float &umidade, float &temperatura) {
  uint8_t dados[5] = {0, 0, 0, 0, 0};
  uint16_t ciclos[80];
  pinMode(PINO_DHT11, OUTPUT);
  digitalWrite(PINO_DHT11, LOW);
  delay(20);
  pinMode(PINO_DHT11, INPUT_PULLUP);
  delayMicroseconds(55);

  noInterrupts();
  if (contarNivel(LOW) == DHT_TIMEOUT || contarNivel(HIGH) == DHT_TIMEOUT) {
    interrupts();
    return DHT_SEM_RESPOSTA;
  }
  for (uint8_t i = 0; i < 80; i += 2) {
    ciclos[i]     = contarNivel(LOW);
    ciclos[i + 1] = contarNivel(HIGH);
  }
  interrupts();

  for (uint8_t bit = 0; bit < 40; bit++) {
    if (ciclos[2 * bit] == DHT_TIMEOUT || ciclos[2 * bit + 1] == DHT_TIMEOUT) {
      return DHT_ERRO_LEITURA;
    }
    dados[bit / 8] <<= 1;
    if (ciclos[2 * bit + 1] > ciclos[2 * bit]) dados[bit / 8] |= 1;
  }
  uint8_t soma = dados[0] + dados[1] + dados[2] + dados[3];
  if (soma != dados[4]) return DHT_ERRO_CHECKSUM;
  // A 1ª leitura depois de ligar vem zerada e passa na soma: descartar.
  if (dados[0] == 0 && dados[2] == 0 && dados[4] == 0) return DHT_ERRO_LEITURA;
  umidade = dados[0] + dados[1] * 0.1;
  temperatura = dados[2] + (dados[3] & 0x0F) * 0.1;
  if (dados[3] & 0x80) temperatura = -temperatura;
  return DHT_OK;
}

// =============================================================================
//  TESTES DO SHIELD 9 EM 1
// =============================================================================

// Média de 16 leituras, depois de trocar de canal e ESPERAR 100 ms.
// O ADC tem um capacitor interno que chega carregado com a tensão do pino
// lido antes, e o LM35 quase não consegue absorver corrente para
// descarregá-lo. No UNO R3 (medido em 2026-10-03): o LM35 lido logo depois
// de um pino com tensão maior dava até 445 mV (1ª leitura) e ~295 mV nas
// seguintes, contra 239 mV certos; 100 ms depois da troca, a leitura fica
// certa. (No UNO R4 o erro não passa sozinho; ver o README do R4.)
float lerAnalogicoMedio(uint8_t pino) {
  analogRead(pino);  // troca o canal do ADC para este pino
  delay(100);
  unsigned long soma = 0;
  for (uint8_t i = 0; i < 16; i++) soma += analogRead(pino);
  return soma / 16.0;
}

void testeShieldRepouso() {
  Serial.println(F("# Shield: nível de repouso das entradas digitais"));
  bool sw1 = digitalRead(PINO_SW1) == HIGH;
  bool sw2 = digitalRead(PINO_SW2) == HIGH;
  // Um botão em LOW sem ninguém apertar: botão travado, curto com o GND
  // ou pino torto. Visto no 1º UNO R3 testado (SW2/D3 preso em LOW).
  const char *detalhe = (sw1 && sw2) ? "soltos em HIGH (ativos em LOW)"
                        : (!sw1 && !sw2) ? "SW1 e SW2 em LOW sem apertar"
                        : !sw1 ? "SW1 (D2) em LOW sem apertar"
                               : "SW2 (D3) em LOW sem apertar";
  resultado(F("botoes"), F("Botões SW1 e SW2"), (sw1 && sw2) ? OK : AVISO, detalhe);
  bool ir = digitalRead(PINO_IR) == HIGH;
  resultado(F("ir"), F("Receptor infravermelho"), ir ? OK : AVISO,
            ir ? "D6 em HIGH (repouso)" : "D6 em LOW (sinal ou defeito)");
}

void testeShieldSaidas() {
  Serial.println(F("# Shield: saídas D9-D13 (os LEDs piscam rapidamente)"));
  const uint8_t pinos[5] = {PINO_RGB_VERMELHO, PINO_RGB_AZUL, PINO_RGB_VERDE,
                            PINO_LED_D12, PINO_LED_D13};
  char falhas[32] = "";
  char nome[4];
  for (uint8_t i = 0; i < 5; i++) {
    pinMode(pinos[i], OUTPUT);
    digitalWrite(pinos[i], HIGH);
    delay(80);
    bool highOk = digitalRead(pinos[i]) == HIGH;
    digitalWrite(pinos[i], LOW);
    delayMicroseconds(50);
    bool lowOk = digitalRead(pinos[i]) == LOW;
    if (!highOk || !lowOk) {
      nomePino(pinos[i], nome);
      acrescentar(falhas, sizeof(falhas), nome);
      acrescentar(falhas, sizeof(falhas), " ");
    }
  }
  if (falhas[0] == '\0') {
    resultado(F("saidas"), F("LEDs D9 a D13"), OK, "D9 a D13 seguem HIGH/LOW");
  } else {
    resultado(F("saidas"), F("LEDs D9 a D13"), FALHA, falhas);
  }
}

bool testeShieldDht11(float &temperatura) {
  Serial.println(F("# Shield: DHT11 (até 3 tentativas)"));
  float umidade = 0;
  uint8_t erro = DHT_SEM_RESPOSTA;
  for (uint8_t t = 0; t < 3 && erro != DHT_OK; t++) {
    if (t > 0) delay(1200);
    erro = lerDht11(umidade, temperatura);
  }
  if (erro != DHT_OK) {
    resultado(F("dht11"), F("DHT11"), FALHA,
              erro == DHT_SEM_RESPOSTA ? "não respondeu"
              : erro == DHT_ERRO_CHECKSUM ? "soma de verificação errada"
                                          : "transmissão interrompida");
    return false;
  }
  char t[8], u[8], detalhe[32];
  dtostrf(temperatura, 1, 1, t);
  dtostrf(umidade, 1, 0, u);
  snprintf(detalhe, sizeof(detalhe), "%s °C, %s %%", t, u);
  bool plausivel = temperatura >= 0 && temperatura <= 50 && umidade >= 5 && umidade <= 95;
  resultado(F("dht11"), F("DHT11"), plausivel ? OK : AVISO, detalhe);
  return true;
}

void testeShieldAnalogicos(bool dhtValido, float temperaturaDht) {
  Serial.println(F("# Shield: entradas analógicas (10 bits, referência = Vcc medido)"));
  // LM35: 10 mV por °C. Referência padrão (Vcc), com o Vcc medido acima.
  // A referência interna de 1,1V daria mais resolução, mas, ao trocar de
  // referência, o capacitor do pino AREF da placa leva centenas de ms para
  // descarregar de 5V até 1,1V (medido: leituras erradas por ~100 ms e
  // certas só depois de ~500 ms). Para um teste rápido, o Vcc basta.
  analogReference(DEFAULT);
  long mv = map(lround(lerAnalogicoMedio(PINO_LM35)), 0, 1023, 0, vccMilivolts);
  float celsius = mv / 10.0;
  bool lm35Ok = celsius >= 10 && celsius <= 40;
  char texto[8], detalhe[32];
  dtostrf(celsius, 1, 1, texto);
  snprintf(detalhe, sizeof(detalhe), "%s °C", texto);
  resultado(F("lm35"), F("LM35"), lm35Ok ? OK : AVISO, detalhe);

  int ldr = (int)lerAnalogicoMedio(PINO_LDR);
  bool ldrOk = ldr > 10 && ldr < 1013;
  snprintf(detalhe, sizeof(detalhe), ldrOk ? "%d de 1023" : "%d de 1023 (saturado)", ldr);
  resultado(F("ldr"), F("LDR (luz)"), ldrOk ? OK : AVISO, detalhe);

  snprintf(detalhe, sizeof(detalhe), "%d de 1023", (int)lerAnalogicoMedio(PINO_POT));
  resultado(F("pot"), F("Potenciômetro"), INFO, detalhe);

  if (dhtValido && lm35Ok) {
    float diferenca = fabs(celsius - temperaturaDht);
    dtostrf(diferenca, 1, 1, texto);
    snprintf(detalhe, sizeof(detalhe), "LM35 - DHT11 = %s °C", texto);
    resultado(F("temperatura"), F("LM35 x DHT11"), diferenca <= 5 ? OK : AVISO, detalhe);
  } else {
    resultado(F("temperatura"), F("LM35 x DHT11"), PULADO, "sem leitura válida");
  }
}

// =============================================================================
//  ROTEIRO
// =============================================================================
void rodarTestes() {
  zerarResultados();
  Serial.println();
  Serial.print(F("INICIO;teste_uno_r3_automatico;"));
  Serial.println(VERSAO);

  testeChip();
  testeRelogio();
  testeEeprom();
  testeVcc();

  uint8_t pullups = contarPullupsDoShield();
  bool comShield = pullups >= 3;
  bool semShield = pullups == 0;
  char detalhe[32];
  if (comShield) {
    snprintf(detalhe, sizeof(detalhe), "detectado (%u/4 pull-ups)", pullups);
    resultado(F("shield"), F("Shield 9 em 1"), INFO, detalhe);
  } else if (semShield) {
    resultado(F("shield"), F("Shield 9 em 1"), INFO, "nenhum shield detectado");
  } else {
    snprintf(detalhe, sizeof(detalhe), "montagem estranha (%u/4)", pullups);
    resultado(F("shield"), F("Shield 9 em 1"), AVISO, detalhe);
  }

  if (semShield) {
    testeGpio(PINOS_SEM_SHIELD, sizeof(PINOS_SEM_SHIELD));
  } else {
    testeGpio(PINOS_COM_SHIELD, sizeof(PINOS_COM_SHIELD));
  }

  if (comShield) {
    testeShieldRepouso();
    testeShieldSaidas();
    float temperaturaDht = 0;
    bool dhtValido = testeShieldDht11(temperaturaDht);
    testeShieldAnalogicos(dhtValido, temperaturaDht);
  } else {
    resultado(F("shield"), F("Testes do shield"), PULADO, "shield ausente");
  }

  imprimirFim();
}

// =============================================================================
//  SETUP E LOOP
// =============================================================================
// No UNO R3, abrir a porta serial REINICIA a placa (sinal DTR, pelo chip
// da USB). Por isso as boas-vindas ficam no setup(): aparecem toda vez que
// o Monitor Serial abre. O teste só começa com o comando "c".
void setup() {
  Serial.begin(VELOCIDADE_SERIAL);
  imprimirBoasVindas();
}

void loop() {
  if (!Serial.available()) return;
  char comando = lerComando();
  if (comando == 'c' || comando == 'r' || comando == '\0') {
    rodarTestes();
  } else {
    Serial.println(F("# Comando não entendido. Envie c para começar."));
  }
}
