# Código de teste — Shield Multifunção 9 em 1

Sketch que testa, um a um, os 9 periféricos do shield, com um menu no
Monitor Serial. Cada teste diz o que deve acontecer e o que observar.

| Arquivo | Descrição |
|---------|-----------|
| [`teste_shield_9em1/teste_shield_9em1.ino`](teste_shield_9em1/teste_shield_9em1.ino) | Sketch de teste de todos os periféricos |

## Placas suportadas

| Placa | Compila | Uso de memória |
|-------|---------|----------------|
| [Arduino UNO R3](../../../boards/arduino-uno-r3/README.md) | ✅ | 50% da flash, 11% da RAM |
| [Arduino UNO R4 Minima](../../../boards/arduino-uno-r4/README.md) | ✅ | 20% da flash, 12% da RAM |
| [Arduino UNO R4 WiFi](../../../boards/arduino-uno-r4/README.md) | ✅ | 25% da flash, 20% da RAM |

> ⚠️ Não use em placas de 3,3V (ESP32, etc.): o shield é de 5V. Ver
> [Tensão de operação](../README.md#tensão-de-operação).

**Não precisa instalar nenhuma biblioteca.** O DHT11 e o receptor IR são
lidos diretamente no sketch. O comentário no início do código explica por
quê (a biblioteca IRremote usa o Timer2 do UNO R3, que também controla o
PWM do D11 e o `tone()` do buzzer).

## Como usar

1. Encaixe o shield na placa e conecte a placa ao computador pela USB.
2. Abra `teste_shield_9em1/teste_shield_9em1.ino` na IDE do Arduino.
3. Em **Ferramentas > Placa**, escolha a placa (UNO R3: "Arduino Uno";
   UNO R4: "Arduino UNO R4 Minima" ou "Arduino UNO R4 WiFi") e a porta COM.
   Para o UNO R4, instale antes o pacote **Arduino UNO R4 Boards** no
   Gerenciador de Placas.
4. Grave o sketch (botão **Carregar**).
5. Abra o **Monitor Serial** e configure:
   - velocidade: **9600 baud**
   - final de linha: **Nova linha** (Newline). Com "Sem final de linha",
     o Enter sozinho não envia nada e os testes ficam parados.
6. Digite o número do teste e pressione Enter.

Pela linha de comando (`arduino-cli`):

```bash
arduino-cli compile --fqbn arduino:avr:uno teste_shield_9em1
arduino-cli upload  --fqbn arduino:avr:uno -p COM3 teste_shield_9em1
arduino-cli monitor -p COM3 -c baudrate=9600
```

Para o UNO R4, troque o `--fqbn` por `arduino:renesas_uno:minima` ou
`arduino:renesas_uno:unor4wifi`.

## Menu de testes

| Opção | Teste | O que fazer | Resultado esperado |
|-------|-------|-------------|--------------------|
| 1 | LEDs D12 e D13 | Só observar | Vermelho pisca 3×, azul pisca 3×, os dois juntos |
| 2 | LED RGB D9–D11 | Anotar a cor de cada pino | Uma cor por pino; as três juntas formam o branco; brilho varia suave |
| 3 | Botões SW1 e SW2 | Apertar e soltar | Cada aperto conta 1 e acende um LED (SW1 = vermelho, SW2 = azul) |
| 4 | Buzzer D5 | Ouvir | Apita em um dos níveis (HIGH ou LOW) e toca a escala |
| 5 | Potenciômetro A0 | Girar de ponta a ponta | Leitura vai de ~0 a ~1023; brilho do LED RGB acompanha |
| 6 | LDR A1 | Cobrir e iluminar | Diferença de centenas entre escuro e claro |
| 7 | LM35 A2 | Segurar entre os dedos | Temperatura ambiente, subindo com o calor da mão |
| 8 | DHT11 D4 | Soprar no sensor | Umidade sobe; temperatura parecida com a do LM35 |
| 9 | Receptor IR D6 | Apertar botões de um controle | Mesmo botão gera sempre o mesmo código NEC |
| 0 | Todos | Seguir as instruções | Roda os testes 1 a 9 em sequência |
| p | Painel ao vivo | Mexer em tudo | Todas as entradas em uma linha, a cada 0,5 s |

Nos testes contínuos (3, 5, 6, 7, 8, 9 e o painel), pressione Enter para
voltar ao menu.

## Ajustes no início do código

Alguns detalhes mudam conforme o lote do shield. Os testes indicam o valor
certo de cada constante:

| Constante | Padrão | Quando mudar | Teste que mostra |
|-----------|--------|--------------|------------------|
| `RGB_ANODO_COMUM` | `false` | LED RGB aceso quando deveria estar apagado | 2 |
| `BUZZER_NIVEL_LIGADO` | `HIGH` | Buzzer apita sem parar quando a placa liga | 4 |
| `TENSAO_REFERENCIA` | `5.0` | Temperatura do LM35 acima da real (meça o pino 5V com multímetro) | 7 |

## Problemas comuns

| Sintoma | Causa provável |
|---------|----------------|
| Nada aparece no Monitor Serial | Velocidade diferente de 9600 ou porta COM errada. No UNO R3 clone, falta o driver do CH340 (ver [ponte USB-serial](../../../boards/arduino-uno-r3/README.md#ponte-usb-serial)). |
| Menu aparece, mas o Enter não faz nada | Final de linha em "Sem final de linha": mude para "Nova linha". |
| Buzzer apita sem parar | `BUZZER_NIVEL_LIGADO` trocado (rode o teste 4). |
| DHT11: "não respondeu" | Shield mal encaixado ou sensor com defeito. |
| DHT11: erro de checksum de vez em quando | Normal em leituras isoladas; seguidos indicam mau contato. |
| LM35 marca 2–3 °C a mais que o DHT11 | Referência do ADC abaixo de 5V (comum na USB): ajuste `TENSAO_REFERENCIA`. |
| IR: "sinal recebido, mas não é NEC" | O controle usa outro protocolo (Sony, RC5, TV). O receptor está funcionando. |

## Registro dos resultados

Depois de testar um shield, registre no
[README do shield](../README.md) o que ainda está marcado como "a
confirmar": a cor de cada pino do LED RGB, o tipo de buzzer e o nível que o
liga, se o LDR sobe ou desce com a luz e a tensão medida no `VCC`.
