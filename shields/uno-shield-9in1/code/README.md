# Código de teste — Shield Multifunção 9 em 1

Sketch que testa, um a um, os 9 periféricos do shield, com um menu no
Monitor Serial. Cada teste diz o que deve acontecer e o que observar.

Há **uma versão dedicada para cada placa**, com o mesmo menu. Use a da sua
placa: a outra não compila (uma trava no início do código avisa).

| Arquivo | Placa | Uso de memória |
|---------|-------|----------------|
| [`teste_shield_9em1_uno_r3/teste_shield_9em1_uno_r3.ino`](teste_shield_9em1_uno_r3/teste_shield_9em1_uno_r3.ino) | [Arduino UNO R3](../../../boards/arduino-uno-r3/README.md) | 51% da flash, 11% da RAM |
| [`teste_shield_9em1_uno_r4/teste_shield_9em1_uno_r4.ino`](teste_shield_9em1_uno_r4/teste_shield_9em1_uno_r4.ino) | [Arduino UNO R4](../../../boards/arduino-uno-r4/README.md) Minima / WiFi | Minima: 20% da flash, 12% da RAM. WiFi: 25% e 20% |

A única diferença está na leitura do LM35: no UNO R4, o sketch liga a
descarga do capacitor de amostragem do ADC antes de cada leitura. Sem isso,
o LM35 chega a marcar ~50 °C a mais depois que outro pino é lido (ver
[ADC do UNO R4](../../../boards/arduino-uno-r4/README.md#adc-leitura-errada-de-sensores-que-não-absorvem-corrente)).

No UNO R4 também há os testes **automático** e **interativo** da própria
placa, que testam o shield quando ele está encaixado (ver
[Código de teste do UNO R4](../../../boards/arduino-uno-r4/README.md#código-de-teste-e-validação)).

> ⚠️ Não use em placas de 3,3V (ESP32, etc.): o shield é de 5V. Ver
> [Tensão de operação](../README.md#tensão-de-operação).

**Não precisa instalar nenhuma biblioteca.** O DHT11 e o receptor IR são
lidos diretamente no sketch. O comentário no início do código explica por
quê (a biblioteca IRremote usa o Timer2 do UNO R3, que também controla o
PWM do D11 e o `tone()` do buzzer).

## Como usar

1. Encaixe o shield na placa e conecte a placa ao computador pela USB.
2. Abra na IDE do Arduino o sketch da sua placa: `teste_shield_9em1_uno_r3` ou
   `teste_shield_9em1_uno_r4`.
3. Em **Ferramentas > Placa**, escolha a placa (UNO R3: "Arduino Uno";
   UNO R4: "Arduino UNO R4 Minima" ou "Arduino UNO R4 WiFi") e a porta COM.
   Para o UNO R4, instale antes o pacote **Arduino UNO R4 Boards** no
   Gerenciador de Placas.
4. Grave o sketch (botão **Carregar**).
5. Abra o **Monitor Serial** e configure:
   - velocidade: **115200 baud** (a mesma dos testes da placa; no UNO R4 tanto faz)
   - final de linha: **qualquer opção** funciona.
6. Digite o número do teste e envie. Quando o teste pedir para continuar
   ou voltar ao menu, **envie `c`**. (Com "Nova linha", só o Enter também
   serve; com "Sem final de linha", o Enter com a caixa vazia não envia
   nada, por isso o teste pede a letra `c`.)

Pela linha de comando (`arduino-cli`):

```bash
arduino-cli compile --fqbn arduino:avr:uno teste_shield_9em1_uno_r3
arduino-cli upload  --fqbn arduino:avr:uno -p COM3 teste_shield_9em1_uno_r3
arduino-cli monitor -p COM3 -c baudrate=115200
```

Para o UNO R4, use a pasta `teste_shield_9em1_uno_r4` e o `--fqbn`
`arduino:renesas_uno:minima` ou `arduino:renesas_uno:unor4wifi`.

## Menu de testes

| Opção | Teste | O que fazer | Resultado esperado |
|-------|-------|-------------|--------------------|
| 1 | LEDs D12 e D13 | Só observar e conferir a cor | O do D12 (vermelho, no padrão) pisca 3×, o do D13 (azul) pisca 3×, os dois juntos. Há variação de montagem: anote se as cores vieram trocadas |
| 2 | LED RGB D9–D11 | Conferir a cor de cada pino | D9 vermelho, D10 azul, D11 verde; as três juntas formam o branco; brilho varia suave |
| 3 | Botões SW1 e SW2 | Apertar e soltar | Cada aperto conta 1. O LED acende na hora e fica aceso enquanto o botão estiver apertado: SW1 → LED azul (D13), SW2 → LED vermelho (D12). Botão em LOW no repouso é avisado (travado ou em curto) |
| 4 | Buzzer D5 | Ouvir | Buzzer ativo (o testado): apita no nível HIGH; escala e melodia "Nokia Tune" com o ritmo certo, timbre misturado com o apito próprio |
| 5 | Potenciômetro A0 | Girar de ponta a ponta | Leitura vai de ~0 a ~1023; brilho do vermelho do LED RGB acompanha |
| 6 | LDR A1 | Cobrir e iluminar | Leitura sobe com a luz; diferença de centenas entre escuro e claro |
| 7 | LM35 A2 | Segurar entre os dedos | Temperatura ambiente, subindo com o calor da mão |
| 8 | DHT11 D4 | Soprar no sensor | Umidade sobe; temperatura parecida com a do LM35 |
| 9 | Receptor IR D6 | Apertar botões de um controle | Mesmo botão gera sempre o mesmo código NEC |
| 0 | Todos | Seguir as instruções | Roda os testes 1 a 9 em sequência |
| p | Painel ao vivo | Mexer em tudo | Todas as entradas em uma linha, a cada 0,5 s |

Nos testes contínuos (3, 5, 6, 7, 8, 9 e o painel), envie `c` para voltar
ao menu.

## Ajustes no início do código

Alguns detalhes mudam conforme o lote do shield. Os testes indicam o valor
certo de cada constante:

| Constante | Padrão | Quando mudar | Teste que mostra |
|-----------|--------|--------------|------------------|
| `RGB_ANODO_COMUM` | `false` | Não precisa mudar: cátodo comum (ativo em HIGH) confirmado na placa | 2 |
| `BUZZER_NIVEL_LIGADO` | `HIGH` | Só se o buzzer do seu shield apitar sem parar com a placa parada no menu: troque para `LOW` (transistor PNP). O padrão segue o transistor NPN medido | 4 |
| `TENSAO_REFERENCIA` | `5.0` | Temperatura do LM35 acima da real (meça o pino 5V com multímetro) | 7 |

## Problemas comuns

| Sintoma | Causa provável |
|---------|----------------|
| Nada aparece no Monitor Serial | Velocidade diferente de 115200 ou porta COM errada. No UNO R3 clone, falta o driver do CH340 (ver [ponte USB-serial](../../../boards/arduino-uno-r3/README.md#ponte-usb-serial)). |
| O teste pede para continuar e o Enter não faz nada | Com "Sem final de linha", o Enter com a caixa vazia não envia nada: digite `c` e envie. |
| Buzzer apita sem parar | `BUZZER_NIVEL_LIGADO` trocado (rode o teste 4). |
| DHT11: "não respondeu" | Shield mal encaixado ou sensor com defeito. |
| DHT11: erro de checksum de vez em quando | Normal em leituras isoladas; seguidos indicam mau contato. |
| LM35 marca 2–3 °C a mais que o DHT11 | Referência do ADC abaixo de 5V (comum na USB): ajuste `TENSAO_REFERENCIA`. |
| UNO R4: LM35 marca 20 a 50 °C a mais que o DHT11 | Particularidade do ADC do R4 com o LM35, já tratada na versão `teste_shield_9em1_uno_r4` (confira se gravou a versão R4). Se o multímetro entre A2 e GND também marcar alto (25 °C = 0,25 V), aí é o LM35 com defeito. Ver "A confirmar" no [README do shield](../README.md#a-confirmar-com-o-shield-em-mãos). |
| DHT11 mostra 0 °C e 0 % logo depois de ligar | 1ª leitura do sensor vem zerada; o sketch agora a descarta como erro. A próxima leitura já vem certa. |
| IR: "sinal recebido, mas não é NEC" | O controle usa outro protocolo (Sony, RC5, TV). O receptor está funcionando. |

## Registro dos resultados

Depois de testar um shield, registre no
[README do shield](../README.md) o que ainda está marcado como "a
confirmar": a cor de cada pino do LED RGB, o tipo de buzzer e o nível que o
liga, se o LDR sobe ou desce com a luz e a tensão medida no `VCC`.
