---
titulo: "ESP32-S3 UNO (TZT D1 ESP32-S3 N16R8)"
tipo: placa
autor: "Prof. Me. Joao Miguel Lac Roehe (@professorjoaomiguel)"
tags: [esp32, esp32-s3, formato-uno, wifi, bluetooth, usb-c, ch340, ws2812, psram, micropython, 3v3]
---

# ESP32-S3 UNO (TZT D1 ESP32-S3 N16R8)

## Resumo rápido

| Característica | Valor |
|---|---|
| Tensão lógica | **3,3V** (não tolera 5V, apesar do formato de UNO) |
| Placa na IDE | **ESP32S3 Dev Module** (pacote esp32); na N16R8: Flash Size 16MB e PSRAM **OPI PSRAM** (FQBN `esp32:esp32:esp32s3:FlashSize=16M,PSRAM=opi`) |
| Driver USB | CH340 (Windows Update ou [driver da WCH](https://www.wch-ic.com/downloads/CH341SER_EXE.html)) |
| LED embutido | Não há LED simples: LED RGB endereçável WS2812 no GPIO48 (a confirmar na placa) |
| Botões | Só **RST**. Não há BOOT: a gravação é automática pelo CH340 |
| Cuidado nº 1 | Não encaixe shields de 5V, como o Shield 9 em 1: o encaixe é perfeito, mas o 5V queima os GPIOs |

## Visão geral
Placa de desenvolvimento com o módulo **ESP32-S3-WROOM-1** montado no
**formato do Arduino UNO**: mesmo tamanho, mesmos furos de fixação e
barras de pinos nas mesmas posições. Vendida como "TZT D1 ESP32-S3" ou
"ESP32-S3 UNO D1 R3".

Ela junta o processador do ESP32-S3 (dois núcleos de 240 MHz, Wi-Fi e
Bluetooth) com a disposição de pinos que os alunos já conhecem do UNO.
Serve para projetos de IoT, Wi-Fi, Bluetooth e processamento mais pesado
(ex: imagem e voz com a [PSRAM](../../GLOSSARIO.md#psram)).

> ⚠️ **O formato é de UNO, mas a tensão não é.** Os pinos trabalham em
> **3,3V**. Leia a seção **Tensão de operação** antes de encaixar qualquer
> shield.

**Variantes:** o mesmo anúncio vende a versão **[N16R8](../../GLOSSARIO.md#n16r8)** (16 MB de flash e
8 MB de PSRAM) e a **N8R2** (8 MB de flash e 2 MB de PSRAM). A diferença
está só no módulo. Confira a gravação na tampa metálica do módulo
(`ESP32-S3-N16R8` ou `ESP32-S3-N8R2`) ou rode o sketch de teste, que
mostra os tamanhos de memória.

Não confundir com o [ESP32-S3 N16R8 DevKit](../esp32-s3-n16r8/README.md),
que é a placa estreita, de encaixar na protoboard.

## Tensão de operação
| Característica | Valor |
|---|---|
| Tensão lógica dos pinos | **3,3V** |
| Alimentação | USB-C 5V, ou conector DC de **5 a 18V** (segundo o fabricante), ou pino [VIN](../../GLOSSARIO.md#vin) |
| Reguladores | Conversor chaveado (DC-DC) para 5V + regulador AMS1117 para 3,3V |
| Tolera 5V nas entradas? | **Não**: máximo de 3,6V nos GPIOs |
| Corrente máxima por pino | 40 mA (máximo absoluto); prefira até 20 mA |

> ⚠️ **Não encaixe shields de 5V nesta placa**, como o
> [Shield Multifunção 9 em 1](../../shields/uno-shield-9in1/README.md).
> O encaixe físico é perfeito, e por isso o risco é maior: um sinal de 5V
> vindo do shield para um [GPIO](../../GLOSSARIO.md#gpio) pode queimar a porta ou o ESP32-S3.
>
> Dois detalhes do formato UNO que enganam:
> - A posição do **[IOREF](../../GLOSSARIO.md#ioref)** (que num UNO avisa o shield de que a lógica é
>   5V) está ligada a **5V**. Um shield que se adapta pelo IOREF vai achar
>   que pode mandar 5V.
> - A posição do **[AREF](../../GLOSSARIO.md#aref)** está ligada ao **RST** (reset). Um shield que
>   use o AREF vai mexer no reset da placa.
>
> Para usar módulos de 5V, use um conversor de nível lógico (level
> shifter) ou um divisor resistivo nas entradas.

Os pinos **5V** e **3V3** da barra de alimentação são saídas para
alimentar módulos. A corrente disponível em cada um não foi medida.

Compatibilidade no repositório:
- [Shield Multifunção 9 em 1 (UNO)](../../shields/uno-shield-9in1/README.md):
  **não compatível** (shield de 5V).

## Fotos
_Ainda sem foto própria._ Previstas, na pasta `imagens/`:
`vista-de-cima.jpg` (a placa inteira, de cima), `modulo.jpg` (a gravação
na tampa do módulo: N16R8 ou N8R2) e `serigrafia.jpg` (os nomes dos pinos
`IOxx`). Fotos do produto estão nos links de **Referências** (anúncio do
vendedor). A pinagem está em texto logo abaixo.

## Diagrama esquemático / Pinout
O fabricante não publica o esquemático. A pinagem abaixo foi levantada da
serigrafia da placa e do diagrama do anúncio do vendedor, e confere com os
pinos padrão do ESP32-S3 no Arduino ([SPI](../../GLOSSARIO.md#spi), [I2C](../../GLOSSARIO.md#i2c) e serial).

Nesta placa, os nomes da serigrafia são os **números de GPIO** (ex: `IO18`
= GPIO18). No código, use sempre o número do GPIO: `pinMode(18, OUTPUT)`.
Nomes de UNO como `D2` ou `A0` **não existem** para esta placa na IDE.

### Barra digital (lado oposto à alimentação)

| Posição no UNO | Serigrafia | GPIO | Função padrão / observação |
|---|---|---|---|
| D0 | RXD | 44 | UART0 RX, ligado ao [CH340](../../GLOSSARIO.md#ch340) ([Monitor Serial](../../GLOSSARIO.md#monitor-serial)) |
| D1 | TXD | 43 | UART0 TX, ligado ao CH340 (Monitor Serial) |
| D2 | IO18 | 18 | |
| D3 | IO17 | 17 | |
| D4 | IO19 | 19 | USB nativo D− do ESP32-S3 |
| D5 | IO20 | 20 | USB nativo D+ do ESP32-S3 |
| D6 | IO3 | 3 | ⚠️ pino de *[strapping](../../GLOSSARIO.md#strapping-pinos-de)* (ver abaixo) |
| D7 | IO14 | 14 | |
| D8 | IO21 | 21 | |
| D9 | IO46 | 46 | ⚠️ pino de *strapping* (ver abaixo) |
| D10 | IO10 | 10 | SPI SS (CS) |
| D11 | IO11 | 11 | SPI MOSI |
| D12 | IO13 | 13 | SPI MISO |
| D13 | IO12 | 12 | SPI SCK |
| GND | GND | — | Terra |
| AREF | RST | — | ⚠️ Reset da placa, **não** é referência analógica |
| SDA | IO8 | 8 | I2C SDA (padrão do `Wire`) |
| SCL | IO9 | 9 | I2C SCL (padrão do `Wire`) |

### Barra de alimentação e barra analógica

| Posição no UNO | Serigrafia | GPIO | Função padrão / observação |
|---|---|---|---|
| NC | IO0 | 0 | ⚠️ pino de *strapping*: ligado ao GND no reset, entra em modo de gravação |
| IOREF | 5V | — | ⚠️ Saída de 5V (num UNO seria o IOREF) |
| RESET | RST | — | Reset da placa |
| 3.3V | 3V3 | — | Saída de 3,3V |
| 5V | 5V | — | Saída de 5V |
| GND | GND | — | Terra |
| GND | GND | — | Terra |
| VIN | VIN | — | Entrada de alimentação (mesma do conector DC) |
| A0 | IO2 | 2 | ADC1 |
| A1 | IO1 | 1 | ADC1 |
| A2 | IO7 | 7 | ADC1 |
| A3 | IO6 | 6 | ADC1 |
| A4 | IO5 | 5 | ADC1 |
| A5 | IO4 | 4 | ADC1 |

Todas as entradas "analógicas" leem de **0 a 3,3V** (não 0 a 5V como no
UNO), com resolução de 12 bits (`analogRead` devolve de 0 a 4095).

### Furos extras (sem barra soldada)

| Local na placa | Serigrafia | GPIO | Observação |
|---|---|---|---|
| Ao lado da barra digital | IO35, IO36, IO37 | 35, 36, 37 | ⛔ **Não use na versão N16R8** (ver abaixo) |
| Ao lado da barra digital | IO38, IO39, IO40, IO41, IO42 | 38–42 | Livres (IO39–IO42 também são JTAG) |
| Ao lado da barra de alimentação | IO45 | 45 | ⚠️ pino de *strapping* |
| Ao lado da barra de alimentação | IO15, IO16, IO47 | 15, 16, 47 | Livres |
| Ao lado da barra de alimentação | IO48 | 48 | Também ligado ao LED RGB [WS2812](../../GLOSSARIO.md#ws2812) da placa |

### Pinos que exigem cuidado

- **IO35, IO36 e IO37 (versão N16R8):** nos módulos com PSRAM de 8 MB
  (octal), esses três pinos são usados internamente pela PSRAM e **não
  podem** ser usados no projeto. Usar esses pinos trava ou reinicia a
  placa. Na versão N8R2 eles ficam livres.
- **Pinos de *strapping* (IO0, IO3, IO45, IO46):** o ESP32-S3 lê o nível
  desses pinos no momento do reset para decidir como iniciar. Um módulo
  que force um nível nesses pinos durante o reset pode impedir a placa de
  iniciar ou de gravar. Prefira usá-los como saída, ou deixe-os por último.
- **IO19 e IO20:** são o USB nativo do ESP32-S3. Nesta placa o USB-C vai
  para o CH340, então eles ficam livres como GPIO comuns.
- **IO43 e IO44 (RXD/TXD):** são a serial do Monitor Serial e da gravação.
  Ligar algo neles atrapalha o upload.

## Componentes principais
- Módulo **ESP32-S3-WROOM-1-N16R8** (ou N8R2): ESP32-S3 dual-core [Xtensa](../../GLOSSARIO.md#risc-v-e-xtensa)
  LX7 a 240 MHz, 512 KB de SRAM, Wi-Fi 802.11 b/g/n (2,4 GHz) e
  Bluetooth 5 LE, antena na placa (PCB)
- 16 MB de flash e 8 MB de PSRAM octal (N16R8) — ou 8 MB e 2 MB (N8R2)
- **CH340C**: conversor USB-serial, ligado à UART0 (GPIO43/44)
- Conector **USB-C** (gravação, Monitor Serial e alimentação)
- Conector **DC** (jack P4) de 5 a 18V
- Conversor DC-DC (chaveado) para 5V e regulador **AMS1117** para 3,3V
- **LED RGB endereçável WS2812** no GPIO48
- LED indicador de alimentação (POWER)
- **Um botão: RST** (reset). Não há botão BOOT
- Dimensões: 68,5 × 53,5 mm; peso: 22,8 g (dados do fabricante)

## Funcionalidades / Periféricos
- Wi-Fi 2,4 GHz e Bluetooth 5 LE (com Bluetooth Mesh)
- GPIOs com [PWM](../../GLOSSARIO.md#pwm) (LEDC), [ADC](../../GLOSSARIO.md#adc) de 12 bits, I2C, SPI, [UART](../../GLOSSARIO.md#uart), I2S, CAN (TWAI)
  e sensores de toque capacitivos
- PSRAM para buffers grandes: imagem, áudio e aprendizado de máquina
  (ESP-DL, ESP-SR)
- LED RGB endereçável na própria placa, útil como indicador de estado
- Programável em Arduino (C/C++) e em [MicroPython](../../GLOSSARIO.md#micropython) (ver **Como programar**);
  também aceita ESP-IDF

## Como programar

Esta placa é usada em aula de duas formas:

- **Arduino (C/C++)**, a linguagem de programação do Arduino, pela
  Arduino IDE ou pela linha de comando com o `arduino-cli`;
- **MicroPython**, pelo editor **Thonny**.

Nas duas, o computador fala com a placa pelo **CH340** no USB-C. Se o
computador não reconhecer a placa (nenhuma porta COM nova aparece ao
ligar o cabo), instale o driver do CH340 (link em **Referências**) e
confira se o cabo USB transmite dados, e não só carga.

**Se a gravação falhar** ("Failed to connect" / "Wrong boot mode"): a
placa não tem botão BOOT, e a entrada em modo de gravação depende do
CH340. Para forçar, ligue o pino **IO0** ao **GND** com um jumper, aperte
e solte o **RST**, grave, retire o jumper e aperte RST de novo. Vale para
o Arduino e para a gravação do firmware do MicroPython.

### Antes de programar: identificar o módulo (ESPConnect)

O [ESPConnect](https://thelastoutpostworkshop.github.io/ESPConnect/)
lê as informações do módulo ESP direto pelo navegador, sem instalar nada
e sem gravar nenhum programa. É o jeito mais rápido de saber qual variante
(N16R8 ou N8R2) você tem em mãos.

1. Abra o ESPConnect num navegador baseado no Chromium (Chrome, Edge,
   Brave), versão 89 ou mais nova. Firefox e Safari não funcionam, porque
   não têm Web Serial.
2. Feche a Arduino IDE, o Thonny ou qualquer Monitor Serial: só um
   programa por vez pode usar a porta COM.
3. Clique em **Connect** e escolha a porta do CH340.
4. Na aba **Device Info**, anote: família do chip (ESP32-S3), revisão,
   endereço [MAC](../../GLOSSARIO.md#mac), tamanho da flash, frequência do cristal e os recursos
   (*features*) do chip, onde aparece a PSRAM embutida, quando houver.
5. Clique em **Disconnect** para liberar a porta.

> ⚠️ O ESPConnect também grava firmware e **apaga a flash** (Flash,
> Erase, Format). Para só identificar o módulo, use apenas a aba
> **Device Info**. Apagar a flash remove o programa ou o MicroPython
> gravado na placa.

### Qual configuração usar depende do módulo

A PSRAM (memória RAM extra) fica dentro do módulo ESP32-S3, e **nem todo
módulo tem PSRAM, nem toda PSRAM é igual**. O firmware do MicroPython e a
opção PSRAM da Arduino IDE precisam combinar com o módulo que está na sua
placa. Por isso, **identifique o módulo antes** (ESPConnect ou gravação na
tampa metálica).

O nome do módulo diz tudo: `N` = flash em MB, `R` = PSRAM em MB.

| Módulo (gravação) | PSRAM | Firmware MicroPython (`ESP32_GENERIC_S3`) | Arduino IDE: PSRAM | Arduino IDE: Flash Size |
|---|---|---|---|---|
| N16R8, N8R8, N4R8 | 8 MB **octal** (OPI) | variante **`SPIRAM_OCT`** | OPI PSRAM (`PSRAM=opi`) | 16MB / 8MB / 4MB |
| N16R2, N8R2, N4R2 | 2 MB **quad** (QSPI) | variante padrão | QSPI PSRAM (`PSRAM=enabled`) | 16MB / 8MB / 4MB |
| N16, N8, N4 | **sem PSRAM** | variante padrão | Disabled (`PSRAM=disabled`) | 16MB / 8MB / 4MB |

Usar a configuração de PSRAM octal num módulo sem PSRAM octal faz o
programa travar ou reiniciar no boot. O contrário (configuração padrão
num módulo octal) funciona, mas deixa a PSRAM desligada.

No ESPConnect, a PSRAM aparece nos recursos (*features*) do chip: 8 MB
indica octal, 2 MB indica quad, e nenhuma menção indica módulo sem PSRAM.
O texto exato que o ESPConnect mostra ainda está **a confirmar** nesta
placa.

### Arduino (C/C++): Arduino IDE

Instale o pacote **esp32** (Espressif Systems) no Gerenciador de Placas e
use no menu **Ferramentas**:

| Opção | Valor |
|---|---|
| Placa | **ESP32S3 Dev Module** |
| Flash Size | conforme o módulo (tabela acima); 16MB (128Mb) na N16R8 |
| PSRAM | conforme o módulo (tabela acima); **OPI PSRAM** na N16R8 |
| USB CDC On Boot | Disabled (o USB-C passa pelo CH340) |
| Upload Mode | UART0 / Hardware CDC |
| Porta | a porta COM do CH340 |

> O anúncio do vendedor manda escolher "ESP32 Dev Module". Isso está
> **errado** para esta placa: "ESP32 Dev Module" é o ESP32 clássico, e o
> programa não grava ou não roda no S3.

### Arduino (C/C++): arduino-cli

As mesmas opções da IDE vão no **[FQBN](../../GLOSSARIO.md#fqbn)** (o nome completo da placa), depois
de `esp32:esp32:esp32s3:`.

```sh
# 1. Instalar o pacote esp32 (uma vez só)
arduino-cli config add board_manager.additional_urls https://espressif.github.io/arduino-esp32/package_esp32_index.json
arduino-cli core update-index
arduino-cli core install esp32:esp32

# 2. Descobrir a porta da placa (procure a linha do CH340, ex: COM5)
arduino-cli board list

# 3. Compilar, gravar e abrir o Monitor Serial (troque COM5 pela sua porta)
arduino-cli compile --fqbn esp32:esp32:esp32s3:FlashSize=16M,PSRAM=opi boards/esp32-s3-uno/code/teste_esp32_s3_uno
arduino-cli upload  --fqbn esp32:esp32:esp32s3:FlashSize=16M,PSRAM=opi -p COM5 boards/esp32-s3-uno/code/teste_esp32_s3_uno
arduino-cli monitor -p COM5 -c baudrate=115200
```

Para outro módulo, troque `FlashSize` e `PSRAM` conforme a tabela acima
(ex: N8R2 → `FlashSize=8M,PSRAM=enabled`).

### MicroPython: Thonny

**Qual firmware gravar:** o firmware oficial `ESP32_GENERIC_S3`
(https://micropython.org/download/ESP32_GENERIC_S3/), release estável
mais recente, na variante que combina com a PSRAM do módulo (tabela
**Qual configuração usar depende do módulo**, acima). Para a **N16R8**
(PSRAM octal) é a variante **`SPIRAM_OCT`**: em 2026-09 era a v1.29.0,
arquivo `ESP32_GENERIC_S3-SPIRAM_OCT-20260824-v1.29.0.bin`.

A variante **padrão** (sem `SPIRAM_OCT`) grava e roda, mas **não liga a
PSRAM de 8 MB** da N16R8, porque ela procura uma PSRAM *quad* e a da placa
é *octal*. Sinais de variante errada, observados nesta placa em aula
(2026-09-24):

```
E (307) quad_psram: PSRAM chip is not connected, or wrong PSRAM line mode
...
Failed to init external RAM; continuing without it
```

e `gc.mem_free()` perto de 200 KB (foi 218 704) em vez de vários MB. Em
módulos com PSRAM quad (R2) ou sem PSRAM, a variante padrão é a certa.

**Passo a passo:**

1. Instale o **Thonny** (https://thonny.org) e ligue a placa no USB.
2. Descubra a porta COM do CH340 no **Gerenciador de Dispositivos >
   Portas (COM e LPT)**: é a que aparece como "USB-SERIAL CH340" e some
   ao desligar a placa. Cuidado com portas "Serial padrão por link
   Bluetooth" (ex: de um fone pareado): elas aparecem na lista do Thonny
   e fazem a gravação falhar por tempo esgotado.
3. Identifique o módulo (ESPConnect) e baixe o `.bin` da variante
   certa no site do MicroPython (`SPIRAM_OCT` para a N16R8).
4. Grave o firmware por um dos dois métodos:
   - **Pelo Thonny:** em **Ferramentas > Opções > Interpretador**,
     escolha **MicroPython (ESP32)** e a porta do CH340, e clique em
     **Instalar ou atualizar MicroPython (esptool)**. A lista de
     variantes da família ESP32-S3 **não tem** a opção octal genérica (só
     "Espressif • ESP32-S3", que é a padrão, e placas de outros
     fabricantes). Use o menu **≡** da janela do instalador para escolher
     o arquivo `.bin` baixado. Nesse menu também dá para subir a
     velocidade para 460 800 [baud](../../GLOSSARIO.md#baud): o padrão de 115 200 leva ~105 s.
   - **Pela linha de comando (`esptool`):**

     ```sh
     esptool --chip esp32s3 --port COM6 erase_flash
     esptool --chip esp32s3 --port COM6 --baud 460800 write_flash 0 ESP32_GENERIC_S3-SPIRAM_OCT-20260824-v1.29.0.bin
     ```

     Troque `COM6` pela sua porta. Se falhar no meio, rode de novo sem o
     `--baud 460800`. No `esptool` 5 os comandos também se escrevem com
     hífen (`erase-flash`, `write-flash`); a forma com sublinhado continua
     aceita. Testado em 2026-09-29 com o `esptool` 5.2.0 do Thonny: apagar
     levou 3 s e gravar a 460 800 baud levou 28 s. O `esptool` que vem com o Thonny pode ser chamado
     com `python -m esptool`, usando o `python.exe` da pasta do Thonny.

   Antes de gravar, feche o Shell do Thonny (**Executar > Desconectar**)
   e desconecte o ESPConnect no navegador: os dois prendem a porta COM.
   Se a gravação não começar, use o jumper IO0–GND descrito acima.
5. Depois de gravar, o **Shell** do Thonny mostra o prompt `>>>`. Confira:

   ```python
   import os, esp, gc
   os.uname()          # versão do MicroPython
   esp.flash_size()    # 16777216 (16 MB) na N16R8
   gc.mem_free()       # vários milhões com a PSRAM ativa
   ```

6. Para usar os pinos, use o **número do GPIO**:
   `machine.Pin(18, machine.Pin.OUT)`.

## Código de teste e validação

| Formato | Arquivo | Como rodar |
|---|---|---|
| Arduino (C/C++) | [`code/teste_esp32_s3_uno/teste_esp32_s3_uno.ino`](code/teste_esp32_s3_uno/teste_esp32_s3_uno.ino) | Arduino IDE ou `arduino-cli` (ver acima) |
| MicroPython | [`code/teste_esp32_s3_uno_micropython/main.py`](code/teste_esp32_s3_uno_micropython/main.py) | Abrir no Thonny e executar (F5) |

Os dois testes fazem a mesma coisa: mostram os dados da placa (flash,
PSRAM ou RAM livre, frequência), o que confirma a variante N16R8 ou N8R2
e a configuração, e fazem o LED RGB WS2812 (GPIO48) trocar de cor. Nenhum
precisa de biblioteca extra.

O sketch Arduino foi compilado com o pacote esp32 3.3.11. O script
MicroPython teve só a sintaxe verificada, e ainda não rodou numa placa.

### A confirmar na placa real

| Item | Indício (fabricante / serigrafia) | Resultado do teste |
|---|---|---|
| Variante do módulo (ESPConnect, aba Device Info) | N16R8 no anúncio; algumas fotos mostram N8R2 | **N16R8 confirmada** (2026-09-29, placa `1e:20`): `esptool flash-id` mostra `Embedded PSRAM 8MB (AP_3v3)` e flash de 16 MB; eFuse `PSRAM_CAP` = 8M. Flash de 16 MB também na 1ª placa (2026-09-24) |
| LED RGB no GPIO48 | Serigrafia "SW2812 (IO48)" | — |
| Ordem das cores do LED RGB | Padrão WS2812 (GRB), tratado pelo `rgbLedWrite` e pelo `neopixel` | — |
| PSRAM de 8 MB ativa no MicroPython | Firmware `ESP32_GENERIC_S3` variante `SPIRAM_OCT` | Variante padrão: **sem PSRAM** (`quad_psram` no boot, `gc.mem_free()` = 218 704). **`SPIRAM_OCT`: PSRAM ativa** (2026-09-29, v1.29.0): boot sem `quad_psram`, `gc.mem_free()` = 8 319 216, `bytearray` de 4 MB alocado |
| Script MicroPython roda na placa | — | — |
| Gravação automática (sem jumper no IO0) | CH340 com circuito de reset automático | **Funciona** (2026-09-29): `esptool` apagou e gravou sem jumper e sem apertar botões |
| Faixa do conector DC | 5 a 18V (anúncio) | não testar acima de 12V sem medir |
| Corrente disponível nos pinos 5V e 3V3 | não informada | medir |

## Referências
- Datasheet do ESP32-S3 (Espressif):
  https://www.espressif.com/sites/default/files/documentation/esp32-s3_datasheet_en.pdf
- Datasheet do módulo ESP32-S3-WROOM-1 (Espressif), inclusive a restrição
  dos pinos IO35–IO37 nas versões com PSRAM octal:
  https://www.espressif.com/sites/default/files/documentation/esp32-s3-wroom-1_wroom-1u_datasheet_en.pdf
- Datasheet do CH340 (WCH): https://www.wch-ic.com/downloads/CH340DS1_PDF.html
- Driver do CH340 (WCH): https://www.wch-ic.com/downloads/CH341SER_EXE.html
- ESPConnect (identificação do módulo pelo navegador):
  https://thelastoutpostworkshop.github.io/ESPConnect/ —
  código-fonte: https://github.com/thelastoutpostworkshop/ESPConnect
- Anúncio do vendedor (TZT), com o diagrama de pinagem e as medidas:
  https://pt.aliexpress.com/item/1005007217207543.html
- Página do produto (Makers Electronics):
  https://makerselectronics.com/product/esp32-s3-n16r8-development-board-2/
- Firmware MicroPython para ESP32-S3 (oficial):
  https://micropython.org/download/ESP32_GENERIC_S3/
- Artigo introdutório (Blog Saravati):
  https://blog.saravati.com.br/esp32-s3-uno-evolucao-arduino-ia/
- Página na Cirkit Designer (genérica; a tabela de pinos de lá **não**
  corresponde a esta placa):
  https://docs.cirkitdesigner.com/component/fa2bcd40-a306-42c5-87b3-b984d2ca532d/esp32-s3-uno

## Para o professor / histórico de testes

Esta parte é do professor: como identificar cada unidade e registrá-la no
inventário do laboratório. Para usar a placa em aula, as seções acima
bastam.

### Identificar cada placa (MAC e inventário)

Placas do mesmo modelo são iguais por fora, mas cada chip ESP32-S3
sai de fábrica com identificadores próprios, gravados em **eFuse** (uma
memória que só pode ser escrita uma vez). Eles **não mudam** ao apagar a
flash ou regravar o firmware, por isso servem para saber qual placa é qual.

| Identificador | Como ler | Observação |
|---|---|---|
| **Endereço MAC** (6 bytes) | ESPConnect, aba **Device Info**; ou no MicroPython (abaixo); ou `esptool --port COM6 read-mac` | Único por chip. É a chave do inventário. |
| **ID único de 128 bits** | `espefuse --chip esp32s3 --port COM6 summary`, campo `OPTIONAL_UNIQUE_ID` | Registro complementar. É opcional e pode vir zerado em outros lotes. |

No Shell do Thonny, com o MicroPython gravado:

```python
import machine, binascii
binascii.hexlify(machine.unique_id(), ':')   # ex: b'e0:72:a1:d4:1e:20'
```

No ESP32-S3, `machine.unique_id()` devolve o próprio MAC. O `esptool` e o
`espefuse` que vêm com o Thonny podem ser chamados com
`python -m esptool` e `python -m espefuse`, usando o `python.exe` da pasta do
Thonny. Os dois só **leem** as informações e não apagam nada, mas precisam
da porta livre (Thonny desconectado, ESPConnect fechado).

**O que não serve para identificar a placa:** o CH340 não tem número de
série, e o código que o Windows mostra para ele (`USB\VID_1A86&PID_7523\...`)
muda conforme a porta USB do computador. O número da porta COM também muda.

**Inventário:** as placas registradas ficam no inventário do laboratório,
separado do código: [`inventario/esp32-s3-uno.csv`](../../inventario/esp32-s3-uno.csv),
uma linha por placa física. A **etiqueta** colada na placa são os dois
últimos bytes do MAC (ex: `1e:20`). Colunas próprias deste arquivo:

| Coluna | Conteúdo |
|---|---|
| `etiqueta`, `etiqueta_colada`, `dono` | Etiqueta (dois últimos bytes do MAC), se já está colada e de quem é a placa (ver [`inventario/README.md`](../../inventario/README.md)) |
| `mac` | MAC completo |
| `unique_id_128` | ID único de 128 bits (hexadecimal, sem espaços) |
| `chip_rev` | Revisão do chip (`esptool flash-id`) |
| `flash_mb`, `psram_mb`, `psram_modo` | Tamanho da flash, da PSRAM e modo da PSRAM (`octal` / `quad` / `nenhuma`) |
| `firmware` | Último firmware conhecido na placa, quando conferido |
| `registrado_em` | Data do registro (AAAA-MM-DD) |
| `obs` | Observações (defeitos, testes feitos) |

O inventário não guarda dados pessoais: quem está com cada placa não entra
neste repositório público.

---

**Autor:** Prof. Me. Joao Miguel Lac Roehe ([@professorjoaomiguel](https://github.com/professorjoaomiguel)). Documentação sob licença [CC BY-NC 4.0](https://creativecommons.org/licenses/by-nc/4.0/deed.pt-br); código em `code/` sob licença MIT. Veja como citar no [README principal](../../README.md).
