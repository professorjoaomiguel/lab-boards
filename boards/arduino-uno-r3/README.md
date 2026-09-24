---
titulo: "Arduino UNO R3"
tipo: placa
tags: [arduino, uno, uno-r3, avr, atmega328p, usb-b, 5v]
---

# Arduino UNO R3

## Visão geral
A placa Arduino clássica, baseada no microcontrolador de 8 bits
ATmega328P a 16 MHz. É a referência do formato UNO: a maioria dos shields
do mercado foi projetada para ela. Usada em aula como primeira placa para
introduzir entradas/saídas digitais, leitura analógica, PWM e comunicação
serial.

## Tensão de operação
| | |
|---|---|
| Tensão lógica dos pinos | **5V** |
| Alimentação | USB-B 5V, ou Vin/conector P4 de 7–12V (recomendado) |
| Tolera 5V nas entradas? | Sim (a lógica já é 5V) |
| Corrente máxima por pino | 40 mA (máximo absoluto); prefira até 20 mA |

Compatibilidade: funciona com shields de 5V, como o
[Shield Multifunção 9 em 1](../../shields/uno-shield-9in1/README.md). Em
módulos de 3,3V, as saídas da placa (5V) podem queimar o módulo: use um
conversor de nível lógico ou um divisor resistivo.

## Fotos
_Foto ainda não adicionada — colocar o arquivo em `imagens/` e referenciar
aqui com `![foto](imagens/nome-do-arquivo.jpg)`._

## Diagrama esquemático / Pinout
Pinout e esquemático oficiais (Arduino):
- Pinout: https://docs.arduino.cc/resources/pinouts/A000066-full-pinout.pdf
- Esquemático: https://docs.arduino.cc/resources/schematics/A000066-schematics.pdf

## Componentes principais
- Microcontrolador **ATmega328P** (AVR 8 bits, 16 MHz): 32 KB de flash,
  2 KB de SRAM, 1 KB de EEPROM
- Microcontrolador **ATmega16U2**: ponte USB-serial (faz a comunicação da
  USB com o ATmega328P)
- Conector USB-B e conector de alimentação P4 (barrel jack)
- Regulador de 5V e de 3,3V
- LED embutido no D13 (`LED_BUILTIN`), LEDs de alimentação e TX/RX
- Botão RESET e conector ICSP

## Funcionalidades / Periféricos
- 14 pinos digitais (D0–D13), 6 com PWM (D3, D5, D6, D9, D10, D11)
- 6 entradas analógicas (A0–A5), ADC de 10 bits
- UART (D0/D1, compartilhada com a USB), I2C (A4/A5 e SDA/SCL), SPI
  (D10–D13 e ICSP)
- Interrupções externas em D2 e D3

## Código de teste e validação
(preenchido futuramente — ver pasta `code/`)

## Referências
- Datasheet do ATmega328P (Microchip):
  https://ww1.microchip.com/downloads/aemDocuments/documents/MCU08/ProductDocuments/DataSheets/ATmega48A-PA-88A-PA-168A-PA-328-P-DS-DS40002061B.pdf
- Datasheet do ATmega16U2 (Microchip):
  https://ww1.microchip.com/downloads/en/DeviceDoc/doc7799.pdf
- Datasheet da placa (Arduino):
  https://docs.arduino.cc/resources/datasheets/A000066-datasheet.pdf
- Página oficial do Arduino (especificações, tutoriais, downloads):
  https://docs.arduino.cc/hardware/uno-rev3/
