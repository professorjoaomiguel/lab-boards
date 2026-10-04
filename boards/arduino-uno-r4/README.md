---
titulo: "Arduino UNO R4 (Minima / WiFi)"
tipo: placa
autor: "Prof. Joao Miguel Roehe (@professorjoaomiguel)"
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
| Conector Qwiic ([I2C](../../GLOSSARIO.md#i2c)) | — | Sim |
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
- 14 pinos digitais (D0–D13), 6 com [PWM](../../GLOSSARIO.md#pwm) (D3, D5, D6, D9, D10, D11)
- 6 entradas analógicas (A0–A5): [ADC](../../GLOSSARIO.md#adc) de 10 bits por padrão, até 14 bits
  com `analogReadResolution(14)`
- [DAC](../../GLOSSARIO.md#dac) de até 12 bits no A0 (saída analógica real, com `analogWrite(A0, …)`)
- [UART](../../GLOSSARIO.md#uart), I2C, [SPI](../../GLOSSARIO.md#spi) e CAN (D4/D5, requer transceptor externo)
- USB nativa (HID: a placa pode agir como teclado ou mouse)
- Relógio de tempo real (RTC)

### ADC: leitura errada de sensores que não absorvem corrente

**Achado na placa real (2026-10-03), com o [LM35](../../GLOSSARIO.md#lm35) do Shield 9 em 1.** Vale
para qualquer sensor cuja saída só *fornece* corrente, como o LM35.

**Sintoma.** O LM35 marcava de 31 °C a 77 °C numa sala a ~25 °C. O
multímetro no pino A2 mostrava a tensão certa (0,253 V = 25,3 °C), mas o
`analogRead()` lia outra coisa, dependendo do pino lido **antes**:

| Situação (14 bits, referência AVCC = 4,92 V) | A2 lido pelo ADC | A2 no multímetro |
|---|---|---|
| Logo depois de ligar, só o A2 | 302–308 mV (≈31 °C) | — |
| Depois de **uma** leitura do A1 ([LDR](../../GLOSSARIO.md#ldr), ~4,5 V) | **765–777 mV** (≈77 °C) | 0,253 V |
| … e depois de 200 leituras do A2, 5 s parado ou outro canal | continua ~771 mV | — |
| Com a **descarga do capacitor** ligada (abaixo), em qualquer ordem | **259 mV** (≈26 °C) | 0,253 V |

**Causa.** O ADC tem um **capacitor de amostragem** interno (*sample and
hold*): a cada leitura, ele é ligado ao pino, carrega até a tensão do pino
e é medido. Ao trocar de pino, ele chega carregado com a tensão do pino
anterior. Um sensor comum absorve ou fornece corrente e acerta o capacitor
rapidamente. O **LM35 não**: a saída dele fornece até 10 mA, mas quase não
consegue **absorver** corrente. Se o capacitor chega com mais tensão que o
LM35, sobra carga, e o ADC lê alto.

O que ainda **não está explicado**: por que a leitura continua errada por
segundos, mesmo relendo só o A2, e só volta ao normal quando a placa
reinicia. É uma observação, não uma conclusão.

**O que NÃO resolveu:** esperar (`delay`), repetir leituras, ler outro
canal antes, reconfigurar o pino com `pinMode(A2, INPUT)` e ler o LM35
primeiro (a 1ª leitura já vinha ~50 mV, ou 5 °C, acima do real).

**Solução: descarregar o capacitor antes de cada conversão.** O RA4M1 tem
um registrador para isso, o `ADDISCR` (*A/D Disconnection Detection
Control*). Com o campo `ADNDIS` em 15, o capacitor é ligado ao GND por 15
ciclos de clock do ADC antes de cada conversão. Partindo de 0 V, o LM35 só
precisa fornecer corrente, que ele faz bem:

```cpp
// Antes de ler um sensor que não absorve corrente (ex: LM35):
R_ADC0->ADDISCR = 0x0F;           // descarga por 15 ciclos antes de cada conversão
int leitura = analogRead(A2);
```

Com isso, o LDR (A1) e o potenciômetro (A0) leram exatamente igual. O
ajuste vale até o ADC ser reconfigurado. O core do Arduino reconfigura o
ADC em funções como `analogReference()` e `analogReadResolution()`, e não
foi conferido se isso apaga o `ADDISCR`. Por segurança, os sketches deste
repositório escrevem o registrador logo antes das leituras do LM35.

**E no UNO R3?** O ADC do ATmega328P não tem esse registrador. O efeito
não foi testado no R3 (os sketches do R3 não mudam). Por isso o código de
teste do shield tem uma versão por placa.

### Diferenças para o UNO R3 que afetam os exemplos
- `Serial` é a USB e `Serial1` são os pinos D0/D1 (no R3, os dois são a
  mesma porta).
- Alguns sketches de AVR que acessam registradores diretamente (`PORTB`,
  `DDRD`, etc.) não funcionam no RA4M1.
- Leitura do LM35 (e de outros sensores que não absorvem corrente): ligue
  a descarga do ADC (seção acima). No R3, o mesmo código lê sem esse ajuste.

## Ponte USB-serial
Diferente do UNO R3, o UNO R4 **não tem um chip dedicado de ponte
USB-serial** (nem [ATmega16U2](../../GLOSSARIO.md#atmega16u2), nem [CH340](../../GLOSSARIO.md#ch340)). Como não há chip de ponte, não é
preciso instalar driver extra no Windows.

- **UNO R4 Minima:** o RA4M1 tem USB nativa. O conector USB-C vai direto ao
  microcontrolador, que grava o sketch, faz o `Serial` e pode agir como
  teclado ou mouse (HID). Se um sketch travar a USB e a placa sumir da
  porta COM, **aperte RESET duas vezes rápido** para entrar no [bootloader](../../GLOSSARIO.md#bootloader).
- **UNO R4 WiFi:** por padrão, o USB-C vai ao **ESP32-S3**, que faz a ponte
  USB-serial até o RA4M1 (é ele quem grava o RA4M1). A comunicação pode ser
  desviada direto para a USB nativa do RA4M1, por software (pino P408/D40
  em nível alto) ou de forma permanente (soldando o jumper SJ1).

Em nenhuma das versões os pinos D0/D1 estão ligados à USB: eles formam o
`Serial1`, livre para módulos externos.

## Código de teste e validação

Há dois tipos de teste, cada um com o seu sketch:

| Teste | Sketch | Como é | Para quê |
|-------|--------|--------|----------|
| **Só a placa** | [`code/teste_uno_r4_automatico`](code/teste_uno_r4_automatico/teste_uno_r4_automatico.ino) | Automático, só pela serial, **placa sozinha** (sem shield) | Triagem rápida (lote novo, placa suspeita) e registro no inventário |
| **Placa + Shield 9 em 1** | [`teste_shield_9em1_uno_r4`](../../shields/uno-shield-9in1/code/teste_shield_9em1_uno_r4/teste_shield_9em1_uno_r4.ino) (na pasta do shield) | Menu com testes guiados (ver, ouvir, apertar) e uma opção automática | Testar o shield e a placa juntos; informa sobre os dois |

Nenhum precisa de biblioteca externa. Os dois usam **115200 [baud](../../GLOSSARIO.md#baud)** e
aceitam qualquer opção de final de linha do [Monitor Serial](../../GLOSSARIO.md#monitor-serial).

**Sinal de "firmware de teste gravado":** enquanto o sketch espera um
comando, o LED **L** (D13) pisca **duas vezes rápidas a cada 2 s**, um
"tum-tum" diferente do Blink comum. Dá para reconhecer de longe uma placa
com o firmware de teste.

> ⚠️ **O teste da placa é com a placa sozinha.** Ele liga **D2 a D13 e A1 a
> A5 como saída** (HIGH e LOW) e gera tensão no **A0** (DAC). Um módulo,
> protoboard ou shield ligado nesses pinos pode receber esses sinais e se
> danificar: o R4 aguenta só 8 mA por pino. Proteção: antes de acionar os
> pinos, o sketch procura resistores externos neles (como os pull-ups do
> Shield 9 em 1). Se achar, **não aciona nenhum pino** e marca `gpio` e
> `dac` como pulados, dizendo em quais pinos achou algo. A proteção não pega
> todos os circuitos: tire tudo da placa antes. O teste só começa quando
> você envia `c`.

### Teste da placa (automático)

| Teste | O que confere |
|-------|---------------|
| `info`, `clock` | Modelo, ID único do chip e clock de 48 MHz |
| `relogio` | `millis()` e `micros()` medem 1 s corretamente |
| `eeprom` | Grava, lê e **restaura** o último byte da EEPROM (8 KB) |
| `rtc` | O relógio de tempo real conta os segundos |
| `avcc` | Tensão de referência do ADC, medida pela própria placa |
| `gpio` | [Pull-up](../../GLOSSARIO.md#pull-up-e-pull-down) interno e saída HIGH/LOW de D2–D12 e A1–A5 (D13 só saída: o LED L puxa o pino) |
| `dac` | O DAC do A0 gera 25/50/75% e o ADC de 14 bits lê de volta |
| `serial1` | O que sai pelo D1 (TX) volta pelo D0 (RX), com um jumper entre D0 e D1; sem jumper, fica "pulado" |

A saída tem linhas legíveis por programa (`RESULTADO;<teste>;<estado>;<detalhe>`
e `FIM;ok=..;falha=..;aviso=..;pulado=..`) e termina com um **RESUMO**
legível. Estados: **OK**, **FALHA** (defeito), **AVISO** (funcionou, mas o
valor é estranho), **PULADO** (não dava para testar nesta montagem) e
**INFO** (só informação). O cabeçalho do sketch explica cada teste.

### Teste conjunto (placa + Shield 9 em 1)

Fica na pasta do shield, um sketch por placa. Ao abrir o Monitor Serial,
ele mostra a placa (modelo, ID, AVCC) e o menu: os testes **1 a 9** são
guiados (ver os LEDs, ouvir o buzzer, apertar os botões, girar o
potenciômetro…) e a opção **`a`** roda um teste automático do shield com
resumo. Detalhes em
[Código de teste do shield](../../shields/uno-shield-9in1/code/README.md).

### Como rodar

Há dois caminhos, com os mesmos sketches. **Para os alunos, o caminho 1
(só a IDE) basta.** O caminho 2 é a ferramenta do professor: triagem de um
lote de placas e registro no inventário.

#### Caminho 1: só a IDE do Arduino (nada para instalar além da IDE)

1. Em **Ferramentas > Placa**, escolha **Arduino UNO R4 Minima** (ou
   **WiFi**). Se não aparecer, instale o pacote **Arduino UNO R4 Boards**
   no Gerenciador de Placas.
2. Abra o sketch, escolha a porta COM e clique em **Carregar**.
3. Abra o **Monitor Serial** em **115200 baud** (no UNO R4 a velocidade
   nem importa, porque a serial é USB nativa). A opção de final de linha
   também não importa.
4. Teste da placa: aparece um aviso; **envie `c`** para começar (digite
   `c` na caixa de envio e tecle Enter). Nada é testado antes disso, nem
   quando a IDE reabre o Monitor depois de gravar.
   Teste conjunto: digite o número do teste do menu (ou `a`) e envie.
5. No fim aparece o **RESUMO**, um teste por linha:

   ```text
     [  OK  ] Clock do processador ...... 48 MHz
     [  OK  ] Referência do ADC ......... 4.90 V
     [pulado] Serial1 (D0/D1) ........... nada recebido (sem jumper entre D0 e D1)
   --------------------------------------------------------------
     OK: 5   FALHA: 0   AVISO: 0   pulados: 1
     Resultado: tudo certo.
   ```

   Antes do resumo aparecem linhas como `RESULTADO;clock;OK;...`. Elas são
   para o script do caminho 2 e podem ser ignoradas. Envie `c` para rodar
   de novo.

> **Sem final de linha:** só o Enter, com a caixa vazia, não envia nada.
> Por isso o comando para começar é a letra `c`, e não só o Enter.

**Se a gravação falhar** ("No [DFU](../../GLOSSARIO.md#dfu) capable USB device" ou "LIBUSB_ERROR"),
aperte o **RESET duas vezes rápido**: o LED L passa a pulsar devagar (modo
bootloader) e a gravação volta a funcionar. Se ainda falhar, desconecte e
reconecte o cabo USB. Visto na placa real depois de várias gravações
seguidas.

#### Caminho 2: pelo terminal, com o script `serial_placa.py`

O script [`scripts/serial_placa.py`](../../scripts/serial_placa.py)
identifica a placa pela USB, grava o sketch certo, mostra o resultado e
registra a placa no inventário.

**Preparar o computador (uma vez):**

1. Instale o **Python 3.8 ou mais novo** ([python.org](https://www.python.org/downloads/)).
   No Windows, marque **"Add python.exe to PATH"** no instalador.
2. No terminal: `pip install pyserial` (a biblioteca que abre a porta
   serial).
3. Só para gravar pelo script (`--gravar`): instale o
   [arduino-cli](https://arduino.github.io/arduino-cli/latest/installation/)
   e o pacote da placa: `arduino-cli core install arduino:renesas_uno`.
   Sem ele, grave o sketch pela IDE (caminho 1) e use o script só para
   testar.
4. Na pasta raiz do repositório, confira se está tudo pronto:

   ```bash
   python scripts/serial_placa.py verificar
   ```

   Ele mostra `OK`, `AVISO` ou `FALHA` para o Python, o pyserial, o
   arduino-cli, os pacotes de placa e as placas ligadas, e diz como
   corrigir o que faltar.

**Testar:**

```bash
python scripts/serial_placa.py auto --gravar --registrar   # placa sozinha: grava, testa e registra
python scripts/serial_placa.py auto --gravar --shield      # placa + shield: opção "a" do menu
python scripts/serial_placa.py interativo --gravar         # placa + shield: menu de testes guiados
```

- Sem `--gravar`, o script usa o sketch que já está na placa.
- No **interativo**, o que a placa envia aparece no terminal: digite a
  opção do menu ou a resposta e tecle **Enter**. **Ctrl+C** sai.
- Com mais de uma placa ligada, indique qual com `--porta COM8`
  (`python scripts/serial_placa.py listar` mostra as portas).
- Ajuda completa: `python scripts/serial_placa.py --help`.

**Feche o Monitor Serial da IDE antes de usar o script:** só um programa
por vez usa a porta. Se aparecer `não foi possível abrir COM8`, é isso.

### Resultados na placa real (2026-10-03, R4M-01)

- **Teste da placa com o shield encaixado:** `ok=5`, com `gpio` e `dac`
  corretamente **pulados** ("algo ligado em D2 D3 D4 D6 A1 A4 A5") e
  `serial1` pulado (sem jumper). AVCC de 4,90 V.
- **Teste conjunto (opção `a`, shield nº 2):** LM35 em 27,2 °C e [DHT11](../../GLOSSARIO.md#dht11) em
  25,0 °C (diferença de 2,2 °C), botões e IR em repouso corretos. Com a
  correção do ADC (ver
  [ADC: leitura errada de sensores que não absorvem corrente](#adc-leitura-errada-de-sensores-que-não-absorvem-corrente)).

Observações feitas durante os testes:

- **AVCC na USB: 4,78 a 4,93 V** (medida pela própria placa com
  `analogReference()`), e não 5,0 V. Os sketches usam esse valor nas contas
  do ADC.
- **A 1ª leitura do DHT11 depois de ligar vem zerada** (0 °C, 0 %) e passa
  na soma de verificação. O DHT11 entrega a medição *anterior*, que ainda
  não existe. Os sketches descartam essa leitura.
- No R4, `digitalRead()` leva ≈1,2 µs. A leitura do DHT11 por contagem de
  voltas do laço (sem biblioteca) funciona: LOW de 50 µs ≈ 42 voltas, bit 0
  ≈ 17 e bit 1 ≈ 54.
- **Depois de um `analogWrite()`, o `digitalWrite()` não controla mais o
  pino** no R4: o pino fica com o timer do PWM. Para voltar a usá-lo como
  saída digital, chame `pinMode(pino, OUTPUT)` de novo. (No UNO R3,
  `analogWrite(pino, 0)` vira um simples `digitalWrite(LOW)`, por isso o
  mesmo código não mostra o problema.)

### A confirmar na placa real

| Item | Como | Situação |
|------|------|----------|
| Testes `gpio` e `dac` | Teste da placa **sem** o shield | A rodar |
| Teste `serial1` | Teste da placa com jumper D0↔D1 | A rodar |
| Teste conjunto, versão nova | `serial_placa.py auto --shield` no R4 | A rodar (a gravação falhou por LIBUSB no dia; a versão anterior passou) |
| UNO R4 WiFi | Os mesmos sketches (compilam para `unor4wifi`) | Sem placa testada |

## Inventário (identificar cada placa)

Cada RA4M1 sai de fábrica com um **ID único de 128 bits**, gravado no chip
e impossível de apagar. No **UNO R4 Minima**, o core do Arduino usa esse ID
como **número de série da USB** (`cores/arduino/usb/USB.cpp`, função
`R_BSP_UniqueIdGet()`). Por isso o computador vê o ID **sem gravar nada**:

- `python scripts/serial_placa.py listar` → coluna `série=`;
- `arduino-cli board list --format json` → campo `serialNumber`;
- Gerenciador de Dispositivos → Propriedades → Detalhes → "Caminho da
  instância do dispositivo" (o trecho depois de `USB\VID_2341&PID_0069\`).

No **UNO R4 WiFi**, o número de série USB é o do ESP32-S3 (a ponte USB), e
não o do RA4M1. Nele, só o sketch automático mostra o ID (linha `ID_UNICO`).

As placas registradas ficam no inventário do laboratório, separado do
código de teste: [`inventario/arduino-uno-r4.csv`](../../inventario/arduino-uno-r4.csv),
uma linha por placa física. As colunas (etiqueta, dono, variante, ID,
resultado do último teste...) estão explicadas em
[`inventario/README.md`](../../inventario/README.md). Etiquetas: `R4M-01`,
`R4M-02`... (Minima) e `R4W-01`... (WiFi).

**Por que a etiqueta é sequencial, e não um pedaço do ID:** o ID tem
trechos que parecem texto ASCII (ex: `5A323839` = "Z289"), provavelmente
lote e wafer. Os últimos dígitos podem se repetir entre chips do mesmo lote.
Por isso o ID completo é a chave, e a etiqueta é só um apelido curto.

Para registrar (placa sozinha): `python scripts/serial_placa.py auto --gravar --registrar`,
**uma placa por vez**. Uma placa nova ganha a próxima etiqueta; uma já
registrada só tem o último teste atualizado. O inventário não guarda dados
pessoais: quem está com cada placa não entra neste repositório público.

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

---

**Autor:** Prof. Joao Miguel Roehe ([@professorjoaomiguel](https://github.com/professorjoaomiguel)). Documentação sob licença [CC BY-NC 4.0](https://creativecommons.org/licenses/by-nc/4.0/deed.pt-br); código em `code/` sob licença MIT. Veja como citar no [README principal](../../README.md).
