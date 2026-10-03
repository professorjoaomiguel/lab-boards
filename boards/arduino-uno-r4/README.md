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

Dois sketches dedicados ao UNO R4, que se complementam. Os dois funcionam
com a placa sozinha ou com o
[Shield 9 em 1](../../shields/uno-shield-9in1/README.md) encaixado (o
shield é detectado sozinho, pelos resistores de pull-up dele em D2, D3, D4 e
D6). Nenhum precisa de biblioteca externa.

| Sketch | Quem confere | Para quê |
|--------|--------------|----------|
| [`code/teste_uno_r4_automatico`](code/teste_uno_r4_automatico/teste_uno_r4_automatico.ino) | o próprio sketch | Triagem rápida da placa (lote novo, placa suspeita) e registro no inventário |
| [`code/teste_uno_r4_interativo`](code/teste_uno_r4_interativo/teste_uno_r4_interativo.ino) | o aluno, guiado pelo Monitor Serial | O que só uma pessoa vê, ouve ou mede: LED aceso, buzzer, potenciômetro, multímetro |

> ⚠️ **Antes do teste automático, tire tudo da placa: deixe-a sozinha ou só
> com o Shield 9 em 1.** O teste roda sozinho toda vez que a porta serial é
> aberta, inclusive pelo Monitor Serial da IDE. Sem o shield, ele liga
> **D2 a D13 e A1 a A5 como saída** (HIGH e LOW) e gera tensão no **A0**
> (DAC). Um módulo, protoboard ou outro shield ligado nesses pinos pode
> receber esses sinais e se danificar: o R4 aguenta só 8 mA por pino. Há uma
> proteção parcial (um pino que já está sendo puxado para LOW por algo
> externo não é acionado), mas ela não pega todos os casos.

### Teste automático

| Teste | O que confere | Precisa de |
|-------|---------------|------------|
| `info`, `clock` | Modelo, ID único do chip e clock de 48 MHz | — |
| `relogio` | `millis()` e `micros()` medem 1 s corretamente | — |
| `eeprom` | Grava, lê e **restaura** o último byte da EEPROM (8 KB) | — |
| `rtc` | O relógio de tempo real conta os segundos | — |
| `gpio` | Pull-up interno e saída HIGH/LOW de cada pino livre | sem shield: D2–D13 e A1–A5; com shield: D7, D8, A3–A5 |
| `dac` | O DAC do A0 gera 25/50/75% e o ADC de 14 bits lê de volta | **sem** shield (o potenciômetro do shield fica no A0) |
| `serial1` | O que sai pelo D1 (TX) volta pelo D0 (RX) | jumper entre D0 e D1 (no shield: TXD↔RXD) |
| `avcc` | Tensão de referência do ADC, medida pela própria placa | — |
| `botoes`, `ir`, `saidas`, `dht11`, `lm35`, `ldr`, `pot`, `temperatura` | Periféricos do shield | Shield 9 em 1 |

A saída tem linhas legíveis por programa (`RESULTADO;<teste>;<estado>;<detalhe>`
e `FIM;ok=..;falha=..;aviso=..;pulado=..`). Estados: **OK**, **FALHA**
(defeito), **AVISO** (funcionou, mas o valor é estranho), **PULADO** (não
dava para testar nesta montagem) e **INFO** (só informação). O cabeçalho do
sketch explica cada teste.

### Teste interativo (com ajuda do usuário)

Monitor Serial com final de linha **"Nova linha"**. A cada passo, responda
`s` (funcionou), `n` (não funcionou) ou `p` (pular). Alguns passos medem
sozinhos depois que você age (apertar o botão, girar o potenciômetro, cobrir
o LDR, soprar no DHT11, apertar o controle remoto).

| Passo | O que você faz | Montagem |
|-------|----------------|----------|
| `led_l` | Vê o LED "L" piscar 5 vezes | sempre |
| `tensao_5v` | Mede o pino 5V com o multímetro e digita o valor | sempre |
| `botoes` | Aperta SW1 e depois SW2 | shield |
| `pot_adc` | Gira o potenciômetro de ponta a ponta (mostra 10, 12 e 14 bits) | shield |
| `rgb_pwm` | Vê as 3 cores do LED RGB variando o brilho | shield |
| `buzzer` | Ouve a escala musical | shield |
| `ldr` | Cobre o LDR e depois ilumina com a lanterna | shield |
| `dht11` | Sopra no sensor: a umidade deve subir 5 pontos | shield |
| `ir` | Aperta um botão de qualquer controle remoto | shield |
| `dac` | Mede o A0 (metade da alimentação) e digita o valor | **sem** shield |

### Como rodar

Na IDE do Arduino: placa **Arduino UNO R4 Minima** (ou **WiFi**), grave o
sketch e abra o Monitor Serial. No UNO R4 a velocidade do Monitor não
importa (a serial é USB nativa). O teste começa quando a porta é aberta;
envie `r` para repetir.

Pela linha de comando, o script
[`scripts/serial_placa.py`](../../scripts/serial_placa.py) identifica a
placa pela USB, grava o sketch dedicado, mostra o resultado e registra a
placa no inventário (ajuda completa: `python scripts/serial_placa.py --help`):

```bash
python scripts/serial_placa.py listar                       # portas e placas reconhecidas
python scripts/serial_placa.py auto --gravar --registrar    # grava, testa e registra
python scripts/serial_placa.py interativo --gravar          # teste com ajuda do usuário
```

Feche o Monitor Serial da IDE antes de usar o script: só um programa por vez
usa a porta.

### Resultado na placa real (2026-10-03, R4M-01 + Shield 9 em 1)

`ok=11 falha=0 aviso=1 pulado=3`. Os pulados são esperados com o shield
(`dac`, `serial1` sem jumper e a comparação de temperatura). O aviso foi o
LM35 marcando ~48 °C com o DHT11 em 26 °C: ver "A confirmar" no
[README do shield](../../shields/uno-shield-9in1/README.md#a-confirmar-com-o-shield-em-mãos).

Observações feitas durante o teste:

- **AVCC na USB: 4,78 V** (medida pela própria placa com
  `analogReference()`), e não 5,0 V. Os sketches usam esse valor nas contas
  do ADC.
- **A 1ª leitura do DHT11 depois de ligar vem zerada** (0 °C, 0 %) e passa
  na soma de verificação. O DHT11 entrega a medição *anterior*, que ainda
  não existe. Os sketches descartam essa leitura.
- No R4, `digitalRead()` leva ≈1,2 µs. A leitura do DHT11 por contagem de
  voltas do laço (sem biblioteca) funciona: LOW de 50 µs ≈ 42 voltas, bit 0
  ≈ 17 e bit 1 ≈ 54.

### A confirmar na placa real

| Item | Como | Situação |
|------|------|----------|
| Teste `dac` (DAC → ADC no A0) | Teste automático **sem** o shield | A rodar |
| Teste `serial1` | Teste automático com jumper D0↔D1 | A rodar |
| Teste interativo completo | `serial_placa.py interativo` com o shield | Roteiro validado (tudo pulado); falta rodar respondendo |
| Pino 5V medido com multímetro | Passo `tensao_5v` | A medir (a placa mede 4,78 V de AVCC) |
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

As placas registradas ficam em [`inventario.csv`](inventario.csv), uma
linha por placa física:

| Coluna | Conteúdo |
|--------|----------|
| `etiqueta` | Etiqueta física colada na placa: `R4M-01`, `R4M-02`... (Minima) e `R4W-01`... (WiFi) |
| `modelo` | `UNO R4 Minima` ou `UNO R4 WiFi` |
| `id_unico` | ID único do RA4M1, 32 dígitos hexadecimais (a chave do inventário) |
| `registrado_em` | Data do registro (AAAA-MM-DD) |
| `ultimo_teste`, `resultado` | Data e resumo do último teste automático |
| `obs` | Observações escritas à mão (defeitos, consertos). O script não apaga |

**Por que a etiqueta é sequencial, e não um pedaço do ID:** o ID tem
trechos que parecem texto ASCII (ex: `5A323839` = "Z289"), provavelmente
lote e wafer. Os últimos dígitos podem se repetir entre chips do mesmo lote.
Por isso o ID completo é a chave, e a etiqueta é só um apelido curto.

Para registrar: `python scripts/serial_placa.py auto --gravar --registrar`,
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
