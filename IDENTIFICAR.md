[← Voltar ao README](README.md)

# Que placa é essa?

Guia para reconhecer as placas e shields do laboratório **pelo que dá para
ver**, sem precisar ligar nada. Siga as perguntas em ordem. Na dúvida, a
gravação nos chips e módulos é o critério mais seguro. Se a placa tem
etiqueta (`R3-01`, `R4M-01`, `S9-01`...), ela já foi identificada no
[inventário](inventario/README.md).

> ⚠️ Descobriu a placa? Antes de ligar qualquer coisa nela, confira a
> **tensão** (5V ou 3,3V) na tabela do fim e no README dela.

## 1. Qual é o formato?

| O que você vê | Vá para |
|---------------|---------|
| Placa do tamanho de um cartão (≈69 × 53 mm), com barras de **pinos fêmea** nas bordas, onde se encaixam fios ou shields | [2. Formato UNO](#2-formato-uno) |
| Placa comprida e estreita, com **pinos macho** embaixo, feita para encaixar na protoboard | [3. Placas de protoboard](#3-placas-de-protoboard) |
| Placa **minúscula** (≈22 × 18 mm) | [3. Placas de protoboard](#3-placas-de-protoboard) |
| Placa com **pinos macho embaixo**, feita para encaixar **por cima** de outra, cheia de componentes (sensores, botões, LEDs) | [4. Shields](#4-shields) |

## 2. Formato UNO

| O que você vê | Placa |
|---------------|-------|
| Conector USB **tipo B** (grande e quadrado, o da impressora) | [Arduino UNO R3](boards/arduino-uno-r3/README.md) |
| USB-C e uma **matriz de LEDs** (12 × 8 LEDs pequenos) no meio da placa | [Arduino UNO R4 WiFi](boards/arduino-uno-r4/README.md) |
| USB-C, **sem** matriz de LEDs e **sem** módulo metálico grande | [Arduino UNO R4 Minima](boards/arduino-uno-r4/README.md) |
| USB-C e um **módulo metálico grande** com a gravação `ESP32-S3` na tampa; só um botão (RST) | [ESP32-S3 UNO](boards/esp32-s3-uno/README.md) — **3,3V!** |

**UNO R3 original ou clone?** Olhe o chip ao lado do conector USB. Um chip
quadrado pequeno, com um conector de 6 pinos (ICSP) perto dele, é o
ATmega16U2 (original e clones melhores). Um chip retangular com a gravação
`CH340` é um clone. Os dois funcionam igual; muda só o driver. Ver
[Ponte USB-serial](boards/arduino-uno-r3/README.md#ponte-usb-serial).

**ESP32-S3 UNO N16R8 ou N8R2?** Leia a gravação na tampa do módulo:
`ESP32-S3-N16R8` ou `ESP32-S3-N8R2`. A configuração para programar é
diferente (ver o [README da placa](boards/esp32-s3-uno/README.md#qual-configuração-usar-depende-do-módulo)).

## 3. Placas de protoboard

| O que você vê | Placa |
|---------------|-------|
| Placa estreita com **módulo metálico** ESP32-S3, conector USB-C e dois botões (BOOT e RESET) | [ESP32-S3 N16R8 DevKit](boards/esp32-s3-n16r8/README.md) — **3,3V** |
| Placa minúscula, **sem** módulo metálico, com uma antena cerâmica **vermelha** marcada "C3", USB-C e dois botões (BOOT e RST) | [ESP32-C3 SuperMini](boards/esp32-c3-supermini/README.md) — **3,3V** |

A ESP32-C3 SuperMini se parece com a Seeed Studio XIAO ESP32C3, mas **a
pinagem é diferente**: não use tutoriais do XIAO sem conferir.

## 4. Shields

| O que você vê | Shield |
|---------------|--------|
| Formato UNO, com 2 botões, 2 LEDs pequenos (vermelho e azul), LED RGB, buzzer, sensor de umidade DHT11, receptor infravermelho, potenciômetro, LDR e sensor LM35 | [Shield Multifunção 9 em 1](shields/uno-shield-9in1/README.md) — **5V**: só no UNO R3/R4 |

## Tensão de cada placa

| Placa | Tensão lógica |
|-------|---------------|
| Arduino UNO R3 | 5V |
| Arduino UNO R4 (Minima / WiFi) | 5V |
| ESP32-S3 UNO | **3,3V** (formato de UNO, mas não é 5V) |
| ESP32-S3 N16R8 DevKit | **3,3V** |
| ESP32-C3 SuperMini | **3,3V** |
| Shield Multifunção 9 em 1 | 5V |

Não achou a placa aqui? Ela ainda não foi documentada. Anote o que está
escrito nos chips e no verso e fale com o professor.
