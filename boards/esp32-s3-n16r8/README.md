---
titulo: "ESP32-S3 N16R8 DevKit"
tipo: placa
autor: "Prof. Me. Joao Miguel Lac Roehe (@professorjoaomiguel)"
tags: [esp32, esp32-s3, wifi, bluetooth, devkit, usb-c, 3v3]
---

# ESP32-S3 N16R8 DevKit

## Resumo rápido

| Característica | Valor |
|---|---|
| Tensão lógica | **3,3V** (não tolera 5V) |
| Placa na IDE | **a confirmar** (o produto de referência ainda não foi definido); para o módulo N16R8, as opções de flash e PSRAM estão em [Como programar](#como-programar) |
| Driver USB | **a confirmar** (depende do modelo da placa) |
| LED embutido | **a confirmar** |
| Botões | BOOT e RESET |
| Cuidado nº 1 | Não ligue sinais de 5V nos GPIOs |

## Visão geral
Placa de desenvolvimento baseada no módulo ESP32-S3 (dual-core [Xtensa](../../GLOSSARIO.md#risc-v-e-xtensa) LX7),
com 16MB de memória flash e 8MB de [PSRAM](../../GLOSSARIO.md#psram) (daí o sufixo "[N16R8](../../GLOSSARIO.md#n16r8)"). Usada em
aula para projetos que exigem mais memória do que o ESP32 clássico, como
processamento de imagem, buffers de tela e projetos com Wi-Fi/Bluetooth
simultâneos.

## Tensão de operação
| Característica | Valor |
|---|---|
| Tensão lógica dos pinos | **3,3V** |
| Alimentação | USB-C 5V (regulador interno para 3,3V) |
| Tolera 5V nas entradas? | **Não**: máximo de 3,6V nos GPIOs |
| Corrente máxima por pino | 40 mA (máximo absoluto); prefira até 20 mA |

> ⚠️ **Não use shields de 5V diretamente nesta placa**, como o
> [Shield Multifunção 9 em 1](../../shields/uno-shield-9in1/README.md).
> Qualquer sinal de 5V em um [GPIO](../../GLOSSARIO.md#gpio) pode queimar a porta ou o ESP32-S3. Para
> ligar módulos de 5V, use um conversor de nível lógico (level shifter) ou
> um divisor resistivo nas entradas.

Produto de referência: _a definir_ (modelo e fabricante exatos ainda não
registrados).

Não confundir com o [ESP32-S3 UNO](../esp32-s3-uno/README.md), que usa o
mesmo módulo N16R8 mas tem o formato do Arduino UNO.

## Fotos
_Ainda sem foto própria._ Previstas, na pasta `imagens/`:
`vista-de-cima.jpg` (a placa inteira, de cima), `modulo.jpg` (a gravação
na tampa do módulo) e `serigrafia.jpg` (os nomes dos pinos e o modelo da
placa, que ajudam a definir o produto de referência).

## Diagrama esquemático / Pinout
_Diagrama ainda não adicionado — colocar o arquivo em `imagens/` e
referenciar aqui com `![esquemático](imagens/nome-do-arquivo.png)`._

## Componentes principais
- Módulo ESP32-S3 (dual-core Xtensa LX7, Wi-Fi 802.11 b/g/n, Bluetooth 5 LE)
- 16MB de memória flash
- 8MB de PSRAM
- Conector USB-C (programação e alimentação)
- Botões BOOT e RESET

## Funcionalidades / Periféricos
- Wi-Fi e Bluetooth Low Energy integrados
- GPIOs disponíveis para [I2C](../../GLOSSARIO.md#i2c), [SPI](../../GLOSSARIO.md#spi), [UART](../../GLOSSARIO.md#uart), [PWM](../../GLOSSARIO.md#pwm) e [ADC](../../GLOSSARIO.md#adc)
- Suporte nativo a USB (OTG) via ESP32-S3

## Como programar

_A confirmar_ nesta placa: o produto de referência ainda não foi definido.
O módulo é o mesmo **N16R8** da ESP32-S3 UNO, então as opções de flash e
PSRAM da Arduino IDE e o firmware do MicroPython são os mesmos: ver
[Qual configuração usar depende do módulo](../esp32-s3-uno/README.md#qual-configuração-usar-depende-do-módulo).

## Código de teste e validação
(preenchido futuramente — ver pasta `code/`)

## Referências
- Datasheet do ESP32-S3 (Espressif):
  https://www.espressif.com/sites/default/files/documentation/esp32-s3_datasheet_en.pdf
- Datasheet do módulo ESP32-S3-WROOM-1 (Espressif):
  https://www.espressif.com/sites/default/files/documentation/esp32-s3-wroom-1_wroom-1u_datasheet_en.pdf

---

**Autor:** Prof. Me. Joao Miguel Lac Roehe ([@professorjoaomiguel](https://github.com/professorjoaomiguel)). Documentação sob licença [CC BY-NC 4.0](https://creativecommons.org/licenses/by-nc/4.0/deed.pt-br); código em `code/` sob licença MIT. Veja como citar no [README principal](../../README.md).
