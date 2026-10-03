---
titulo: "Arduino UNO R3"
tipo: placa
autor: "Prof. Joao Miguel Roehe (@professorjoaomiguel)"
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

### ADC: cuidados medidos na placa real (2026-10-03)

Achados com o LM35 do
[Shield 9 em 1](../../shields/uno-shield-9in1/README.md), um sensor cuja
saída quase não consegue **absorver** corrente:

- **Depois de trocar de pino, espere antes de ler um sensor assim.** O ADC
  tem um capacitor interno que chega carregado com a tensão do pino lido
  antes. Logo depois de ler um pino com tensão maior (LDR, potenciômetro ou
  o canal interno de 1,1 V), o LM35 deu até 445 mV na 1ª leitura e ~295 mV
  nas seguintes, contra 239 mV reais. **100 ms depois da troca**, a
  leitura fica certa. Descartar só 1 leitura não bastou. No
  [UNO R4](../arduino-uno-r4/README.md#adc-leitura-errada-de-sensores-que-não-absorvem-corrente)
  o mesmo efeito não passa sozinho e precisa de um ajuste de registrador.

  ```cpp
  analogRead(A2);   // troca o canal do ADC para o A2
  delay(100);       // espera o LM35 acertar o capacitor do ADC
  int leitura = analogRead(A2);
  ```

- **Depois de `analogReference(INTERNAL)`, espere ~0,5 s.** O pino AREF
  tem um capacitor de 100 nF na placa, que leva centenas de milissegundos
  para descarregar de 5 V até 1,1 V. Medido: o LM35 (246 mV reais) leu 0 a
  22 mV nos primeiros 20 ms, 235 mV aos 100 ms e 256 mV aos 500 ms.
- **Medir o Vcc sem multímetro:** o ADC pode ler a referência interna de
  1,1 V usando o Vcc como régua: Vcc = 1,1 V × 1023 / leitura. Na USB, a
  placa testada mediu **4,87 a 4,89 V**. O valor de 1,1 V varia de 1,0 a
  1,2 V entre chips (datasheet), então essa medida tem até ~10% de erro.

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

Há dois tipos de teste, cada um com o seu sketch:

| Teste | Sketch | Como é | Para quê |
|-------|--------|--------|----------|
| **Só a placa** | [`code/teste_uno_r3_automatico`](code/teste_uno_r3_automatico/teste_uno_r3_automatico.ino) | Automático, só pela serial, **placa sozinha** (sem shield) | Triagem rápida e registro no inventário |
| **Placa + Shield 9 em 1** | [`teste_shield_9em1_uno_r3`](../../shields/uno-shield-9in1/code/teste_shield_9em1_uno_r3/teste_shield_9em1_uno_r3.ino) (na pasta do shield) | Menu com testes guiados (ver, ouvir, apertar) e uma opção automática | Testar o shield e a placa juntos; informa sobre os dois |

Os dois usam **115200 baud** no Monitor Serial e aceitam qualquer opção de
final de linha. Enquanto esperam um comando, o LED **L** (D13) pisca **duas
vezes rápidas a cada 2 s**: é o sinal de que o firmware de teste está
gravado.

> ⚠️ **O teste da placa é com a placa sozinha.** Ele liga **D2 a D13 e A1 a
> A5 como saída** (HIGH e LOW). Um módulo, protoboard ou shield ligado
> nesses pinos pode receber esses sinais e se danificar. Proteção: antes de
> acionar os pinos, o sketch procura resistores externos neles (como os
> pull-ups do Shield 9 em 1). Se achar, não aciona nenhum pino, marca
> `gpio` como pulado e diz em quais pinos achou algo. O teste só começa
> quando você envia `c`.

### Teste da placa (automático)

| Teste | O que confere |
|-------|---------------|
| `chip` | Assinatura do microcontrolador: `1E 95 0F` = ATmega328P (clones com ATmega328PB mostram `1E 95 16`) |
| `relogio` | `millis()` e `micros()` medem 1 s corretamente |
| `eeprom` | Grava, lê e **restaura** o último byte da EEPROM (1 KB) |
| `vcc` | Tensão de alimentação, medida pela própria placa (até ~10% de erro) |
| `gpio` | Pull-up interno e saída HIGH/LOW de D2–D12 e A1–A5 |

O UNO R3 não tem RTC, DAC, segunda serial nem ID único no chip, então esses
testes do UNO R4 não existem aqui.

**Como rodar pela IDE do Arduino:** placa **Arduino Uno**, grave o sketch,
abra o **Monitor Serial em 115200 baud** e **envie `c`**. No fim aparece o
**RESUMO**, um teste por linha. No UNO R3, abrir o Monitor Serial
**reinicia a placa** (é assim que o chip da USB funciona), por isso o aviso
inicial aparece de novo toda vez que o Monitor abre.

### Teste conjunto (placa + Shield 9 em 1)

Fica na pasta do shield. Ao abrir o Monitor Serial, mostra a placa
(chip e Vcc) e o menu: os testes **1 a 9** são guiados e a opção **`a`**
roda um teste automático do shield com resumo. Detalhes em
[Código de teste do shield](../../shields/uno-shield-9in1/code/README.md).

### Pelo terminal (ferramenta do professor)

Preparação do computador em
[Como rodar, no README do UNO R4](../arduino-uno-r4/README.md#como-rodar).

```bash
python scripts/serial_placa.py auto --porta COM10 --gravar --registrar   # placa sozinha
python scripts/serial_placa.py auto --porta COM10 --gravar --shield      # placa + shield
```

O script espera o aviso inicial (ou o menu) antes de enviar o comando: o
que chega durante o ~1,5 s em que o bootloader roda, logo após o reset, se
perde.

### Resultados na placa real (2026-10-03, R3-01)

Placa original (ATmega16U2), Vcc de 4,87–4,89 V na USB.

- **Teste da placa com o shield encaixado:** chip, relógio, EEPROM e Vcc
  OK; `gpio` corretamente **pulado** ("ligado: D2 D4 D6 A1").
- **Teste conjunto (opção `a`):** `ok=7 aviso=1`. LM35 em 23,9 °C e
  DHT11 em 28,8 °C. O aviso foi do **shield**: o botão **SW2 (D3) fica em
  LOW sem ninguém apertar**. O D3 lê LOW mesmo com o pull-up interno
  ligado, o que indica botão travado ou curto com o GND no shield (ver o
  README do shield).

### A confirmar na placa real

| Item | Como | Situação |
|------|------|----------|
| Teste `gpio` completo (D2–D12, A1–A5) | Teste da placa **sem** o shield | A rodar |
| Vcc com multímetro | Comparar com o valor do teste `vcc` | A medir |
| Clone com CH340 | Os mesmos testes; o inventário precisa de registro à mão (sem número de série USB) | Sem placa testada |

## Inventário (identificar cada placa)

O ATmega328P **não tem ID único**. No UNO R3 **original**, o chip da USB
(ATmega16U2) tem um **número de série** gravado pela Arduino, diferente em
cada placa, que o computador vê sem gravar nada:

- `python scripts/serial_placa.py listar` → coluna `série=`;
- na IDE: **Ferramentas > Obter informações da placa** (campo SN).

As placas registradas ficam em [`inventario.csv`](inventario.csv), com as
mesmas colunas do [inventário do UNO R4](../arduino-uno-r4/README.md#inventário-identificar-cada-placa).
A etiqueta física é `R3-01`, `R3-02`... e a chave é o número de série USB.
Para registrar, **uma placa por vez**:
`python scripts/serial_placa.py auto --porta COMx --gravar --registrar`.

**Clones com CH340 não têm número de série USB.** O script avisa e não
registra: anote a placa à mão no CSV (coluna `id_unico` vazia e uma
descrição em `obs`). O inventário não guarda dados pessoais.

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

---

**Autor:** Prof. Joao Miguel Roehe ([@professorjoaomiguel](https://github.com/professorjoaomiguel)). Documentação sob licença [CC BY-NC 4.0](https://creativecommons.org/licenses/by-nc/4.0/deed.pt-br); código em `code/` sob licença MIT. Veja como citar no [README principal](../../README.md).
