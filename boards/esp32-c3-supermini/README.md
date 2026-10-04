---
titulo: "ESP32-C3 SuperMini"
tipo: placa
autor: "Prof. Me. Joao Miguel Lac Roehe (@professorjoaomiguel)"
tags: [esp32, esp32-c3, supermini, risc-v, wifi, bluetooth, usb-c, usb-nativo, micropython, 3v3]
---

# ESP32-C3 SuperMini

## Resumo rápido

| Característica | Valor |
|---|---|
| Tensão lógica | **3,3V** (não tolera 5V) |
| Placa na IDE | **Nologo ESP32C3 Super Mini** (pacote esp32; FQBN `esp32:esp32:nologo_esp32c3_super_mini`), com USB CDC On Boot **Enabled** |
| Driver USB | Não precisa (USB nativo do ESP32-C3) |
| LED embutido | Azul, no GPIO8 (`LED_BUILTIN`); o nível que acende está **a confirmar** (a comunidade relata LOW) |
| Botões | **BOOT** (GPIO9) e **RST** |
| Cuidado nº 1 | Não é o Seeed XIAO ESP32C3: a pinagem é diferente, então tutoriais do XIAO não servem sem ajuste |

## Visão geral
Placa minúscula (22,5 × 18 mm) com o chip **ESP32-C3**: um núcleo
**[RISC-V](../../GLOSSARIO.md#risc-v-e-xtensa)** de 160 MHz, Wi-Fi 2,4 GHz e Bluetooth 5 LE, com 4 MB de flash
dentro do próprio chip. Tem 16 pinos: 13 GPIOs e 3 de alimentação. O
USB-C vai **direto no ESP32-C3** (USB nativo), sem chip conversor
USB-serial.

Serve para projetos pequenos de IoT, sensores sem fio e wearables, e
encaixa numa protoboard deixando uma fileira livre de cada lado.

**Não é igual ao Seeed Studio XIAO ESP32C3.** O tamanho e a ideia são
parecidos, e por isso ela costuma ser chamada de "clone do XIAO", mas a
pinagem é **diferente**. Exemplos e tutoriais do XIAO não funcionam
sem ajuste. Veja a comparação na seção **Diagrama esquemático / Pinout**.

## Tensão de operação
| Característica | Valor |
|---|---|
| Tensão lógica dos pinos | **3,3V** |
| Alimentação | USB-C 5V, ou 5V no pino **5V**, ou 3,3V no pino **3.3** |
| Regulador | regulador linear (LDO) de 5V para 3,3V; modelo e corrente máxima **a confirmar** |
| Tolera 5V nas entradas? | **Não**: máximo de 3,6V nos GPIOs |
| Corrente máxima por pino | 40 mA (máximo absoluto); prefira até 20 mA |

> ⚠️ **Não ligue sinais de 5V nos GPIOs.** Módulos de 5V (sensores,
> displays, o [Shield Multifunção 9 em 1](../../shields/uno-shield-9in1/README.md))
> precisam de conversor de nível lógico ou divisor resistivo nas entradas.

> ⚠️ **Não alimente pelo pino 5V com o USB ligado ao mesmo tempo.** Não
> está confirmado se a placa tem diodo de proteção entre o USB e o pino
> 5V. Sem diodo, a fonte externa e a porta USB do computador ficam
> ligadas direto uma na outra.

Compatibilidade no repositório:
- [Shield Multifunção 9 em 1 (UNO)](../../shields/uno-shield-9in1/README.md):
  **não compatível** (formato e tensão diferentes).

## Fotos
_Ainda sem foto própria._ Previstas, na pasta `imagens/`:
`vista-de-cima.jpg` (a placa de cima, com a antena vermelha "C3"),
`serigrafia.jpg` (o lado com os números dos GPIOs) e `conector-usb.jpg`
(o USB-C e os botões BOOT e RST). Fotos do produto estão no anúncio do
vendedor (link em **Referências**). A pinagem está em texto logo abaixo.

## Diagrama esquemático / Pinout
O fabricante não publica o esquemático. A pinagem abaixo vem da
serigrafia da placa e do diagrama do anúncio, e confere com a definição
da placa **Nologo ESP32C3 Super Mini** no pacote esp32 da Arduino IDE
(arquivo `variants/nologo_esp32c3_super_mini/pins_arduino.h`).

A serigrafia mostra só o **número do [GPIO](../../GLOSSARIO.md#gpio)** (`0`, `1`, ... `21`). No
código, use esse número: `pinMode(4, OUTPUT)`.

Vista **de cima**, com o **USB-C para cima**:

| Lado esquerdo | GPIO | Função padrão | | Lado direito | GPIO | Função padrão |
|---|---|---|---|---|---|---|
| 5 | 5 | A5 (ADC2), [SPI](../../GLOSSARIO.md#spi) MISO | | 5V | — | Entrada/saída de 5V (ligado ao USB) |
| 6 | 6 | SPI MOSI | | G | — | Terra (GND) |
| 7 | 7 | SPI SS (CS) | | 3.3 | — | Saída de 3,3V do regulador |
| 8 | 8 | **[I2C](../../GLOSSARIO.md#i2c) SDA**, **LED azul**, ⚠️ *[strapping](../../GLOSSARIO.md#strapping-pinos-de)* | | 4 | 4 | A4 (ADC1), SPI SCK |
| 9 | 9 | **I2C SCL**, **botão BOOT**, ⚠️ *strapping* | | 3 | 3 | A3 (ADC1) |
| 10 | 10 | | | 2 | 2 | A2 (ADC1), ⚠️ *strapping* |
| 20 | 20 | UART0 RX | | 1 | 1 | A1 (ADC1) |
| 21 | 21 | UART0 TX | | 0 | 0 | A0 (ADC1) |

No diagrama do anúncio, o lado dos pinos 5V–0 aparece à esquerda porque
o desenho mostra a placa vista por baixo. Confira sempre pela serigrafia.

### Pinos que exigem cuidado

- **GPIO8: LED e I2C no mesmo pino.** O LED azul da placa está no GPIO8,
  que também é o SDA padrão do I2C. Num projeto com I2C, o LED pisca junto
  com a comunicação. Isso é normal.
- **Pinos de *strapping* (GPIO2, GPIO8 e GPIO9):** o ESP32-C3 lê o nível
  desses pinos no reset para decidir como iniciar. Com o GPIO9 em LOW no
  reset (botão BOOT apertado), a placa entra em modo de gravação. Não
  ligue nesses pinos nada que os force para LOW durante o reset. Os
  resistores de [pull-up](../../GLOSSARIO.md#pull-up-e-pull-down) do I2C (GPIO8/9) não atrapalham.
- **Entradas analógicas:** use **A0–A4 (GPIO0 a GPIO4)**, que são do
  ADC1. O A5 (GPIO5) é do ADC2, que no ESP32-C3 tem leitura pouco
  confiável e não funciona junto com o Wi-Fi. Todas leem de **0 a 3,3V**,
  com 12 bits (`analogRead` devolve de 0 a 4095).
- **GPIO20 e GPIO21 (RX/TX):** são a UART0. Como o [Monitor Serial](../../GLOSSARIO.md#monitor-serial) passa
  pelo USB nativo, eles ficam livres para ligar outro módulo serial (GPS,
  por exemplo).

### Comparação com o Seeed Studio XIAO ESP32C3

| | ESP32-C3 SuperMini | XIAO ESP32C3 (Seeed) |
|---|---|---|
| Tamanho | 22,5 × 18 mm | 21 × 17,8 mm |
| Pinos | 16 (13 GPIOs) | 14 (11 GPIOs) |
| I2C padrão | SDA = GPIO8, SCL = GPIO9 | SDA = GPIO6 (D4), SCL = GPIO7 (D5) |
| SPI padrão | SCK 4, MISO 5, MOSI 6, SS 7 | SCK 8 (D8), MISO 9 (D9), MOSI 10 (D10) |
| Serial (TX/RX) | GPIO21 / GPIO20 | GPIO21 (D6) / GPIO20 (D7) |
| Nomes na IDE | só números de GPIO | D0–D10 |
| LED do usuário | sim, GPIO8 | não (só LED de carga) |
| Antena | cerâmica, na placa | externa (conector U.FL) |
| Bateria | não | carregador de LiPo 3,7V |

Um código escrito para o XIAO com os nomes `D0`–`D10` ou com o I2C
padrão (`Wire.begin()`) **não** usa os mesmos pinos físicos nesta placa.

## Componentes principais
- Chip **ESP32-C3** (encapsulamento QFN32 de 5 × 5 mm), com 4 MB de flash
  interna (versão FH4/FN4): núcleo RISC-V de 32 bits a 160 MHz, 400 KB de
  SRAM, Wi-Fi 802.11 b/g/n (2,4 GHz) e Bluetooth 5 LE
- **Sem [PSRAM](../../GLOSSARIO.md#psram)**
- Antena cerâmica na placa (componente vermelho marcado "C3")
- Cristal de 40 MHz
- Regulador de 3,3V (SOT-23-5)
- Conector **USB-C** ligado ao **USB nativo** do ESP32-C3 (USB Serial/JTAG)
- **LED azul** no GPIO8 e LED vermelho de alimentação (PWR)
- Botões **BOOT** (GPIO9) e **RST** (reset)
- Dimensões: 22,52 × 18,00 mm; fileiras de pinos a 15,24 mm (0,6")

## Funcionalidades / Periféricos
- Wi-Fi 2,4 GHz e Bluetooth 5 LE
- GPIOs com [PWM](../../GLOSSARIO.md#pwm) (LEDC), [ADC](../../GLOSSARIO.md#adc) de 12 bits, I2C, SPI, [UART](../../GLOSSARIO.md#uart), I2S e CAN (TWAI)
- USB nativo: gravação e Monitor Serial sem chip conversor
- Modos de baixo consumo (deep sleep)
- Programável em Arduino (C/C++) e em [MicroPython](../../GLOSSARIO.md#micropython) (ver **Como
  programar**); também aceita ESP-IDF

### Problema conhecido: Wi-Fi fraco

A comunidade relata que muitas unidades desta placa conectam mal no
Wi-Fi, ou só funcionam bem perto do roteador. A causa apontada é o
projeto da antena, e a solução mais citada é **reduzir a potência de
transmissão** logo depois de ligar o Wi-Fi:

```cpp
WiFi.begin(ssid, senha);
WiFi.setTxPower(WIFI_POWER_8_5dBm);
```

```python
wlan.active(True)
wlan.config(txpower=8.5)
```

Ainda **não testado** nas placas do laboratório (ver **A confirmar**).

## Como programar

Esta placa é usada em aula de duas formas:

- **Arduino (C/C++)**, pela Arduino IDE ou pelo `arduino-cli`;
- **MicroPython**, pelo editor **Thonny**.

Como o USB é nativo, **não há driver para instalar**: a placa aparece
como uma porta COM ("Dispositivo Serial USB") assim que é ligada.

**Se a gravação falhar** ("Failed to connect" / "No serial data
received"): segure o botão **BOOT**, aperte e solte o **RST**, solte o
BOOT e grave de novo. Depois da gravação, aperte **RST** para o programa
começar. Isso também resolve a placa "sumir" da lista de portas quando
um programa trava o USB.

### Antes de programar: identificar o chip (ESPConnect)

O [ESPConnect](https://thelastoutpostworkshop.github.io/ESPConnect/) lê as
informações do chip direto pelo navegador (Chrome, Edge ou Brave). Feche a
Arduino IDE e o Thonny, clique em **Connect**, escolha a porta da placa e
veja a aba **Device Info**: família (ESP32-C3), revisão, [MAC](../../GLOSSARIO.md#mac) e tamanho da
flash. Use só essa aba: as outras gravam e apagam a flash.

### Arduino (C/C++): Arduino IDE

Instale o pacote **esp32** (Espressif Systems) no Gerenciador de Placas e
use no menu **Ferramentas**:

| Opção | Valor |
|---|---|
| Placa | **Nologo ESP32C3 Super Mini** |
| USB CDC On Boot | **Enabled** (já vem assim nesta placa) |
| Porta | a porta COM da placa |

A placa "Nologo ESP32C3 Super Mini" já traz a pinagem certa (`LED_BUILTIN`
= 8, `SDA` = 8, `SCL` = 9, `A0`–`A5`). Também funciona **ESP32C3 Dev
Module**, desde que **USB CDC On Boot** fique em **Enabled**. Sem isso,
nada aparece no Monitor Serial.

> Não use a placa **XIAO_ESP32C3** da lista: ela compila, mas com a
> pinagem do XIAO (`SDA` = 6, `SCL` = 7, sem `LED_BUILTIN`).

Com o USB nativo, o Monitor Serial só recebe dados depois que o
computador abre a porta. Por isso os sketches esperam um pouco no
`setup()`:

```cpp
Serial.begin(115200);
while (!Serial && millis() < 3000) delay(10);
```

### Arduino (C/C++): arduino-cli

```sh
# 1. Instalar o pacote esp32 (uma vez só)
arduino-cli config add board_manager.additional_urls https://espressif.github.io/arduino-esp32/package_esp32_index.json
arduino-cli core update-index
arduino-cli core install esp32:esp32

# 2. Descobrir a porta da placa
arduino-cli board list

# 3. Compilar, gravar e abrir o Monitor Serial (troque COM7 pela sua porta)
arduino-cli compile --fqbn esp32:esp32:nologo_esp32c3_super_mini boards/esp32-c3-supermini/code/teste_esp32_c3_supermini
arduino-cli upload  --fqbn esp32:esp32:nologo_esp32c3_super_mini -p COM7 boards/esp32-c3-supermini/code/teste_esp32_c3_supermini
arduino-cli monitor -p COM7 -c baudrate=115200
```

**Erro `cannot execute 'cc1plus.exe'` ou "Uma política de Controle de
Aplicativo bloqueou este arquivo":** o Windows (Smart App Control ou uma
política da instituição) bloqueou o compilador **RISC-V** do pacote esp32
(`tools/esp-rv32/.../cc1plus.exe`). Isso afeta o ESP32-C3, mas não o
ESP32-S3, que usa outro compilador (Xtensa). Não é erro no código: peça
ao administrador da máquina para liberar a pasta do pacote esp32.

### MicroPython: Thonny

**Qual firmware gravar:** o firmware oficial **`ESP32_GENERIC_C3`**,
release estável mais recente, em
https://micropython.org/download/ESP32_GENERIC_C3/. O ESP32-C3 não tem
PSRAM, então não há variante para escolher.

1. Instale o **Thonny** (https://thonny.org) e ligue a placa no USB.
2. Em **Ferramentas > Opções > Interpretador**, escolha
   **MicroPython (ESP32)** e a porta da placa.
3. Clique em **Instalar ou atualizar MicroPython (esptool)**, escolha a
   família **ESP32-C3** e a variante **Espressif • ESP32-C3**, e instale.
   Se não começar, entre no modo de gravação (BOOT + RST, acima).
   Pela linha de comando, o equivalente é:

   ```sh
   esptool --chip esp32c3 --port COM7 erase_flash
   esptool --chip esp32c3 --port COM7 --baud 460800 write_flash 0 ESP32_GENERIC_C3-<data>-<versão>.bin
   ```

4. Depois de gravar, aperte **RST**. O **Shell** do Thonny mostra o
   prompt `>>>`. Confira:

   ```python
   import os, esp
   os.uname()          # versão do MicroPython
   esp.flash_size()    # 4194304 (4 MB)
   ```

5. Para usar os pinos, use o número do GPIO:
   `machine.Pin(8, machine.Pin.OUT)`.

## Código de teste e validação

| Formato | Arquivo | Como rodar |
|---|---|---|
| Arduino (C/C++) | [`code/teste_esp32_c3_supermini/teste_esp32_c3_supermini.ino`](code/teste_esp32_c3_supermini/teste_esp32_c3_supermini.ino) | Arduino IDE ou `arduino-cli` (ver acima) |
| MicroPython | [`code/teste_esp32_c3_supermini_micropython/main.py`](code/teste_esp32_c3_supermini_micropython/main.py) | Abrir no Thonny e executar (F5) |

Os dois testes fazem a mesma coisa: mostram os dados do chip (modelo,
flash, MAC), piscam o LED azul (GPIO8) mostrando o nível do pino, para
descobrir se ele acende com LOW ou HIGH, e avisam quando o botão BOOT
(GPIO9) é apertado. Nenhum precisa de biblioteca extra.

Estado da verificação: o sketch Arduino **ainda não foi compilado**, porque
o Windows da máquina de desenvolvimento bloqueou o compilador RISC-V (ver
acima). O script MicroPython teve só a sintaxe verificada. Nenhum dos dois
rodou numa placa ainda.

### A confirmar na placa real

| Item | Indício (fabricante / comunidade) | Resultado do teste |
|---|---|---|
| Chip e flash (ESPConnect) | ESP32-C3, 4 MB de flash interna | — |
| LED azul no GPIO8 e nível que acende | serigrafia "IO8"; a comunidade relata ativo em LOW | — |
| Botão BOOT no GPIO9 | serigrafia e anúncio | — |
| Sketch Arduino compila e roda | — | compilação bloqueada pelo Windows |
| Script MicroPython roda | firmware `ESP32_GENERIC_C3` | — |
| Wi-Fi conecta com potência normal | relatos de Wi-Fi fraco | — |
| Wi-Fi com `WIFI_POWER_8_5dBm` | solução citada pela comunidade | — |
| Modelo e corrente máxima do regulador 3,3V | SOT-23-5, modelo não identificado | ler a marcação do chip |
| Diodo entre o USB e o pino 5V | não informado | medir (multímetro) |

## Referências
- Datasheet do ESP32-C3 (Espressif):
  https://www.espressif.com/sites/default/files/documentation/esp32-c3_datasheet_en.pdf
- Pinagem da placa na Arduino IDE (pacote esp32, arquivo
  `pins_arduino.h` da variante `nologo_esp32c3_super_mini`):
  https://github.com/espressif/arduino-esp32/blob/master/variants/nologo_esp32c3_super_mini/pins_arduino.h
- Anúncio do vendedor (AliExpress), com o diagrama de pinagem e as medidas:
  https://pt.aliexpress.com/item/1005012179601988.html
- Firmware MicroPython para ESP32-C3 (oficial):
  https://micropython.org/download/ESP32_GENERIC_C3/
- ESPConnect (identificação do chip pelo navegador):
  https://thelastoutpostworkshop.github.io/ESPConnect/
- Seeed Studio XIAO ESP32C3, para comparação (pinagem diferente):
  https://wiki.seeedstudio.com/XIAO_ESP32C3_Getting_Started/

---

**Autor:** Prof. Me. Joao Miguel Lac Roehe ([@professorjoaomiguel](https://github.com/professorjoaomiguel)). Documentação sob licença [CC BY-NC 4.0](https://creativecommons.org/licenses/by-nc/4.0/deed.pt-br); código em `code/` sob licença MIT. Veja como citar no [README principal](../../README.md).
