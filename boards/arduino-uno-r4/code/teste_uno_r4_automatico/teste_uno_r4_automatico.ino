/*
 * =============================================================================
 *  Teste AUTOMÁTICO da placa Arduino UNO R4 (Minima / WiFi)
 * =============================================================================
 *
 *  Autor: Prof. Joao Miguel Roehe (@professorjoaomiguel)
 *  Licença: MIT (SPDX-License-Identifier: MIT) — ver LICENSE-CODE na raiz
 *
 *  O QUE ESTE SKETCH FAZ
 *  ---------------------
 *  Roda uma bateria de testes SEM precisar de ninguém apertando botões e
 *  imprime o resultado no Monitor Serial. Serve para conferir rapidamente
 *  se uma placa está boa (por exemplo, ao receber um lote novo) e para
 *  registrar a placa no inventário.
 *
 *  Testes da placa (sempre rodam):
 *    - info ......... modelo, ID único do chip e frequência do clock
 *    - relogio ...... millis() e micros() andam juntos e no ritmo certo
 *    - eeprom ....... grava, lê e restaura o último byte da EEPROM
 *    - rtc .......... o relógio de tempo real (RTC) conta os segundos
 *    - gpio ......... cada pino livre: pull-up interno e saída HIGH/LOW
 *    - dac .......... DAC do A0 lido de volta pelo ADC de 14 bits
 *                     (só SEM o shield: o potenciômetro do shield fica no A0)
 *    - serial1 ...... laço D1 (TX) -> D0 (RX), se houver um jumper entre eles
 *
 *  Testes do Shield 9 em 1 (só rodam se o shield for detectado):
 *    - botoes, ir ... nível de repouso de D2, D3 (botões) e D6 (receptor IR)
 *    - saidas ....... LEDs D12/D13 e LED RGB D9-D11 (escreve e lê de volta)
 *    - dht11 ........ leitura com soma de verificação e valores plausíveis
 *    - lm35, ldr .... leituras dentro da faixa esperada
 *    - pot .......... posição atual do potenciômetro (só informa)
 *    - temperatura .. LM35 e DHT11 concordam (diferença de até 5 °C)
 *
 *  FORMATO DA SAÍDA
 *  ----------------
 *  As linhas que começam com "#" são explicações para quem lê. As demais
 *  seguem um formato fixo, separado por ";", para que um programa (como o
 *  scripts/serial_placa.py) consiga ler o resultado sozinho:
 *
 *    INICIO;teste_uno_r4_automatico;<versão>
 *    ID_UNICO;<32 dígitos hexadecimais>
 *    RESULTADO;<teste>;<OK|FALHA|AVISO|PULADO|INFO>;<detalhe>
 *    FIM;ok=<n>;falha=<n>;aviso=<n>;pulado=<n>
 *
 *    OK     = funcionou como esperado
 *    FALHA  = não funcionou: a placa (ou o shield) tem problema
 *    AVISO  = funcionou, mas com um valor estranho: vale olhar de perto
 *    PULADO = não deu para testar nesta montagem (ex: sem shield)
 *    INFO   = só informação, não conta como teste
 *
 *  COMO USAR
 *  ---------
 *  1. Para os testes da placa: NADA ligado aos pinos. Para os testes do
 *     shield: o Shield 9 em 1 encaixado (sem mais nada).
 *     Opcional: um jumper entre D0 e D1 (no shield, entre TXD e RXD da
 *     barra "serial TTL") ativa o teste "serial1".
 *  2. Grave o sketch (placa "Arduino UNO R4 Minima" ou "Arduino UNO R4
 *     WiFi").
 *  3. Abra o Monitor Serial (qualquer velocidade: no UNO R4 a serial é USB
 *     nativa e ignora o baud; qualquer opção de final de linha). Envie "c"
 *     para começar. No fim aparece um RESUMO, um teste por linha. Envie
 *     "c" de novo para repetir.
 *
 *  Ou, pela linha de comando, com o script do repositório:
 *    python scripts/serial_placa.py auto --porta COM8
 *
 *  ATENÇÃO: RODE COM A PLACA SOZINHA OU SÓ COM O SHIELD 9 EM 1
 *  ------------------------------------------------------------
 *  O teste só começa quando você envia "c" (abrir a porta não basta).
 *  Se o shield não for detectado, o teste
 *  "gpio" liga D2 a D13 e A1 a A5 como saída (HIGH e LOW) e o teste "dac"
 *  gera tensão no A0. Um módulo, protoboard ou outro shield ligado nesses
 *  pinos pode receber esses sinais e se danificar (o UNO R4 aguenta só
 *  8 mA por pino). Há uma proteção parcial: um pino que já está sendo
 *  puxado para LOW por algo externo não é ligado como saída. Mas ela não
 *  pega tudo: tire tudo da placa antes de testar.
 *  Com o shield, usa só os pinos que ele deixa livres (D7, D8, A3, A4, A5).
 *
 *  Nenhuma biblioteca externa: EEPROM e RTC já vêm no pacote "Arduino UNO
 *  R4 Boards".
 *
 *  Repositório: https://github.com/professorjoaomiguel/lab-boards
 * =============================================================================
 */

#if !defined(ARDUINO_ARCH_RENESAS_UNO)
#error "Este sketch é só para o Arduino UNO R4 (Minima ou WiFi)."
#endif

#include <EEPROM.h>
#include "RTC.h"

const char *VERSAO = "2";

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
// LEDs de 3 mm: no padrão, D12 é VERMELHO e D13 é AZUL. Mas há variação de
// montagem (um shield testado veio com as cores trocadas), por isso o nome
// é o pino, e não a cor.
const uint8_t PINO_LED_D12      = 12;
const uint8_t PINO_LED_D13      = 13;
const uint8_t PINO_POT          = A0;
const uint8_t PINO_LDR          = A1;
const uint8_t PINO_LM35         = A2;

// Pinos que o teste "gpio" usa. D0/D1 ficam de fora (são o Serial1) e o A0
// tem teste próprio (DAC).
//
// O D13 fica fora da parte do pull-up: o LED "L" da placa está ligado nele
// e puxa o pino para baixo. A entrada do RA4M1 só lê HIGH acima de 0,8 x
// 5V = 4V, e o pull-up interno (fraco, dezenas de kΩ) não chega lá com o
// LED conduzindo um pouquinho. Ele ainda passa pelo teste de saída.
const uint8_t PINOS_SEM_SHIELD[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, A1, A2, A3, A4, A5};
const uint8_t PINOS_COM_SHIELD[] = {7, 8, A3, A4, A5};  // livres no shield

// =============================================================================
//  CONTAGEM E IMPRESSÃO DOS RESULTADOS
// =============================================================================
unsigned int totalOk, totalFalha, totalAviso, totalPulado;

// Cada resultado sai na hora numa linha "RESULTADO;..." (formato fixo, lido
// pelo scripts/serial_placa.py) e fica guardado para o RESUMO do final,
// que é o que o aluno lê no Monitor Serial.
struct Registro {
  const char *teste;
  const char *estado;
  String detalhe;
};
const uint8_t MAX_REGISTROS = 40;
Registro registros[MAX_REGISTROS];
uint8_t totalRegistros = 0;

void zerarResultados() {
  totalOk = totalFalha = totalAviso = totalPulado = 0;
  totalRegistros = 0;
}

void resultado(const char *teste, const char *estado, const String &detalhe) {
  Serial.print("RESULTADO;");
  Serial.print(teste);
  Serial.print(';');
  Serial.print(estado);
  Serial.print(';');
  Serial.println(detalhe);

  if (totalRegistros < MAX_REGISTROS) {
    registros[totalRegistros].teste = teste;
    registros[totalRegistros].estado = estado;
    registros[totalRegistros].detalhe = detalhe;
    totalRegistros++;
  }

  if (strcmp(estado, "OK") == 0) totalOk++;
  else if (strcmp(estado, "FALHA") == 0) totalFalha++;
  else if (strcmp(estado, "AVISO") == 0) totalAviso++;
  else if (strcmp(estado, "PULADO") == 0) totalPulado++;
}

// Nome de cada teste no resumo. A linha RESULTADO usa o identificador curto
// (sem acento nem espaço), que é o que o script lê.
const char *nomeDoTeste(const char *id) {
  struct Nome { const char *id; const char *nome; };
  static const Nome NOMES[] = {
    {"info", "Informação"},             {"clock", "Clock do processador"},
    {"relogio", "millis() e micros()"}, {"eeprom", "EEPROM"},
    {"rtc", "RTC (relógio)"},           {"shield", "Shield 9 em 1"},
    {"gpio", "Pinos livres (GPIO)"},    {"dac", "DAC no A0"},
    {"serial1", "Serial1 (D0/D1)"},     {"botoes", "Botões SW1 e SW2"},
    {"ir", "Receptor infravermelho"},   {"saidas", "LEDs D9 a D13"},
    {"dht11", "DHT11"},                 {"avcc", "Referência do ADC"},
    {"lm35", "LM35"},                   {"ldr", "LDR (luz)"},
    {"pot", "Potenciômetro"},           {"temperatura", "LM35 x DHT11"},
    {"led_l", "LED L (D13)"},           {"tensao_5v", "Pino 5V (multímetro)"},
    {"pot_adc", "Potenciômetro e ADC"}, {"rgb_pwm", "LED RGB (PWM)"},
    {"buzzer", "Buzzer"},
  };
  for (const Nome &n : NOMES) {
    if (strcmp(n.id, id) == 0) return n.nome;
  }
  return id;
}

// Quantos caracteres aparecem na tela. Letras acentuadas ocupam 2 bytes em
// UTF-8 ("ç" = 0xC3 0xA7); só o 1º byte conta. Sem isso, os pontinhos do
// resumo ficariam desalinhados nas linhas com acento.
size_t larguraNaTela(const char *texto) {
  size_t largura = 0;
  for (const char *p = texto; *p; p++) {
    if ((*p & 0xC0) != 0x80) largura++;
  }
  return largura;
}

// Resumo legível, um teste por linha, ex:
//   [  OK  ] DHT11 ..................... 23.0 °C, 51 %
void imprimirResumo() {
  Serial.println();
  Serial.println("==============================================================");
  Serial.println("  RESUMO");
  Serial.println("==============================================================");
  for (uint8_t i = 0; i < totalRegistros; i++) {
    const char *estado = registros[i].estado;
    if (strcmp(estado, "OK") == 0)          Serial.print("  [  OK  ] ");
    else if (strcmp(estado, "FALHA") == 0)  Serial.print("  [FALHA!] ");
    else if (strcmp(estado, "AVISO") == 0)  Serial.print("  [AVISO ] ");
    else if (strcmp(estado, "PULADO") == 0) Serial.print("  [pulado] ");
    else                                    Serial.print("  [ info ] ");

    const char *nome = nomeDoTeste(registros[i].teste);
    Serial.print(nome);
    Serial.print(' ');
    for (size_t p = larguraNaTela(nome); p < 26; p++) Serial.print('.');
    Serial.print(' ');
    Serial.println(registros[i].detalhe);
  }
  Serial.println("--------------------------------------------------------------");
  Serial.print("  OK: ");
  Serial.print(totalOk);
  Serial.print("   FALHA: ");
  Serial.print(totalFalha);
  Serial.print("   AVISO: ");
  Serial.print(totalAviso);
  Serial.print("   pulados: ");
  Serial.println(totalPulado);
  if (totalFalha > 0) {
    Serial.println("  Resultado: há FALHAS. Veja as linhas marcadas com [FALHA!].");
  } else if (totalAviso > 0) {
    Serial.println("  Resultado: sem falhas, mas confira as linhas com [AVISO ].");
  } else {
    Serial.println("  Resultado: tudo certo.");
  }
  Serial.println("==============================================================");
}

// Resumo para o aluno e, por último, a linha FIM para o script (o script
// para de ler na linha FIM, então o resumo vem antes dela).
void imprimirFim() {
  imprimirResumo();
  Serial.print("FIM;ok=");
  Serial.print(totalOk);
  Serial.print(";falha=");
  Serial.print(totalFalha);
  Serial.print(";aviso=");
  Serial.print(totalAviso);
  Serial.print(";pulado=");
  Serial.println(totalPulado);
  Serial.println("# Envie c para rodar de novo.");
}

// =============================================================================
//  ENTRADA PELO MONITOR SERIAL
// =============================================================================

// Descarta os caracteres de final de linha que sobraram. Com "Ambos, NL e
// CR", o Monitor envia "\r\n": sem isto, o "\n" viraria uma linha vazia
// extra e seria lido como a próxima resposta.
void descartarFimDeLinha() {
  delay(20);
  while (Serial.peek() == '\r' || Serial.peek() == '\n') {
    Serial.read();
  }
}

// Lê uma mensagem enviada pelo Monitor Serial e espera o tempo que for
// preciso: quem dita o ritmo é a pessoa.
//
// Funciona com QUALQUER opção de final de linha do Monitor ("Nova linha",
// "Retorno de carro", "Ambos" ou "Sem final de linha"). Com final de
// linha, a mensagem termina no '\n' ou no '\r'. Sem final de linha, o
// Monitor envia o texto todo de uma vez; quando passam 200 ms sem chegar
// nada, a mensagem é dada como completa. Atenção: sem final de linha, só
// o Enter (caixa vazia) não envia nada. Por isso as instruções pedem uma
// letra (ex: "c").
String lerLinha() {
  String linha;
  unsigned long ultimoCaractere = 0;
  while (true) {
    if (Serial.available()) {
      char c = Serial.read();
      if (c == '\n' || c == '\r') {
        descartarFimDeLinha();
        break;
      }
      linha += c;
      ultimoCaractere = millis();
    } else if (linha.length() > 0 && millis() - ultimoCaractere > 200) {
      break;
    }
  }
  linha.trim();
  return linha;
}

// Mensagem mostrada quando o Monitor Serial (ou o script) abre a porta.
// Nada é testado antes de a pessoa mandar começar.
void imprimirBoasVindas(const char *titulo, const char *aviso) {
  Serial.println();
  Serial.println("==============================================================");
  Serial.print("  ");
  Serial.println(titulo);
  Serial.println("==============================================================");
  Serial.println(aviso);
  Serial.println("> Envie c para começar (ou só Enter, se o Monitor estiver em \"Nova linha\").");
}

// Lê o comando, se chegou algum. Devolve true se a pessoa mandou começar.
bool pediuParaComecar() {
  if (!Serial.available()) return false;
  String comando = lerLinha();
  comando.toLowerCase();
  if (comando == "" || comando == "c" || comando == "r") return true;
  Serial.print("# Comando não entendido: \"");
  Serial.print(comando);
  Serial.println("\". Envie c para começar.");
  return false;
}

// Nome do pino como está na serigrafia (ex: "D7", "A3").
String nomePino(uint8_t pino) {
  if (pino >= A0 && pino <= A5) {
    return "A" + String(pino - A0);
  }
  return "D" + String(pino);
}

// =============================================================================
//  TESTES DA PLACA
// =============================================================================

// ID único do RA4M1: 16 bytes gravados na fábrica, diferentes em cada chip.
// O core do Arduino usa este mesmo número como "número de série" da USB,
// impresso aqui no mesmo formato (4 palavras de 32 bits, em hexadecimal
// maiúsculo). Assim o ID visto pelo computador (Gerenciador de
// Dispositivos, arduino-cli board list) bate com o impresso pelo sketch.
//
// No UNO R4 WiFi o número de série da USB é o do ESP32-S3 (a ponte), e só
// este sketch mostra o ID do RA4M1.
String idUnico() {
  const bsp_unique_id_t *id = R_BSP_UniqueIdGet();
  String texto;
  char palavra[9];
  for (uint8_t i = 0; i < 4; i++) {
    snprintf(palavra, sizeof(palavra), "%08lX", (unsigned long)id->unique_id_words[i]);
    texto += palavra;
  }
  return texto;
}

void testeInfo() {
  Serial.println("# Informações da placa");
#if defined(ARDUINO_MINIMA)
  resultado("info", "INFO", "modelo=UNO R4 Minima");
#elif defined(ARDUINO_UNOWIFIR4)
  resultado("info", "INFO", "modelo=UNO R4 WiFi");
#else
  resultado("info", "INFO", "modelo=desconhecido (placa não é Minima nem WiFi)");
#endif

  String id = idUnico();
  Serial.print("ID_UNICO;");
  Serial.println(id);
  resultado("info", "INFO", "id_unico=" + id);

  // SystemCoreClock é a frequência real do processador (variável do CMSIS).
  // No RA4M1 do UNO R4 o esperado é 48 MHz.
  unsigned long mhz = SystemCoreClock / 1000000UL;
  if (SystemCoreClock == 48000000UL) {
    resultado("clock", "OK", "48 MHz");
  } else {
    resultado("clock", "FALHA", String(mhz) + " MHz (esperado 48 MHz)");
  }
}

// millis() e micros() vêm de contadores diferentes do mesmo clock. Se eles
// discordam, ou se 1 s medido não dá 1 s, o clock ou as interrupções estão
// com problema.
void testeRelogio() {
  Serial.println("# Relógio: millis() e micros() durante 1 segundo");
  unsigned long m0 = millis();
  unsigned long u0 = micros();
  delay(1000);
  unsigned long dm = millis() - m0;
  unsigned long du = micros() - u0;

  String detalhe = "millis=" + String(dm) + " ms, micros=" + String(du) + " us";
  bool millisOk = (dm >= 999 && dm <= 1002);
  bool microsOk = (du >= 995000UL && du <= 1005000UL);
  resultado("relogio", (millisOk && microsOk) ? "OK" : "FALHA", detalhe);
}

// A "EEPROM" do UNO R4 é, na verdade, uma área de flash de dados de 8 KB.
// O teste usa o último endereço e devolve o valor original no fim, para
// não apagar nada que um aluno tenha salvo.
void testeEeprom() {
  Serial.println("# EEPROM: grava, lê e restaura o último byte");
  int endereco = EEPROM.length() - 1;
  uint8_t original = EEPROM.read(endereco);
  uint8_t teste = original ^ 0xFF;  // o inverso garante que o valor muda

  EEPROM.write(endereco, teste);
  uint8_t lido = EEPROM.read(endereco);
  EEPROM.write(endereco, original);
  uint8_t restaurado = EEPROM.read(endereco);

  String detalhe = String(EEPROM.length()) + " bytes, endereço " + String(endereco);
  if (lido == teste && restaurado == original) {
    resultado("eeprom", "OK", detalhe);
  } else {
    resultado("eeprom", "FALHA", detalhe + ": gravou 0x" + String(teste, HEX) +
              ", leu 0x" + String(lido, HEX));
  }
}

// O RTC (relógio de tempo real) conta segundos de forma independente do
// processador. No Minima ele usa o oscilador interno de baixa velocidade
// (LOCO), que é menos preciso que um cristal: atrasar ou adiantar alguns
// segundos por hora é normal. Aqui só confere se ele anda.
void testeRtc() {
  Serial.println("# RTC: acerta 12:00:00 e confere depois de ~2 s");
  RTC.begin();
  // 1º de janeiro de 2026 foi uma quinta-feira.
  RTCTime inicio(1, Month::JANUARY, 2026, 12, 0, 0, DayOfWeek::THURSDAY,
                 SaveLight::SAVING_TIME_INACTIVE);
  RTC.setTime(inicio);
  delay(2200);

  RTCTime agora;
  RTC.getTime(agora);
  int segundos = agora.getSeconds();
  String detalhe = "contou " + String(segundos) + " s em 2,2 s";
  if (!RTC.isRunning()) {
    resultado("rtc", "FALHA", "RTC parado");
  } else if (segundos >= 2 && segundos <= 3) {
    resultado("rtc", "OK", detalhe);
  } else {
    resultado("rtc", "FALHA", detalhe);
  }
}

// Para cada pino: liga o pull-up interno e confere HIGH; depois vira saída
// e confere se o pino segue o que foi escrito (LOW e HIGH). O digitalRead()
// do RA4M1 lê o nível REAL do pino, mesmo em modo saída, então um pino em
// curto com o GND ou o 5V aparece aqui.
void testeGpio(const uint8_t *pinos, size_t quantidade) {
  Serial.println("# GPIO: pull-up interno e saída em cada pino livre");
  String falhas;
  String testados;

  for (size_t i = 0; i < quantidade; i++) {
    uint8_t pino = pinos[i];
    testados += nomePino(pino) + " ";

    pinMode(pino, INPUT_PULLUP);
    delay(2);
    bool pullupOk = (digitalRead(pino) == HIGH);

    // Segurança: se o pull-up não segurou o HIGH, algo externo está puxando
    // o pino para baixo (um fio, um módulo, outro shield). Nesse caso o pino
    // NÃO é ligado como saída, para não brigar com esse circuito.
    if (!pullupOk) {
      pinMode(pino, INPUT);
      falhas += nomePino(pino) + "(pull-up; saída não testada) ";
      continue;
    }

    pinMode(pino, OUTPUT);
    digitalWrite(pino, LOW);
    delayMicroseconds(50);
    bool lowOk = (digitalRead(pino) == LOW);
    digitalWrite(pino, HIGH);
    delayMicroseconds(50);
    bool highOk = (digitalRead(pino) == HIGH);

    digitalWrite(pino, LOW);
    pinMode(pino, INPUT);  // devolve o pino em alta impedância

    if (!lowOk)    falhas += nomePino(pino) + "(saída LOW) ";
    if (!highOk)   falhas += nomePino(pino) + "(saída HIGH) ";
  }

  testados.trim();
  if (falhas.length() == 0) {
    resultado("gpio", "OK", String(quantidade) + " pinos: " + testados);
  } else {
    falhas.trim();
    resultado("gpio", "FALHA", falhas + " (há algo ligado no pino?)");
  }
}

// O A0 do UNO R4 tem um DAC de 12 bits: uma saída analógica de verdade,
// que gera uma tensão fixa (não PWM). O mesmo pino também é entrada do
// ADC, então dá para ler de volta a tensão gerada. Como o DAC e o ADC usam
// a mesma referência (a alimentação analógica), o resultado é uma fração
// e não depende de a USB entregar 5,0V ou 4,7V.
void testeDac(bool podeUsarA0) {
  if (!podeUsarA0) {
    resultado("dac", "PULADO", "A0 ocupado (potenciômetro do shield)");
    return;
  }
  Serial.println("# DAC: gera 25%, 50% e 75% no A0 e lê de volta com 14 bits");
  analogWriteResolution(12);  // DAC: 0 a 4095
  analogReadResolution(14);   // ADC: 0 a 16383

  const uint16_t niveis[3] = {1024, 2048, 3072};
  String detalhe;
  bool ok = true;
  for (uint8_t i = 0; i < 3; i++) {
    analogWrite(DAC, niveis[i]);
    delay(5);
    long lido = analogRead(A0);
    long esperado = (long)niveis[i] * 4;  // 12 bits -> 14 bits
    long erro = labs(lido - esperado);
    detalhe += String(niveis[i] * 100 / 4096) + "%=" + String(lido) + " ";
    // 3% de tolerância: cobre o erro de ganho e de offset do DAC e do ADC.
    if (erro > 16383L * 3 / 100) {
      ok = false;
    }
  }
  analogWrite(DAC, 0);
  analogReadResolution(10);  // volta ao padrão para os testes seguintes
  detalhe.trim();
  resultado("dac", ok ? "OK" : "FALHA", detalhe + " (de 16383)");
}

// Serial1 usa D0 (RX) e D1 (TX), independentes da USB. Com um jumper entre
// eles, o que sai pelo TX volta pelo RX.
void testeSerial1() {
  Serial.println("# Serial1: envia pelo D1 (TX) e espera receber no D0 (RX)");
  const char *MENSAGEM = "LAB-R4";
  Serial1.begin(115200);
  delay(5);
  while (Serial1.available()) Serial1.read();  // descarta lixo antigo

  Serial1.print(MENSAGEM);
  Serial1.flush();  // espera terminar de enviar
  delay(20);

  String recebido;
  while (Serial1.available() && recebido.length() < 32) {
    recebido += (char)Serial1.read();
  }
  Serial1.end();

  if (recebido.length() == 0) {
    resultado("serial1", "PULADO", "nada recebido (sem jumper entre D0 e D1)");
  } else if (recebido == MENSAGEM) {
    resultado("serial1", "OK", "enviou e recebeu \"" + String(MENSAGEM) + "\"");
  } else {
    // Sem jumper, um RX solto pode captar ruído. Com jumper, indica defeito.
    resultado("serial1", "AVISO", String(recebido.length()) +
              " bytes diferentes do enviado (RX solto ou mau contato no jumper)");
  }
}

// =============================================================================
//  DETECÇÃO DO SHIELD
// =============================================================================
//
//  O shield tem resistores de pull-up EXTERNOS para o 5V em D2, D3 (≈10 kΩ),
//  D4 (≈3,3 kΩ) e D6 (≈10 kΩ) — medidos com multímetro. O truque:
//    1. coloca o pino em LOW como saída (descarrega o pino);
//    2. vira entrada SEM pull-up interno e espera 200 µs;
//    3. com o shield, o resistor externo puxa o pino para HIGH;
//       sem o shield, o pino fica "solto" e continua perto de 0V.
//  Conta quantos dos 4 pinos subiram. Um botão apertado ou um sensor
//  respondendo pode derrubar um deles, por isso basta 3 de 4.

uint8_t contarPullupsDoShield() {
  const uint8_t pinos[4] = {PINO_SW1, PINO_SW2, PINO_DHT11, PINO_IR};
  uint8_t altos = 0;
  for (uint8_t i = 0; i < 4; i++) {
    pinMode(pinos[i], OUTPUT);
    digitalWrite(pinos[i], LOW);
    delayMicroseconds(100);
    pinMode(pinos[i], INPUT);
    delayMicroseconds(200);
    if (digitalRead(pinos[i]) == HIGH) {
      altos++;
    }
  }
  return altos;
}

// =============================================================================
//  LEITURA DO DHT11 (sem biblioteca)
//  Mesma técnica do sketch do shield (shields/uno-shield-9in1/code): conta
//  as voltas do laço em cada nível em vez de medir microssegundos. O
//  comentário completo do protocolo está lá.
// =============================================================================
const uint8_t DHT_OK            = 0;
const uint8_t DHT_SEM_RESPOSTA  = 1;
const uint8_t DHT_ERRO_LEITURA  = 2;
const uint8_t DHT_ERRO_CHECKSUM = 3;
const uint16_t DHT_TIMEOUT = 0xFFFF;

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
    uint16_t tempoLow  = ciclos[2 * bit];
    uint16_t tempoHigh = ciclos[2 * bit + 1];
    if (tempoLow == DHT_TIMEOUT || tempoHigh == DHT_TIMEOUT) {
      return DHT_ERRO_LEITURA;
    }
    dados[bit / 8] <<= 1;
    if (tempoHigh > tempoLow) {
      dados[bit / 8] |= 1;
    }
  }

  uint8_t soma = dados[0] + dados[1] + dados[2] + dados[3];
  if (soma != dados[4]) {
    return DHT_ERRO_CHECKSUM;
  }
  // O DHT11 responde com a medição ANTERIOR. Na 1ª leitura depois de ligar
  // ainda não há medição, e ele envia 5 bytes zerados, que "passam" na soma
  // de verificação (0+0+0+0 = 0). Visto na placa real: descartar.
  if (dados[0] == 0 && dados[2] == 0 && dados[4] == 0) {
    return DHT_ERRO_LEITURA;
  }
  umidade = dados[0] + dados[1] * 0.1;
  temperatura = dados[2] + (dados[3] & 0x0F) * 0.1;
  if (dados[3] & 0x80) {
    temperatura = -temperatura;
  }
  return DHT_OK;
}

// =============================================================================
//  TESTES DO SHIELD 9 EM 1
// =============================================================================

// Liga a DESCARGA do capacitor de amostragem do ADC antes de cada conversão.
//
// Por quê: o ADC tem um capacitor interno que guarda a tensão do último
// pino lido. O LM35 fornece corrente bem, mas quase não consegue ABSORVER
// corrente. Se o pino lido antes estava alto (o LDR fica perto de 4,5V), o
// capacitor empurra o A2 para cima e o LM35 não consegue puxá-lo de volta.
// Medido na placa real (2026-10-03): o multímetro marcava 0,253 V no A2 e o
// ADC lia 0,77 V (77 °C!) depois de uma leitura do A1, e continuava errado
// até a placa reiniciar.
//
// O RA4M1 tem um recurso para isso: o registrador ADDISCR. Com ADNDIS = 15,
// o capacitor é descarregado para 0V por 15 ciclos antes de cada conversão.
// Partindo de 0V, o LM35 só precisa fornecer corrente. Com isso o ADC leu
// 0,259 V em qualquer ordem de leitura, e o LDR e o potenciômetro não
// mudaram. O core do Arduino reconfigura o ADC em algumas funções (ex:
// analogReference()), por isso este ajuste é feito logo antes das leituras.
void ligarDescargaDoAdc() {
  R_ADC0->ADDISCR = 0x0F;
}

// Média de 16 leituras do ADC (10 bits), para reduzir o ruído.
float lerAnalogicoMedio(uint8_t pino) {
  unsigned long soma = 0;
  for (uint8_t i = 0; i < 16; i++) {
    soma += analogRead(pino);
  }
  return soma / 16.0;
}

void testeShieldRepouso() {
  Serial.println("# Shield: nível de repouso das entradas digitais");
  // Botões: pull-up de ≈10 kΩ, ativos em LOW (medido). Solto = HIGH.
  bool sw1 = digitalRead(PINO_SW1) == HIGH;
  bool sw2 = digitalRead(PINO_SW2) == HIGH;
  if (sw1 && sw2) {
    resultado("botoes", "OK", "SW1 e SW2 soltos em HIGH (ativos em LOW)");
  } else {
    resultado("botoes", "AVISO", String("LOW em ") + (sw1 ? "" : "SW1 ") + (sw2 ? "" : "SW2 ") +
              "(botão apertado ou em curto)");
  }
  // Receptor IR: a saída fica em HIGH sem sinal de controle remoto.
  if (digitalRead(PINO_IR) == HIGH) {
    resultado("ir", "OK", "D6 em HIGH (repouso, sem sinal)");
  } else {
    resultado("ir", "AVISO", "D6 em LOW (recebendo IR agora, ou receptor com defeito)");
  }
}

// Escreve HIGH e LOW em cada saída do shield e lê de volta. Não prova que o
// LED acende (isso só o olho confirma, no teste interativo), mas pega pino
// em curto. Os LEDs piscam rapidamente durante este teste.
void testeShieldSaidas() {
  Serial.println("# Shield: saídas D9-D13 (os LEDs piscam rapidamente)");
  const uint8_t pinos[5] = {PINO_RGB_VERMELHO, PINO_RGB_AZUL, PINO_RGB_VERDE,
                            PINO_LED_D12, PINO_LED_D13};
  String falhas;
  for (uint8_t i = 0; i < 5; i++) {
    pinMode(pinos[i], OUTPUT);
    digitalWrite(pinos[i], HIGH);
    delay(80);
    bool highOk = digitalRead(pinos[i]) == HIGH;
    digitalWrite(pinos[i], LOW);
    delayMicroseconds(50);
    bool lowOk = digitalRead(pinos[i]) == LOW;
    if (!highOk || !lowOk) {
      falhas += nomePino(pinos[i]) + " ";
    }
  }
  falhas.trim();
  if (falhas.length() == 0) {
    resultado("saidas", "OK", "D9 D10 D11 D12 D13 seguem HIGH/LOW");
  } else {
    resultado("saidas", "FALHA", "não seguem o valor escrito: " + falhas);
  }
}

// Devolve true e preenche temperatura se o DHT11 respondeu.
bool testeShieldDht11(float &temperatura) {
  Serial.println("# Shield: DHT11 (até 3 tentativas)");
  float umidade = 0;
  uint8_t erro = DHT_SEM_RESPOSTA;
  for (uint8_t tentativa = 0; tentativa < 3 && erro != DHT_OK; tentativa++) {
    if (tentativa > 0) {
      delay(1200);  // o DHT11 não aceita leituras com menos de 1 s entre elas
    }
    erro = lerDht11(umidade, temperatura);
  }

  if (erro == DHT_SEM_RESPOSTA) {
    resultado("dht11", "FALHA", "não respondeu");
    return false;
  }
  if (erro != DHT_OK) {
    resultado("dht11", "FALHA", erro == DHT_ERRO_CHECKSUM ? "soma de verificação errada"
                                                          : "transmissão interrompida");
    return false;
  }
  String detalhe = String(temperatura, 1) + " °C, " + String(umidade, 0) + " %";
  bool plausivel = temperatura >= 0 && temperatura <= 50 && umidade >= 5 && umidade <= 95;
  resultado("dht11", plausivel ? "OK" : "AVISO", detalhe);
  return true;
}

void testeShieldAnalogicos(bool dhtValido, float temperaturaDht) {
  Serial.println("# Shield: entradas analógicas (10 bits, referência = AVCC medida)");
  analogReadResolution(10);

  // A referência padrão do ADC é a alimentação analógica (AVCC), que na USB
  // fica perto de 4,7V, e não 5,0V. O UNO R4 Minima mede a própria AVCC:
  // analogReference() sem argumento devolve esse valor (no core, via um
  // divisor interno lido contra a referência de 1,43V). Usar a AVCC medida
  // evita alguns graus de erro no LM35.
  float avcc = analogReference();
  if (isnan(avcc) || avcc < 3.0) {
    avcc = 5.0;  // UNO R4 WiFi: o core não mede a AVCC; supõe 5V
    resultado("avcc", "INFO", "não medida; supondo 5,00 V");
  } else {
    bool avccOk = avcc >= 4.5 && avcc <= 5.25;
    resultado("avcc", avccOk ? "OK" : "AVISO", String(avcc, 2) + " V (referência do ADC)");
  }

  ligarDescargaDoAdc();  // sem isso o LM35 lê alto (ver a função)

  // LM35: 10 mV por °C. A faixa aceita (10 a 40 °C) é a de uma sala de aula.
  // Fora dela, meça a tensão entre A2 e GND com um multímetro.
  //
  // map() faz a "regra de três": a leitura de 0 a 1023 vira uma tensão de
  // 0 a AVCC. Como map() só trabalha com números inteiros, a conta é feita
  // em milivolts (mV); em volts, tudo viraria 0 ou 4.
  long avccMv = lround(avcc * 1000);
  long mv = map(lround(lerAnalogicoMedio(PINO_LM35)), 0, 1023, 0, avccMv);
  float celsius = mv / 10.0;  // 10 mV por °C
  bool lm35Ok = celsius >= 10 && celsius <= 40;
  resultado("lm35", lm35Ok ? "OK" : "AVISO", String(celsius, 1) + " °C");

  // LDR: com luz ambiente, a leitura não deve estar colada em 0 nem em 1023.
  int ldr = (int)lerAnalogicoMedio(PINO_LDR);
  bool ldrOk = ldr > 10 && ldr < 1013;
  resultado("ldr", ldrOk ? "OK" : "AVISO", String(ldr) + " de 1023" +
            (ldrOk ? "" : " (saturado: luz forte demais ou defeito)"));

  // Potenciômetro: qualquer posição é válida; só informa.
  resultado("pot", "INFO", String(analogRead(PINO_POT)) + " de 1023");

  if (dhtValido && lm35Ok) {
    float diferenca = fabs(celsius - temperaturaDht);
    resultado("temperatura", diferenca <= 5 ? "OK" : "AVISO",
              "LM35 - DHT11 = " + String(diferenca, 1) + " °C");
  } else {
    resultado("temperatura", "PULADO", "LM35 ou DHT11 sem leitura válida");
  }
}

// =============================================================================
//  ROTEIRO
// =============================================================================
void rodarTestes() {
  zerarResultados();

  Serial.println();
  Serial.print("INICIO;teste_uno_r4_automatico;");
  Serial.println(VERSAO);

  testeInfo();
  testeRelogio();
  testeEeprom();
  testeRtc();

  uint8_t pullups = contarPullupsDoShield();
  bool comShield = pullups >= 3;
  bool semShield = pullups == 0;
  if (comShield) {
    resultado("shield", "INFO", "Shield 9 em 1 detectado (" + String(pullups) + "/4 pull-ups)");
  } else if (semShield) {
    resultado("shield", "INFO", "nenhum shield detectado");
  } else {
    // Algo puxa só alguns pinos: outro shield, fios ou um botão apertado.
    // Por segurança, nada é ligado como saída nos pinos do shield.
    resultado("shield", "AVISO", "montagem não reconhecida (" + String(pullups) +
              "/4 pull-ups): testes de pinos reduzidos");
  }

  if (semShield) {
    testeGpio(PINOS_SEM_SHIELD, sizeof(PINOS_SEM_SHIELD));
  } else {
    testeGpio(PINOS_COM_SHIELD, sizeof(PINOS_COM_SHIELD));
  }
  testeDac(semShield);
  testeSerial1();

  if (comShield) {
    testeShieldRepouso();
    testeShieldSaidas();
    float temperaturaDht = 0;
    bool dhtValido = testeShieldDht11(temperaturaDht);
    testeShieldAnalogicos(dhtValido, temperaturaDht);
  } else {
    resultado("shield", "PULADO", "testes do Shield 9 em 1 (shield ausente)");
  }

  imprimirFim();
}

// =============================================================================
//  SETUP E LOOP
// =============================================================================
void setup() {
  Serial.begin(115200);
}

// Quando um programa abre a porta (o Monitor Serial ou o script), aparece
// a mensagem de boas-vindas, e o teste só começa depois do comando "c".
// No UNO R4, "Serial" só fica verdadeiro quando o computador abre a porta
// (sinal DTR da USB), e abrir a porta não reinicia a placa, como acontecia
// no UNO R3. Esperar o comando evita que o teste acione os pinos sozinho,
// por exemplo quando a IDE reabre o Monitor depois de gravar um sketch.
bool conectadoAntes = false;

void loop() {
  bool conectado = Serial;
  if (conectado && !conectadoAntes) {
    delay(300);  // dá tempo do programa no PC começar a ler
    imprimirBoasVindas("TESTE AUTOMÁTICO DO UNO R4",
                       "ATENÇÃO: a placa deve estar SOZINHA ou só com o Shield 9 em 1.\n"
                       "O teste liga pinos como saída: tire módulos, fios e protoboard.");
  }
  conectadoAntes = conectado;

  if (conectado && pediuParaComecar()) {
    rodarTestes();
  }
}
