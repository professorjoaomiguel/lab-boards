/*
 * =============================================================================
 *  Teste básico da placa ESP32-C3 SuperMini
 * =============================================================================
 *
 *  Autor: Prof. Joao Miguel Roehe (@professorjoaomiguel)
 *  Licença: MIT (SPDX-License-Identifier: MIT) — ver LICENSE-CODE na raiz
 *
 *  O QUE ESTE SKETCH FAZ
 *  ---------------------
 *  1. Mostra no Monitor Serial os dados do chip: modelo, revisão, número
 *     de núcleos, frequência, tamanho da flash e endereço MAC do Wi-Fi.
 *  2. Pisca o LED azul da placa (GPIO8) e mostra no Monitor Serial o nível
 *     lógico do pino a cada troca. Assim o aluno descobre se o LED acende
 *     com LOW ou com HIGH (LED "ativo em LOW" ou "ativo em HIGH").
 *  3. Avisa no Monitor Serial cada vez que o botão BOOT (GPIO9) é
 *     apertado. Depois que a placa iniciou, o BOOT pode ser usado como um
 *     botão comum.
 *
 *  O QUE OBSERVAR
 *  --------------
 *    - "Modelo: ESP32-C3", "Núcleos: 1" e "Flash: 4 MB".
 *    - Em qual nível (LOW ou HIGH) o LED azul fica ACESO. Anote no README.
 *    - A mensagem "BOOT apertado" ao apertar o botão BOOT.
 *
 *  CONFIGURAÇÃO NA ARDUINO IDE
 *  ---------------------------
 *    Placa ........... Nologo ESP32C3 Super Mini   (pacote "esp32" da
 *                      Espressif). Alternativa: ESP32C3 Dev Module com
 *                      "USB CDC On Boot" = Enabled.
 *    USB CDC On Boot . Enabled   (esta placa não tem chip USB-serial: o
 *                                 USB-C vai direto no ESP32-C3)
 *    Monitor Serial .. 115200 baud
 *
 *    Pela linha de comando (arduino-cli):
 *      arduino-cli compile --fqbn esp32:esp32:nologo_esp32c3_super_mini \
 *        boards/esp32-c3-supermini/code/teste_esp32_c3_supermini
 *
 *  SE O UPLOAD FALHAR: segure BOOT, aperte e solte RST, solte BOOT e grave
 *  de novo. Depois da gravação, aperte RST para rodar o programa.
 *
 *  Nenhuma biblioteca extra é necessária.
 *
 *  ATENÇÃO: esta placa trabalha em 3,3V. Não ligue sinais de 5V nos GPIOs.
 * =============================================================================
 */

// GPIO do LED azul da placa (serigrafia "IO8"). É o mesmo pino do SDA do
// I2C: num projeto com I2C, o LED pisca junto com a comunicação.
const uint8_t PINO_LED = 8;

// GPIO do botão BOOT. O botão liga o pino ao GND, então apertado = LOW.
const uint8_t PINO_BOOT = 9;

// Tempo entre as trocas do LED, em milissegundos.
const unsigned long INTERVALO_LED_MS = 1000;

unsigned long ultimaTrocaLed = 0;
bool nivelLed = LOW;
bool bootAnterior = HIGH;

/*
 * Mostra no Monitor Serial os dados do chip e da flash.
 */
void mostrarDadosDoChip() {
  Serial.println();
  Serial.println(F("=== ESP32-C3 SuperMini: dados do chip ==="));
  Serial.printf("Modelo ........ %s (revisão %d)\n", ESP.getChipModel(), ESP.getChipRevision());
  Serial.printf("Núcleos ....... %d\n", ESP.getChipCores());
  Serial.printf("Frequência .... %lu MHz\n", (unsigned long)ESP.getCpuFreqMHz());
  Serial.printf("Flash ......... %lu MB\n", (unsigned long)(ESP.getFlashChipSize() / (1024UL * 1024UL)));

  // O MAC fica gravado de fábrica no chip (eFuse); os 6 bytes identificam
  // a placa na rede Wi-Fi.
  uint64_t mac = ESP.getEfuseMac();
  Serial.printf("MAC (eFuse) ... %02X:%02X:%02X:%02X:%02X:%02X\n",
                (uint8_t)(mac), (uint8_t)(mac >> 8), (uint8_t)(mac >> 16),
                (uint8_t)(mac >> 24), (uint8_t)(mac >> 32), (uint8_t)(mac >> 40));
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  // Com o USB nativo, a porta serial só existe depois que o computador a
  // abre. Espera até 3 s pelo Monitor Serial, sem travar se ele não abrir.
  unsigned long inicio = millis();
  while (!Serial && millis() - inicio < 3000) {
    delay(10);
  }

  pinMode(PINO_LED, OUTPUT);
  pinMode(PINO_BOOT, INPUT_PULLUP);

  mostrarDadosDoChip();
  Serial.println(F("LED azul (GPIO8) troca de nível a cada segundo."));
  Serial.println(F("Anote em qual nível ele fica ACESO. Aperte BOOT para testar o botão."));
}

void loop() {
  // Troca o LED sem usar delay(), para o botão continuar sendo lido.
  if (millis() - ultimaTrocaLed >= INTERVALO_LED_MS) {
    ultimaTrocaLed = millis();
    nivelLed = !nivelLed;
    digitalWrite(PINO_LED, nivelLed);
    Serial.printf("LED: GPIO8 = %s\n", nivelLed ? "HIGH" : "LOW");
  }

  // Detecta só a borda de descida (solto -> apertado), para avisar uma
  // vez por aperto e não a cada volta do loop.
  bool bootAgora = digitalRead(PINO_BOOT);
  if (bootAnterior == HIGH && bootAgora == LOW) {
    Serial.println(F("BOOT apertado"));
  }
  bootAnterior = bootAgora;
  delay(10);  // filtro simples contra o repique (bounce) do botão
}
