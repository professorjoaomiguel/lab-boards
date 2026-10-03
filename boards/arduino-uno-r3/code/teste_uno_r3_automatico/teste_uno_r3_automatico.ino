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
 *  Testa SÓ A PLACA, sem shield: roda uma bateria de testes sem precisar
 *  de ninguém apertando botões e termina com um RESUMO no Monitor Serial,
 *  um teste por linha. Serve para conferir rapidamente se uma placa está
 *  boa.
 *
 *    - chip ......... assinatura do microcontrolador (ATmega328P = 1E 95 0F)
 *    - relogio ...... millis() e micros() andam juntos e no ritmo certo
 *    - eeprom ....... grava, lê e restaura o último byte da EEPROM
 *    - vcc .......... tensão de alimentação, medida pela própria placa
 *    - gpio ......... cada pino: pull-up interno e saída HIGH/LOW
 *
 *  O UNO R3 não tem RTC, DAC, segunda serial nem ID único no chip: esses
 *  testes do UNO R4 não existem aqui. Os periféricos do Shield 9 em 1 são
 *  testados no sketch conjunto (placa + shield):
 *  shields/uno-shield-9in1/code/teste_shield_9em1_uno_r3.
 *
 *  COMO USAR
 *  ---------
 *  1. Placa SOZINHA: sem shield e sem nada ligado aos pinos.
 *  2. Grave o sketch (placa "Arduino Uno").
 *  3. Abra o Monitor Serial em 115200 baud (qualquer opção de final de
 *     linha). Envie "c" para começar. Envie "c" de novo para repetir.
 *
 *  Ou, pela linha de comando:
 *    python scripts/serial_placa.py auto --porta COM10
 *
 *  SINAL DE FIRMWARE GRAVADO: enquanto espera o "c", o LED "L" (D13) pisca
 *  duas vezes rápidas a cada 2 s.
 *
 *  ATENÇÃO: RODE COM A PLACA SOZINHA
 *  ---------------------------------
 *  O teste "gpio" liga D2 a D13 e A1 a A5 como saída (HIGH e LOW). Um
 *  módulo, protoboard ou shield ligado nesses pinos pode receber esses
 *  sinais e se danificar. Proteção: antes de acionar os pinos, o sketch
 *  procura resistores externos neles (como os pull-ups do Shield 9 em 1).
 *  Se achar, ele NÃO aciona nenhum pino e pede para tirar o que estiver
 *  ligado. A proteção não pega todos os circuitos: tire tudo da placa.
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

const char VERSAO[] = "2";
const unsigned long VELOCIDADE_SERIAL = 115200;

// =============================================================================
//  PINOS TESTADOS
// =============================================================================
// D0/D1 ficam de fora: são a serial da USB.
// O D13 fica fora da parte do pull-up: o LED "L" da placa está nele (em
// clones, às vezes ligado direto) e pode puxar o pino para baixo.
const uint8_t PINOS_GPIO[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, A1, A2, A3, A4, A5};

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
  Serial.println(F("ATENÇÃO: a placa deve estar SOZINHA, sem shield e sem nada nos pinos."));
  Serial.println(F("O teste liga pinos como saída: tire shield, módulos, fios e protoboard."));
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


void testeVcc() {
  Serial.println(F("# Vcc medido pela referência interna (bandgap)"));
  long vccMilivolts = medirVccMilivolts();
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
    resultado(F("gpio"), F("Pinos (GPIO)"), OK, detalhe);
  } else {
    resultado(F("gpio"), F("Pinos (GPIO)"), FALHA, falhas);
  }
}

// =============================================================================
//  PROTEÇÃO: HÁ ALGO LIGADO NOS PINOS?
// =============================================================================
//  Para cada pino: LOW como saída por 100 µs (descarrega), depois entrada
//  SEM pull-up e espera 200 µs. Num pino livre, ele continua perto de 0V;
//  se algo o puxa para cima (como os pull-ups do Shield 9 em 1 em D2, D3,
//  D4 e D6), ele volta para HIGH. Preenche "lista" com os pinos achados.
void pinosComAlgoLigado(char *lista, size_t tamanho) {
  lista[0] = '\0';
  char nome[4];
  for (uint8_t i = 0; i < sizeof(PINOS_GPIO); i++) {
    uint8_t pino = PINOS_GPIO[i];
    pinMode(pino, OUTPUT);
    digitalWrite(pino, LOW);
    delayMicroseconds(100);
    pinMode(pino, INPUT);
    delayMicroseconds(200);
    if (digitalRead(pino) == HIGH) {
      nomePino(pino, nome);
      acrescentar(lista, tamanho, nome);
      acrescentar(lista, tamanho, " ");
    }
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

  // O teste dos pinos só roda com a placa sozinha.
  char ocupados[24];
  pinosComAlgoLigado(ocupados, sizeof(ocupados));
  if (ocupados[0] == '\0') {
    testeGpio(PINOS_GPIO, sizeof(PINOS_GPIO));
  } else {
    char detalhe[32] = "ligado: ";
    acrescentar(detalhe, sizeof(detalhe), ocupados);
    resultado(F("gpio"), F("Pinos (GPIO)"), PULADO, detalhe);
    Serial.println(F("# Há algo ligado nos pinos (shield?). Tire tudo para testar os pinos."));
  }

  imprimirFim();
}

// =============================================================================
//  SETUP E LOOP
// =============================================================================
// No UNO R3, abrir a porta serial REINICIA a placa (sinal DTR, pelo chip
// da USB). Por isso as boas-vindas ficam no setup(): aparecem toda vez que
// o Monitor Serial abre. O teste só começa com o comando "c".
// Sinal de "firmware de teste gravado": enquanto espera o comando, o LED
// "L" (D13) pisca duas vezes rápidas a cada 2 s, um "tum-tum" diferente do
// Blink comum (1 s aceso, 1 s apagado). O D13 já é usado pelo bootloader,
// então piscá-lo aqui não acrescenta risco a algo ligado nele.
void sinalizarEspera() {
  unsigned long t = millis() % 2000;
  bool aceso = t < 100 || (t >= 250 && t < 350);
  digitalWrite(LED_BUILTIN, aceso ? HIGH : LOW);
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(VELOCIDADE_SERIAL);
  imprimirBoasVindas();
}

void loop() {
  if (!Serial.available()) {
    sinalizarEspera();
    return;
  }
  char comando = lerComando();
  if (comando == 'c' || comando == 'r' || comando == '\0') {
    digitalWrite(LED_BUILTIN, LOW);  // encerra o sinal de espera
    rodarTestes();
  } else {
    Serial.println(F("# Comando não entendido. Envie c para começar."));
  }
}
