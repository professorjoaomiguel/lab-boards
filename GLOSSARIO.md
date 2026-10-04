# Glossário

Termos técnicos usados neste repositório, em ordem alfabética, com uma
explicação curta. Aqui fica o **conceito geral**; o que vale para uma placa
específica (pinos, valores medidos) fica no README dela, indicado em
**Neste repositório**.

Para linkar um termo a partir de um README, use o título dele como âncora:
`[ADC](../../GLOSSARIO.md#adc)`. Ao usar um termo técnico novo num README,
acrescente-o aqui (regra em [`.ai/CONVENTIONS.md`](.ai/CONVENTIONS.md#glossário)).

[A](#a) · [B](#b) · [C](#c) · [D](#d) · [E](#e) · [F](#f) · [G](#g) ·
[H](#h) · [I](#i) · [J](#j) · [L](#l) · [M](#m) · [N](#n) · [O](#o) ·
[P](#p) · [R](#r) · [S](#s) · [T](#t) · [U](#u) · [V](#v) · [W](#w)

## A

### A confirmar

Marca usada nos READMEs para um dado que veio do fabricante ou de uma
dedução, mas **ainda não foi testado na placa real**. Quando o teste real
diverge do fabricante, vale o teste real.

### ADC

*Analog-to-Digital Converter*, conversor analógico-digital. Transforma uma
tensão num número que o programa lê com `analogRead()`. A **resolução** diz
quantos degraus existem: com 10 bits, de 0 a 1023; com 12 bits, de 0 a
4095. O valor máximo corresponde à tensão de referência (ver [AREF](#aref)).

**Neste repositório:** [ADC do UNO R3](boards/arduino-uno-r3/README.md#adc-cuidados-medidos-na-placa-real-2026-10-03)
e [ADC do UNO R4](boards/arduino-uno-r4/README.md#adc-leitura-errada-de-sensores-que-não-absorvem-corrente)
(leituras erradas medidas na placa real e como evitar).

### Ânodo comum

LED RGB em que os três LEDs internos compartilham o **positivo**. Cada cor
acende com o pino em **LOW**. O contrário é o [cátodo comum](#cátodo-comum).

### AREF

Pino que define a **tensão de referência** do [ADC](#adc), ou seja, a
tensão que corresponde à leitura máxima. Muda com `analogReference()`.

**Neste repositório:** no Shield 9 em 1 o AREF não está ligado a nada; na
[ESP32-S3 UNO](boards/esp32-s3-uno/README.md#tensão-de-operação), a posição
do AREF está ligada ao **reset** da placa.

### ATmega16U2

Microcontrolador pequeno que, no UNO R3 original, faz a
[ponte USB-serial](#ponte-usb-serial). Tem um **número de série USB**
gravado pela Arduino, diferente em cada placa, usado para identificar a
unidade no inventário.

**Neste repositório:** [Ponte USB-serial do UNO R3](boards/arduino-uno-r3/README.md#ponte-usb-serial).

## B

### Baud

Velocidade da comunicação serial ([UART](#uart)), em símbolos por segundo.
Os dois lados precisam usar o mesmo valor; se não, o
[Monitor Serial](#monitor-serial) mostra caracteres sem sentido.

**Neste repositório:** todos os sketches de teste usam **115200**.

### Bootloader

Programa pequeno, gravado no microcontrolador, que roda logo depois do
reset e recebe um sketch novo pela USB ou pela serial. É ele que torna
possível gravar sem programador externo.

**Neste repositório:** no UNO R4, apertar RESET duas vezes rápido força o
bootloader ([Ponte USB-serial do R4](boards/arduino-uno-r4/README.md#ponte-usb-serial)).

### Botão BOOT

Botão que, apertado durante o reset, coloca um ESP32 em **modo de
gravação** (ele segura um pino de [strapping](#strapping-pinos-de) em LOW).

**Neste repositório:** a [ESP32-C3 SuperMini](boards/esp32-c3-supermini/README.md#pinos-que-exigem-cuidado)
tem o botão (GPIO9). A ESP32-S3 UNO **não tem**: a gravação é automática
pelo [CH340](#ch340).

## C

### Cátodo comum

LED RGB em que os três LEDs internos compartilham o **negativo** (GND).
Cada cor acende com o pino em **HIGH**. O contrário é o
[ânodo comum](#ânodo-comum).

**Neste repositório:** o LED RGB do Shield 9 em 1 é de cátodo comum
(confirmado na placa).

### CDC (USB CDC)

Classe USB que faz um dispositivo aparecer no computador como **porta
serial** (porta COM), sem precisar de um chip de
[ponte USB-serial](#ponte-usb-serial). Placas com USB nativa (UNO R4
Minima, ESP32-C3) usam CDC.

**Neste repositório:** na ESP32-S3 UNO, a opção "USB CDC On Boot" fica
desligada, porque o USB-C passa pelo CH340
([configuração da IDE](boards/esp32-s3-uno/README.md#arduino-cc-arduino-ide)).

### CH340

Chip barato de [ponte USB-serial](#ponte-usb-serial), da fabricante WCH,
comum em clones do Arduino e em várias placas ESP32. Precisa de driver no
Windows (em geral instalado pelo Windows Update). **Não tem número de
série USB**, então placas com CH340 são registradas à mão no inventário.

**Neste repositório:** [Ponte USB-serial do UNO R3](boards/arduino-uno-r3/README.md#ponte-usb-serial).

## D

### DAC

*Digital-to-Analog Converter*, conversor digital-analógico. O contrário do
[ADC](#adc): gera uma **tensão de verdade** a partir de um número. Não
confundir com o [PWM](#pwm), que só liga e desliga rápido.

**Neste repositório:** o UNO R4 tem um DAC de até 12 bits no A0 (o teste
`dac` do [teste da placa](boards/arduino-uno-r4/README.md#teste-da-placa-automático)).

### Debounce

Tratamento do **repique** de um botão: ao apertar, o contato mecânico
abre e fecha várias vezes em poucos milissegundos. O código espera o sinal
firmar antes de aceitar o toque, para não contar vários apertos.

### DFU

*Device Firmware Upgrade*: modo USB padrão para gravar firmware num chip.

**Neste repositório:** o UNO R4 Minima grava por DFU; se aparecer "No DFU
capable USB device" ou "LIBUSB_ERROR", veja
[Como rodar](boards/arduino-uno-r4/README.md#como-rodar).

### DHT11

Sensor digital de **temperatura e umidade**, que conversa com a placa por
um único fio de dados.

**Neste repositório:** a primeira leitura depois de ligar vem zerada e os
sketches a descartam; no Shield 9 em 1 ele é a referência de temperatura
(ver [LM35](#lm35)).

### Divisor de tensão

Dois resistores em série (ou um [trimpot](#trimpot)) que entregam, no
ponto do meio, uma fração da tensão de entrada:
`Vsaída = Ventrada × R2 / (R1 + R2)`. É assim que um potenciômetro ou um
[LDR](#ldr) vira uma tensão que o [ADC](#adc) lê.

**Neste repositório:** [Medições do circuito](shields/uno-shield-9in1/README.md#medições-do-circuito-2026-10-01)
do Shield 9 em 1.

## E

### Etiqueta (inventário)

Código colado em cada unidade física do laboratório (`R3-01`, `R4M-01`,
`S9-01`...), sequencial por tipo de placa. Liga a peça na mão à linha dela
no inventário.

**Neste repositório:** [inventário](inventario/README.md#colunas-comuns).

## F

### Flash

Memória que **guarda o programa** e não se apaga ao desligar a placa. Nos
módulos ESP32 é indicada pela letra `N` (ver [N16R8](#n16r8)).

### FQBN

*Fully Qualified Board Name*: o nome completo da placa para o
`arduino-cli`, com o pacote, a placa e as opções. Exemplo:
`esp32:esp32:esp32s3:PSRAM=opi`.

**Neste repositório:** [arduino-cli na ESP32-S3 UNO](boards/esp32-s3-uno/README.md#arduino-cc-arduino-cli).

### Front matter

Bloco no topo de cada README, entre linhas `---`, com `titulo`, `tipo` e
`tags`. É lido pelo `scripts/gerar_indice.py` para montar o `INDEX.md`.

**Neste repositório:** [Adicionar um item novo](.ai/CONVENTIONS.md#adicionar-um-item-novo).

## G

### GPIO

*General Purpose Input/Output*: pino de uso geral, que o programa
configura como **entrada** (ler um botão) ou **saída** (acender um LED).

## H

### Header

Fileira de pinos (ou de conectores fêmea) na borda da placa. É no header
que o [shield](#shield) encaixa e por onde passam 5V, 3,3V, GND e os sinais.

## I

### I2C

Barramento de comunicação de **dois fios** (SDA para dados e SCL para o
relógio) em que vários módulos, cada um com seu endereço, dividem os mesmos
pinos. Os fios precisam de resistores de [pull-up](#pull-up-e-pull-down).

### ID único

Número gravado de fábrica no chip, diferente em cada unidade e impossível
de apagar. Serve de **chave** para identificar a placa no inventário.

**Neste repositório:** no UNO R4 é o ID de 128 bits do RA4M1; no R3
original, o número de série do [ATmega16U2](#atmega16u2); nas ESP32, o
[MAC](#mac). Ver [Colunas comuns do inventário](inventario/README.md#colunas-comuns).

### IOREF

Pino do header que **informa ao shield** a tensão lógica da placa (5V no
UNO, 3,3V em placas de 3,3V), para que um shield bem projetado se adapte.

**Neste repositório:** no Shield 9 em 1 o IOREF não está ligado a nada, por
isso ele não se adapta a placas de 3,3V
([Tensão de operação](shields/uno-shield-9in1/README.md#tensão-de-operação)).

## J

### Jumper

Fio curto (ou pequeno conector) que liga dois pinos. Usado em testes (D0↔D1
no teste `serial1` do UNO R4) e para forçar modos (IO0 → GND para gravar a
ESP32-S3 UNO se a gravação automática falhar).

## L

### LDR

*Light Dependent Resistor*, resistor que varia com a luz: quanto mais luz,
**menor** a resistência. Num [divisor de tensão](#divisor-de-tensão), vira
um sensor de luminosidade.

**Neste repositório:** no Shield 9 em 1, o LDR fica entre o 5V e o A1, então
a tensão no A1 **sobe** com a luz.

### LED_BUILTIN

Constante do Arduino com o número do pino do LED da própria placa (D13 no
UNO R3 e no R4). Usar `LED_BUILTIN` em vez do número deixa o sketch
portável entre placas.

### LM35

Sensor **analógico** de temperatura: a saída sobe **10 mV por °C** (0,25 V
a 25 °C). Precisa de pelo menos 4V de alimentação, então não funciona em
3,3V.

**Neste repositório:** instável nos shields testados; use o DHT11 como
referência ([LM35 instável](shields/uno-shield-9in1/README.md#lm35-instável-2026-10-03)).

## M

### MAC

Endereço físico de rede, com 6 bytes (`e0:72:a1:d4:1e:20`), gravado de
fábrica em cada chip com Wi-Fi.

**Neste repositório:** é a chave do inventário das ESP32-S3 UNO, e os dois
últimos bytes viram a etiqueta (`1e:20`). Ver
[Identificar cada placa](boards/esp32-s3-uno/README.md#identificar-cada-placa-mac-e-inventário).

### MicroPython

Versão do Python feita para microcontroladores. Grava-se o **firmware**
do MicroPython uma vez; depois os programas (`main.py`) são enviados por
uma ferramenta como o Thonny, sem compilar.

**Neste repositório:** [MicroPython na ESP32-S3 UNO](boards/esp32-s3-uno/README.md#micropython-thonny)
(a variante do firmware depende da [PSRAM](#psram)).

### Monitor Serial

Janela da IDE do Arduino que mostra o que a placa envia pela serial e
envia texto para ela. Tem dois ajustes que precisam combinar com o sketch:
a velocidade ([baud](#baud)) e o **final de linha** (nenhum, `\n`, `\r` ou
os dois).

**Neste repositório:** os sketches de teste aceitam qualquer final de
linha e só começam depois do comando `c`.

## N

### N16R8

Código do módulo ESP32-S3: `N` = [flash](#flash) em MB e `R` =
[PSRAM](#psram) em MB. N16R8 = 16 MB de flash e 8 MB de PSRAM (octal).

**Neste repositório:** [Qual configuração usar depende do módulo](boards/esp32-s3-uno/README.md#qual-configuração-usar-depende-do-módulo).

### Nível lógico

Tensão que representa **HIGH** numa placa: 5V no UNO R3/R4, 3,3V nas
ESP32. Ligar um sinal de 5V num pino de 3,3V pode queimar o pino. Por isso
todo item do repositório tem a seção **Tensão de operação** e a tag `5v`
ou `3v3`.

**Neste repositório:** [Tensão de operação](.ai/CONVENTIONS.md#tensão-de-operação-obrigatório).

### NPN e PNP

Os dois tipos de transistor bipolar usados como chave. O **NPN** liga a
carga quando o pino do Arduino vai a **HIGH**; o **PNP**, quando vai a
**LOW**.

**Neste repositório:** o buzzer do Shield 9 em 1 é acionado por um NPN e
liga em HIGH (a Keyestudio diz o contrário; vale o teste real).

## O

### OPI e QSPI

Formas de ligar a [PSRAM](#psram) ao chip. **QSPI** (*quad*) usa 4 fios de
dados; **OPI** (*octal*) usa 8, é mais rápida e ocupa mais pinos. A
configuração do firmware precisa combinar com o módulo.

**Neste repositório:** na ESP32-S3 UNO N16R8 a PSRAM é octal, e os pinos
IO35 a IO37 ficam reservados para ela
([Pinos que exigem cuidado](boards/esp32-s3-uno/README.md#pinos-que-exigem-cuidado)).

## P

### Ponte USB-serial

Chip que converte USB em [UART](#uart), para um microcontrolador sem USB
própria aparecer no computador como porta COM. É por ela que o sketch é
gravado e o Monitor Serial funciona. Exemplos: [ATmega16U2](#atmega16u2) e
[CH340](#ch340).

**Neste repositório:** [UNO R3](boards/arduino-uno-r3/README.md#ponte-usb-serial)
(tem ponte) e [UNO R4](boards/arduino-uno-r4/README.md#ponte-usb-serial)
(não tem).

### PSRAM

Memória RAM **extra**, fora do chip mas dentro do módulo ESP32, usada para
dados grandes (imagens, buffers). Nem todo módulo tem, e a configuração do
firmware precisa combinar com o tipo (ver [OPI e QSPI](#opi-e-qspi)).

**Neste repositório:** [Qual configuração usar depende do módulo](boards/esp32-s3-uno/README.md#qual-configuração-usar-depende-do-módulo).

### Pull-up e pull-down

Resistor que mantém um pino num nível definido quando nada o comanda. O
**pull-up** puxa para HIGH; o **pull-down**, para LOW. Com pull-up, um
botão que liga o pino ao GND é lido como **LOW quando apertado** (ativo em
LOW).

**Neste repositório:** os botões do Shield 9 em 1 têm pull-up de ≈10 kΩ
para o 5V ([Medições do circuito](shields/uno-shield-9in1/README.md#medições-do-circuito-2026-10-01)).

### PWM

*Pulse Width Modulation*: o pino liga e desliga muito rápido, e a fração
do tempo ligado (*duty cycle*) controla a média. Serve para variar o
brilho de um LED ou a velocidade de um motor com `analogWrite()`. Não é
uma tensão contínua (ver [DAC](#dac)).

## R

### RISC-V e Xtensa

Duas **arquiteturas** de processador, cada uma com seu compilador. O
ESP32-S3 usa Xtensa; o ESP32-C3 usa RISC-V.

**Neste repositório:** nos computadores do laboratório, o compilador
RISC-V está bloqueado pelo Windows, então o ESP32-C3 ainda não compila (ver
`.ai/STATE.md`).

## S

### Shield

Placa que **encaixa por cima** de outra, pelo [header](#header), e
acrescenta componentes (sensores, botões, LEDs) sem fios soltos. Um shield
precisa ter o mesmo [nível lógico](#nível-lógico) da placa.

**Neste repositório:** [shields documentados](shields/README.md).

### Sketch

Programa do Arduino: um arquivo `.ino` dentro de uma pasta com o mesmo
nome.

**Neste repositório:** os sketches de teste ficam em `code/` dentro da
pasta de cada item ([Código de teste](.ai/CONVENTIONS.md#código-de-teste-sketches)).

### Slug

Nome da pasta de um item: minúsculas, sem acento, palavras separadas por
hífen (`arduino-uno-r4`, `uno-shield-9in1`).

**Neste repositório:** [Slugs](.ai/CONVENTIONS.md#slugs).

### SPI

Barramento de comunicação rápido com fios separados para enviar e receber
(MOSI, MISO), um relógio (SCK) e um fio de seleção (CS) para cada módulo.

### Strapping (pinos de)

Pinos que o ESP32 lê **no momento do reset** para decidir como iniciar
(rodar o programa ou entrar em modo de gravação). Um módulo que force um
nível nesses pinos durante o reset pode impedir a placa de iniciar ou de
gravar.

**Neste repositório:** [ESP32-S3 UNO](boards/esp32-s3-uno/README.md#pinos-que-exigem-cuidado)
e [ESP32-C3 SuperMini](boards/esp32-c3-supermini/README.md#pinos-que-exigem-cuidado).

## T

### Trimpot

Potenciômetro pequeno, ajustado com chave de fenda. Funciona como
[divisor de tensão](#divisor-de-tensão).

**Neste repositório:** no Shield 9 em 1 fica no A0; num dos extremos, o A0
fica ligado **direto** ao 5V.

## U

### UART

Comunicação serial assíncrona com dois fios: TX (envia) e RX (recebe),
cruzados entre os dois lados. É a base do `Serial` do Arduino.

**Neste repositório:** no UNO R3, os pinos D0/D1 estão ligados à
[ponte USB-serial](#ponte-usb-serial); no R4 eles ficam livres como
`Serial1`.

## V

### VIN

Pino de **entrada de alimentação** externa, ligado ao mesmo circuito do
conector de fonte (P4/DC). A faixa aceita muda de placa para placa: veja a
seção **Tensão de operação** de cada README.

## W

### WS2812

LED RGB **endereçável**: tem um controlador interno e recebe a cor por um
único fio de dados. Vários podem ser ligados em fila no mesmo pino.

**Neste repositório:** a ESP32-S3 UNO tem um no GPIO48
([Componentes principais](boards/esp32-s3-uno/README.md#componentes-principais)).
