/*
 * =============================================================================
 *  Teste básico da placa ESP32-S3 UNO (TZT D1 ESP32-S3 N16R8)
 * =============================================================================
 *
 *  Autor: Prof. Joao Miguel Roehe (@professorjoaomiguel)
 *  Licença: MIT (SPDX-License-Identifier: MIT) — ver LICENSE-CODE na raiz
 *
 *  O QUE ESTE SKETCH FAZ
 *  ---------------------
 *  1. Mostra no Monitor Serial os dados do chip: modelo, número de núcleos,
 *     frequência, tamanho da memória flash e da PSRAM. Serve para confirmar
 *     que a placa é mesmo uma N16R8 (16 MB de flash e 8 MB de PSRAM) e que
 *     a IDE foi configurada corretamente.
 *  2. Pisca o LED RGB endereçável (WS2812) da placa, ligado no GPIO48,
 *     passando por vermelho, verde, azul e branco.
 *
 *  O QUE OBSERVAR
 *  --------------
 *    - "Flash: 16 MB" e "PSRAM: 8 MB" no Monitor Serial (versão N16R8).
 *      A versão N8R2 mostra 8 MB e 2 MB; nela, use Flash Size "8MB" e
 *      PSRAM "QSPI PSRAM".
 *      PSRAM = 0 MB  -> a opção PSRAM não está em "OPI PSRAM" (ver abaixo).
 *      Flash = 4 MB  -> a opção Flash Size não está em "16MB".
 *    - O LED RGB trocando de cor a cada segundo, na ordem indicada no
 *      Monitor Serial. Se as cores saírem trocadas (ex: verde quando o
 *      Monitor diz vermelho), anote no README da placa.
 *
 *  CONFIGURAÇÃO NA ARDUINO IDE
 *  ---------------------------
 *    Placa ........... ESP32S3 Dev Module   (pacote "esp32" da Espressif)
 *    Flash Size ...... 16MB (128Mb)
 *    PSRAM ........... OPI PSRAM
 *    USB CDC On Boot . Disabled   (a porta USB-C passa pelo chip CH340,
 *                                  ligado na UART0: TXD=GPIO43, RXD=GPIO44)
 *    Monitor Serial .. 115200 baud
 *
 *    Pela linha de comando (arduino-cli):
 *      arduino-cli compile --fqbn esp32:esp32:esp32s3:FlashSize=16M,PSRAM=opi \
 *        boards/esp32-s3-uno/code/teste_esp32_s3_uno
 *
 *  SE O UPLOAD FALHAR ("Failed to connect"): a placa não tem botão BOOT.
 *  Ligue o pino IO0 ao GND com um jumper, aperte e solte RST, faça o
 *  upload, retire o jumper e aperte RST de novo.
 *
 *  Nenhuma biblioteca extra é necessária: rgbLedWrite() já vem no pacote
 *  "esp32" (versão 3.x).
 *
 *  ATENÇÃO: esta placa trabalha em 3,3V, mesmo tendo o formato do UNO. Não
 *  encaixe shields de 5V nela. Ver ../../README.md.
 * =============================================================================
 */

// GPIO onde está o LED RGB endereçável (WS2812) da placa. A serigrafia
// da placa indica "SW2812 (IO48)".
const uint8_t PINO_LED_RGB = 48;

// Brilho de cada cor (0 a 255). Um valor baixo evita ofuscar e consome
// menos corrente; o WS2812 é muito brilhante mesmo em 40.
const uint8_t BRILHO = 40;

// Uma cor do teste: nome para o Monitor Serial e intensidade de cada canal.
struct Cor {
  const char *nome;
  uint8_t vermelho;
  uint8_t verde;
  uint8_t azul;
};

const Cor CORES[] = {
  {"vermelho", BRILHO, 0, 0},
  {"verde", 0, BRILHO, 0},
  {"azul", 0, 0, BRILHO},
  {"branco", BRILHO, BRILHO, BRILHO},
};
const size_t NUM_CORES = sizeof(CORES) / sizeof(CORES[0]);

/*
 * Converte bytes para megabytes (MB), arredondando para baixo.
 *
 *   bytes: quantidade de memória em bytes.
 *   Retorna o valor em MB (1 MB = 1024 * 1024 bytes).
 */
uint32_t emMB(uint32_t bytes) {
  return bytes / (1024UL * 1024UL);
}

/*
 * Mostra no Monitor Serial os dados do chip e das memórias.
 */
void mostrarDadosDoChip() {
  Serial.println();
  Serial.println(F("=== ESP32-S3 UNO: dados do chip ==="));
  Serial.printf("Modelo ........ %s (revisão %d)\n", ESP.getChipModel(), ESP.getChipRevision());
  Serial.printf("Núcleos ....... %d\n", ESP.getChipCores());
  Serial.printf("Frequência .... %lu MHz\n", (unsigned long)ESP.getCpuFreqMHz());
  Serial.printf("Flash ......... %lu MB\n", (unsigned long)emMB(ESP.getFlashChipSize()));
  // getPsramSize() devolve 0 se a PSRAM não foi habilitada na IDE.
  Serial.printf("PSRAM ......... %lu MB\n", (unsigned long)emMB(ESP.getPsramSize()));

  if (ESP.getPsramSize() == 0) {
    Serial.println(F("AVISO: PSRAM não encontrada. Em Ferramentas > PSRAM, escolha \"OPI PSRAM\"."));
  }
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  // Pequena espera para dar tempo de abrir o Monitor Serial depois do reset.
  delay(1000);
  mostrarDadosDoChip();
  Serial.println(F("Teste do LED RGB (GPIO48): a cor muda a cada segundo."));
}

void loop() {
  for (size_t i = 0; i < NUM_CORES; i++) {
    Serial.printf("LED RGB: %s\n", CORES[i].nome);
    rgbLedWrite(PINO_LED_RGB, CORES[i].vermelho, CORES[i].verde, CORES[i].azul);
    delay(1000);
  }
  // Apaga por um segundo para marcar o fim de cada ciclo.
  Serial.println(F("LED RGB: apagado"));
  rgbLedWrite(PINO_LED_RGB, 0, 0, 0);
  delay(1000);
}
