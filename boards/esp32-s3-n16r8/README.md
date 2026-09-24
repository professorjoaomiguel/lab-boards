---
titulo: "ESP32-S3 N16R8 DevKit"
tipo: placa
tags: [esp32, esp32-s3, wifi, bluetooth, devkit, usb-c, 3v3]
---

# ESP32-S3 N16R8 DevKit

## Visão geral
Placa de desenvolvimento baseada no módulo ESP32-S3 (dual-core Xtensa LX7),
com 16MB de memória flash e 8MB de PSRAM (daí o sufixo "N16R8"). Usada em
aula para projetos que exigem mais memória do que o ESP32 clássico, como
processamento de imagem, buffers de tela e projetos com Wi-Fi/Bluetooth
simultâneos.

## Tensão de operação
| | |
|---|---|
| Tensão lógica dos pinos | **3,3V** |
| Alimentação | USB-C 5V (regulador interno para 3,3V) |
| Tolera 5V nas entradas? | **Não**: máximo de 3,6V nos GPIOs |
| Corrente máxima por pino | 40 mA (máximo absoluto); prefira até 20 mA |

> ⚠️ **Não use shields de 5V diretamente nesta placa**, como o
> [Shield Multifunção 9 em 1](../../shields/uno-shield-9in1/README.md).
> Qualquer sinal de 5V em um GPIO pode queimar a porta ou o ESP32-S3. Para
> ligar módulos de 5V, use um conversor de nível lógico (level shifter) ou
> um divisor resistivo nas entradas.

Produto de referência: https://makerselectronics.com/product/esp32-s3-n16r8-development-board-2

## Fotos
_Foto ainda não adicionada — colocar o arquivo em `imagens/` e referenciar
aqui com `![foto](imagens/nome-do-arquivo.jpg)`._

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
- GPIOs disponíveis para I2C, SPI, UART, PWM e ADC
- Suporte nativo a USB (OTG) via ESP32-S3

## Código de teste e validação
(preenchido futuramente — ver pasta `code/`)

## Referências
- Datasheet do ESP32-S3 (Espressif):
  https://www.espressif.com/sites/default/files/documentation/esp32-s3_datasheet_en.pdf
- Datasheet do módulo ESP32-S3-WROOM-1 (Espressif):
  https://www.espressif.com/sites/default/files/documentation/esp32-s3-wroom-1_wroom-1u_datasheet_en.pdf
- Produto: https://makerselectronics.com/product/esp32-s3-n16r8-development-board-2
