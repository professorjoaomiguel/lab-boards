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
  USB com o ATmega328P). Em clones, costuma ser trocado por um CH340 (ver
  [Ponte USB-serial](#ponte-usb-serial))
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

## Ponte USB-serial
O ATmega328P não tem USB: ele só fala serial (UART, pinos D0/D1). Entre o
conector USB e o ATmega328P existe um segundo chip, a **ponte USB-serial**,
que o computador enxerga como uma porta COM. É por ela que o sketch é
gravado e que o Monitor Serial funciona. A ponte também reinicia o
ATmega328P antes de cada gravação (sinal DTR), o que dispara o bootloader.

Qual chip faz a ponte depende de quem fabricou a placa:

| Chip da ponte | Onde aparece | Driver no Windows | Nome no Gerenciador de Dispositivos |
|---------------|--------------|-------------------|-------------------------------------|
| **ATmega16U2** | UNO R3 original (Arduino) e clones de melhor qualidade | Instalado com a IDE do Arduino | `Arduino Uno (COMx)` |
| **CH340** (CH340G/CH340C) | Maioria dos clones baratos | Em geral instalado pelo Windows Update; se não, [driver da WCH](https://www.wch-ic.com/downloads/CH341SER_EXE.html) | `USB-SERIAL CH340 (COMx)` |
| ATmega8U2 | UNO R1 e R2 (versões antigas) | Instalado com a IDE do Arduino | `Arduino Uno (COMx)` |

**Como identificar na placa:** o chip da ponte fica ao lado do conector
USB. O ATmega16U2 é um chip quadrado pequeno (QFN) com um segundo conector
ICSP de 6 pinos perto dele. O CH340 é um chip retangular com a marcação
`CH340`.

**O que muda na prática:**
- Para gravar e usar o Monitor Serial, as duas pontes funcionam igual: a
  diferença é só o driver e o nome da porta COM.
- Se a placa não aparece como porta COM, quase sempre é falta do driver do
  CH340 (ou um cabo USB que só carrega, sem dados).
- Só o ATmega16U2 é um microcontrolador reprogramável: com o firmware
  certo (modo DFU), ele pode fazer a placa aparecer como teclado, mouse ou
  dispositivo MIDI. Com o CH340, a placa só funciona como porta serial.
- Como D0/D1 estão ligados à ponte, qualquer circuito nesses pinos pode
  atrapalhar a gravação e o Monitor Serial.

## Código de teste e validação
(preenchido futuramente — ver pasta `code/`)

## Referências
- Datasheet do ATmega328P (Microchip):
  https://ww1.microchip.com/downloads/aemDocuments/documents/MCU08/ProductDocuments/DataSheets/ATmega48A-PA-88A-PA-168A-PA-328-P-DS-DS40002061B.pdf
- Datasheet do ATmega16U2 (Microchip):
  https://ww1.microchip.com/downloads/en/DeviceDoc/doc7799.pdf
- Datasheet do CH340 (WCH, ponte USB-serial dos clones):
  https://www.wch-ic.com/downloads/CH340DS1_PDF.html
- Datasheet da placa (Arduino):
  https://docs.arduino.cc/resources/datasheets/A000066-datasheet.pdf
- Página oficial do Arduino (especificações, tutoriais, downloads):
  https://docs.arduino.cc/hardware/uno-rev3/
