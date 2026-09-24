---
titulo: "Shield Multifunção 9 em 1 (UNO)"
tipo: shield
tags: [arduino, uno, dht11, lm35, ldr, infravermelho, buzzer, led-rgb, i2c]
---

# Shield Multifunção 9 em 1 (UNO)

## Visão geral
Shield de aprendizado no formato Arduino UNO que reúne 9 periféricos em uma
única placa: 2 botões, 2 LEDs, LED RGB, sensor DHT11, buzzer, receptor
infravermelho, potenciômetro, LDR e sensor de temperatura LM35. Encaixa
direto sobre a placa, sem protoboard e sem fios, o que o torna ideal para
aulas introdutórias de entradas/saídas digitais, leitura analógica, PWM e
sensores.

Também traz barras de pinos para expansão: I2C, serial TTL, digitais livres
(D7, D8) e um analógico livre (A3), além do botão RESET repetido.

## Fotos
![frente e verso](imagens/frente-verso.jpg)

## Diagrama esquemático / Pinout
![pinout anotado](imagens/pinout-anotado.jpg)

| Pino | Periférico | Serigrafia na placa |
|------|------------|---------------------|
| D2 | Botão SW1 | `SW1 D2` |
| D3 | Botão SW2 | `SW2 D3` |
| D4 | Sensor de temperatura e umidade DHT11 | `DHT11 D4` |
| D5 | Buzzer | `Buzzer D5` |
| D6 | Receptor infravermelho (IR) | `IR Receiver D6` |
| D9, D10, D11 | LED RGB (um pino por cor) | `RGB LED D9-D11` |
| D12 | LED 3 mm vermelho | `LED2 D12` |
| D13 | LED 3 mm azul | `LED1 D13` |
| A0 | Potenciômetro | `Rotation A0` |
| A1 | LDR (sensor de luminosidade) | `Light A1` |
| A2 | Sensor de temperatura LM35 | `LM35 A2` |

### Pinos livres / expansão

| Conector | Pinos |
|----------|-------|
| Digital | `D7 VCC GND` e `D8 VCC GND` |
| Analógico | `GND VCC A3` |
| I2C | `GND VCC SDA SCL` |
| Serial TTL | `TXD RXD VCC GND` |

> **Atenção:** D13 também é o LED embutido do UNO (`LED_BUILTIN`), então o
> LED azul acende junto com ele. D9–D11 são pinos PWM, o que permite variar
> as cores do LED RGB com `analogWrite()`. O serial TTL usa os mesmos pinos
> D0/D1 da USB: evite usá-lo enquanto o Monitor Serial estiver aberto.

## Componentes principais
- 2 botões táteis (SW1, SW2)
- 2 LEDs de 3 mm (vermelho e azul)
- 1 LED RGB SMD
- Sensor de temperatura e umidade DHT11
- Buzzer
- Receptor infravermelho (IR)
- Potenciômetro (trimpot)
- LDR (fotoresistor)
- Sensor de temperatura LM35
- Botão RESET

## Funcionalidades / Periféricos
- Entradas digitais: botões (D2, D3)
- Saídas digitais: LEDs (D12, D13) e buzzer (D5)
- PWM: LED RGB (D9, D10, D11)
- Entradas analógicas: potenciômetro (A0), LDR (A1), LM35 (A2)
- Sensor digital de um fio: DHT11 (D4)
- Recepção de controle remoto IR (D6)
- Expansão: I2C, serial TTL, D7, D8 e A3

## Código de teste e validação
(preenchido futuramente — ver pasta `code/`)

## Referências
- Pinagem levantada a partir da serigrafia da placa e das imagens em
  `imagens/`.
