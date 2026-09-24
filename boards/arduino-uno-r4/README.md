---
titulo: "Arduino UNO R4 (Minima / WiFi)"
tipo: placa
tags: [arduino, uno, uno-r4, renesas, ra4m1, usb-c, wifi, bluetooth, 5v]
---

# Arduino UNO R4 (Minima / WiFi)

## Visão geral
Sucessora do UNO R3, no mesmo formato e com a mesma pinagem, mas com um
microcontrolador de 32 bits: o Renesas RA4M1 (Arm Cortex-M4 a 48 MHz). Tem
8 vezes mais flash e 16 vezes mais SRAM que o R3, e mantém a lógica de 5V,
então os shields feitos para o UNO R3 continuam funcionando.

Existem duas versões:

| | UNO R4 Minima | UNO R4 WiFi |
|---|---|---|
| Microcontrolador principal | RA4M1 | RA4M1 |
| Wi-Fi / Bluetooth | — | Sim, via módulo ESP32-S3-MINI-1 |
| Matriz de LEDs 12×8 | — | Sim |
| Conector Qwiic (I2C) | — | Sim |
| Conector SWD/JTAG | Sim | — |

## Tensão de operação
| | |
|---|---|
| Tensão lógica dos pinos | **5V** |
| Alimentação | USB-C 5V, ou Vin/conector P4 de 6–24V |
| Tolera 5V nas entradas? | Sim (a lógica já é 5V) |
| Corrente máxima por pino | **8 mA** |

> ⚠️ **Atenção à corrente: 8 mA por pino, contra 20 mA do UNO R3.** Um
> circuito que funcionava no R3 (ex: LED com resistor baixo, relé ou motor
> ligado direto no pino) pode danificar o RA4M1. Para cargas maiores, use
> um transistor ou um driver.
>
> **UNO R4 WiFi:** o módulo ESP32-S3 interno trabalha em 3,3V, mas fica
> atrás de um tradutor de nível lógico (TXB0108). Os pinos do header
> continuam sendo 5V.

Compatibilidade: funciona com shields de 5V, como o
[Shield Multifunção 9 em 1](../../shields/uno-shield-9in1/README.md). Em
módulos de 3,3V, as saídas da placa (5V) podem queimar o módulo: use um
conversor de nível lógico ou um divisor resistivo.

## Fotos
_Foto ainda não adicionada — colocar o arquivo em `imagens/` e referenciar
aqui com `![foto](imagens/nome-do-arquivo.jpg)`._

## Diagrama esquemático / Pinout
Pinout e esquemático oficiais (Arduino):
- UNO R4 Minima: [pinout](https://docs.arduino.cc/resources/pinouts/ABX00080-full-pinout.pdf) ·
  [esquemático](https://docs.arduino.cc/resources/schematics/ABX00080-schematics.pdf)
- UNO R4 WiFi: [pinout](https://docs.arduino.cc/resources/pinouts/ABX00087-full-pinout.pdf) ·
  [esquemático](https://docs.arduino.cc/resources/schematics/ABX00087-schematics.pdf)

## Componentes principais
- Microcontrolador **Renesas RA4M1** (R7FA4M1AB3CFM, Arm Cortex-M4,
  48 MHz): 256 KB de flash, 32 KB de SRAM, 8 KB de EEPROM
- Apenas no WiFi: módulo **ESP32-S3-MINI-1-N8** (Wi-Fi e Bluetooth LE) e
  matriz de LEDs 12×8
- Conector USB-C e conector de alimentação P4 (barrel jack)
- LED embutido no D13 (`LED_BUILTIN`)
- Botão RESET

## Funcionalidades / Periféricos
- 14 pinos digitais (D0–D13), 6 com PWM (D3, D5, D6, D9, D10, D11)
- 6 entradas analógicas (A0–A5): ADC de 10 bits por padrão, até 14 bits
  com `analogReadResolution(14)`
- DAC de até 12 bits no A0 (saída analógica real, com `analogWrite(A0, …)`)
- UART, I2C, SPI e CAN (D4/D5, requer transceptor externo)
- USB nativa (HID: a placa pode agir como teclado ou mouse)
- Relógio de tempo real (RTC)

### Diferenças para o UNO R3 que afetam os exemplos
- `Serial` é a USB e `Serial1` são os pinos D0/D1 (no R3, os dois são a
  mesma porta).
- Alguns sketches de AVR que acessam registradores diretamente (`PORTB`,
  `DDRD`, etc.) não funcionam no RA4M1.

## Ponte USB-serial
Diferente do UNO R3, o UNO R4 **não tem um chip dedicado de ponte
USB-serial** (nem ATmega16U2, nem CH340). Como não há chip de ponte, não é
preciso instalar driver extra no Windows.

- **UNO R4 Minima:** o RA4M1 tem USB nativa. O conector USB-C vai direto ao
  microcontrolador, que grava o sketch, faz o `Serial` e pode agir como
  teclado ou mouse (HID). Se um sketch travar a USB e a placa sumir da
  porta COM, **aperte RESET duas vezes rápido** para entrar no bootloader.
- **UNO R4 WiFi:** por padrão, o USB-C vai ao **ESP32-S3**, que faz a ponte
  USB-serial até o RA4M1 (é ele quem grava o RA4M1). A comunicação pode ser
  desviada direto para a USB nativa do RA4M1, por software (pino P408/D40
  em nível alto) ou de forma permanente (soldando o jumper SJ1).

Em nenhuma das versões os pinos D0/D1 estão ligados à USB: eles formam o
`Serial1`, livre para módulos externos.

## Código de teste e validação
(preenchido futuramente — ver pasta `code/`)

## Referências
- Datasheet do Renesas RA4M1 (Renesas):
  https://www.renesas.com/en/document/dst/ra4m1-group-datasheet
- Datasheet do ESP32-S3-MINI-1 (Espressif, apenas UNO R4 WiFi):
  https://www.espressif.com/sites/default/files/documentation/esp32-s3-mini-1_mini-1u_datasheet_en.pdf
- Datasheet da placa UNO R4 Minima (Arduino):
  https://docs.arduino.cc/resources/datasheets/ABX00080-datasheet.pdf
- Datasheet da placa UNO R4 WiFi (Arduino):
  https://docs.arduino.cc/resources/datasheets/ABX00087-datasheet.pdf
- Página oficial do Arduino — UNO R4 Minima:
  https://docs.arduino.cc/hardware/uno-r4-minima/
- Página oficial do Arduino — UNO R4 WiFi:
  https://docs.arduino.cc/hardware/uno-r4-wifi/
