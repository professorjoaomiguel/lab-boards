# Estado do projeto

Log vivo do que já foi feito e do que falta neste repositório. Atualize
este arquivo sempre que adicionar um item novo ou tomar uma decisão
estrutural.

## Itens documentados

| Item | Tipo | Status |
|------|------|--------|
| [ESP32-S3 N16R8 DevKit](../boards/esp32-s3-n16r8/README.md) | placa | Estrutura completa; fotos e diagrama ainda pendentes |
| [ESP32-S3 UNO (TZT D1 ESP32-S3 N16R8)](../boards/esp32-s3-uno/README.md) | placa | Pinagem completa, sketch de teste e inventário por MAC; N16R8 e PSRAM octal confirmadas; foto própria e parte dos itens "a confirmar" pendentes |
| [ESP32-C3 SuperMini](../boards/esp32-c3-supermini/README.md) | placa | Pinagem, comparação com o XIAO e testes nos dois formatos; sketch sem compilar (compilador RISC-V bloqueado), foto e "a confirmar" pendentes |
| [Arduino UNO R3](../boards/arduino-uno-r3/README.md) | placa | Teste da placa (automático) e teste conjunto com o shield; cuidados do ADC documentados; R3-01 (D3 com defeito) e R3-02 no inventário; fotos e "a confirmar" pendentes |
| [Arduino UNO R4 (Minima / WiFi)](../boards/arduino-uno-r4/README.md) | placa | Teste da placa (automático) e teste conjunto com o shield; R4M-01 no inventário; fotos e parte dos "a confirmar" pendentes |
| [Shield Multifunção 9 em 1 (UNO)](../shields/uno-shield-9in1/README.md) | shield | Completo, com sketch de teste; itens "a confirmar" pendentes de teste na placa |

## Decisões tomadas

- **Relatório do inventário** (2026-10-03): `scripts/gerar_inventario.py`
  gera `inventario/relatorio.html` (arquivo único, dados embutidos, abre
  offline) com filtros, busca e contadores. Regerar e commitar junto com o
  CSV a cada mudança. Para ver online, falta ligar o GitHub Pages.
- **Inventário separado do código de teste** (2026-10-03, decisão do
  usuário): `inventario/` com um CSV por tipo de placa e um de shields,
  colunas `etiqueta_colada` e `dono` (professor / SENAI / a confirmar) e,
  no R3, `variante` (ATmega16U2 ou CH340). Placas do professor: o
  ESP32-S3 UNO e o UNO R4 Minima; há R3 do professor (16U2 e CH340) e do
  SENAI (2 ou 3 tipos); shields 9 em 1 do professor e do SENAI. A cada
  placa conectada, registrar o número de série (lista de conhecidas ×
  desconhecidas).
- **Rotinas de teste encerradas nesta etapa** (2026-10-03): o usuário vai
  definir depois uma rotina mais clara e objetiva de teste e identificação
  das placas. Não ampliar os testes até lá; só corrigir defeitos.

- Estrutura do repositório, template de item, sistema de tags e script de
  índice: ver `docs/superpowers/specs/2026-07-23-repositorio-documentacao-placas-design.md`.
- Plano de implementação do scaffold inicial: ver
  `docs/superpowers/plans/2026-07-23-lab-dev-boards-scaffold.md`.
- Seção **Tensão de operação** obrigatória e tag `5v`/`3v3` em todo item,
  para evitar queimar portas misturando placas e shields de tensões
  diferentes (2026-09-24). Ver `.ai/CONVENTIONS.md`.
- `.ai/` como SSoT para agentes de IA: ver o adendo no spec acima
  (2026-07-23).
- Diretiva do repositório (2026-09-28): público, com dois públicos (alunos
  e agentes de IA) e dois modos de acesso (local e direto pelo GitHub).
  `AGENTS.md` na raiz é a entrada para agentes que consultam o repositório.
  As regras ficam em `.ai/CONVENTIONS.md`, seção "Diretiva do repositório".
- Licença e autoria (2026-09-28): documentação sob CC BY-NC 4.0 (`LICENSE`),
  código sob MIT (`LICENSE-CODE`), citação em `CITATION.cff`. Autor
  identificado como Prof. Joao Miguel Roehe (@professorjoaomiguel), sem
  e-mail público.
- Testes pela IDE como caminho oficial para alunos (2026-10-03): os
  sketches de teste do R4 funcionam só com a IDE do Arduino e o Monitor
  Serial (sem Python nem pyserial). O `scripts/serial_placa.py` é
  ferramenta do professor (triagem de lote, inventário). Para isso, os
  sketches do R4 (versão 2): (1) terminam com um RESUMO legível, um teste
  por linha, antes da linha `FIM`; (2) aceitam qualquer opção de final de
  linha do Monitor (fim da mensagem no `\r`/`\n` ou após 200 ms sem
  caracteres); (3) só começam com o comando `c` (abrir a porta mostra só o
  aviso), e o script envia o `c` no modo `auto`. Validado no R4 Minima
  com as três opções de final de linha.
- **Teste da placa × teste conjunto** (2026-10-03, decisão do usuário):
  o teste da placa (`boards/<slug>/code`) é automático, só serial + LED,
  placa sozinha; o teste conjunto placa + shield fica na pasta do shield,
  com menu guiado e opção `a` automática, e informa sobre os dois. Os
  sketches interativos da placa foram removidos. Todos em 115200 baud.
  Regras em `.ai/CONVENTIONS.md`, seção "Código de teste".
- Código de teste dedicado por placa (2026-10-03): cada placa tem os
  próprios sketches, em vez de um sketch genérico com `#ifdef` para várias
  placas (preferência do usuário). Cada placa tem dois formatos: automático
  (sem interação, saída `RESULTADO;...`/`FIM;...` lida por
  `scripts/serial_placa.py`) e interativo (guiado, respostas s/n/p).
- Inventário do UNO R4 (2026-10-03): a chave é o ID único de 128 bits do
  RA4M1, que no Minima é também o número de série USB. A etiqueta física é
  sequencial (`R4M-NN`, `R4W-NN`), não um pedaço do ID: trechos do ID se
  repetem entre chips do mesmo lote.
- Scripts documentados no próprio arquivo (2026-09-28): Python com docstrings
  e `--help` via argparse; PowerShell com ajuda baseada em comentários
  (`Get-Help`). Ver `.ai/CONVENTIONS.md`, seção "Scripts".

## Próximos passos

- **Inventário (em andamento):** colar as etiquetas físicas (nenhuma foi
  colada ainda) e preencher `dono` de cada unidade (R3-01, R3-02 e os
  shields S9-01 a S9-05 estão "a confirmar"). Registrar cada placa nova
  que for conectada. S9-05 não foi testado.
- **Shield S9-04 no R4: LM35 incoerente** (2026-10-03): o ADC leu ~203 mV
  (20,3 °C, estável, 7 °C abaixo do DHT11) e o multímetro mediu 28 mV no
  A2. Hoje também, com o S9-02 no R4, o ADC leu 180/480 mV e o multímetro
  19 mV. Ou o multímetro mede outra coisa nesses casos, ou a leitura do R4
  com a descarga (ADDISCR) está errada. Investigar junto com o LM35
  instável (abaixo), de preferência com osciloscópio.

- Adicionar fotos de `boards/arduino-uno-r3/` e `boards/arduino-uno-r4/`.
- **UNO R4: registrar as outras placas, uma de cada vez** (o laboratório
  só tem uma USB livre). Para cada uma: `python scripts/serial_placa.py
  auto --gravar --registrar`, colar a etiqueta `R4M-NN` que o script
  indicar e commitar `inventario/arduino-uno-r4.csv`. Comparar os IDs entre placas para
  confirmar quais trechos mudam (lote/wafer).
- UNO R4: rodar o teste da placa **sem shield** (`gpio` e `dac`) e com
  jumper D0↔D1 (`serial1`); rodar o teste conjunto novo (`auto --shield`;
  a gravação falhou por LIBUSB em 2026-10-03, resolver com RESET duplo).
  Preencher "A confirmar" no README do R4. Nenhuma UNO R4 WiFi testada.
- **ADC do UNO R4 lê errado o LM35** (achado em 2026-10-03, resolvido): o
  capacitor de amostragem chega carregado do pino anterior e o LM35 não
  absorve corrente (A2 lido a 0,77 V com 0,253 V no multímetro). Solução:
  `R_ADC0->ADDISCR = 0x0F` antes das leituras. Documentado no README do R4
  ("ADC: leitura errada de sensores que não absorvem corrente"). Pendente:
  (1) entender por que o erro persiste até o reset; (2) conferir se
  `analogReference()`/`analogReadResolution()` apagam o ADDISCR; (3) [feito: no R3 o efeito existe, mas passa em ~100 ms] (4) reconferir o 1º
  shield (multímetro marcou 0,47 V no A2 com o ADC parado: provável
  defeito real) com o sketch corrigido.
- **Buzzer do Shield 9 em 1: ativo, liga em HIGH** (2026-10-03, shield
  nº 2 no UNO R4). Com o padrão antigo (`LOW` = ligado), o buzzer apitava
  sem parar no menu; os dois sketches do shield agora usam `HIGH`. Faixa
  de frequência explorada de ouvido (tabela no README do shield). Pendente:
  medir a frequência do apito natural (app de afinador). Conferido
  também no shield do UNO R3-01: mesmo comportamento (2026-10-03).
- Sketch do Shield 9 em 1 separado por placa (2026-10-03):
  `teste_shield_9em1_uno_r3` e `teste_shield_9em1_uno_r4` (só a versão R4
  liga a descarga do ADC).
- **UNO R3: teste automático dedicado feito** (2026-10-03),
  `boards/arduino-uno-r3/code/teste_uno_r3_automatico`, validado na R3-01
  com o shield. Inventário pelo número de série USB do ATmega16U2 (clones
  com CH340 não têm: registro à mão). Pendente: rodar sem shield (`gpio`
  completo), medir o Vcc com multímetro, testar um clone com CH340 e
  decidir se o R3 ganha um teste interativo dedicado (hoje o sketch de
  menu do shield faz esse papel).
- **ADC do UNO R3** (2026-10-03): o LM35 lido logo depois de outro canal
  sai alto (até 445 mV contra 239 mV), mas acerta 100 ms depois da troca;
  depois de `analogReference(INTERNAL)`, ~0,5 s até firmar (capacitor de
  100 nF no AREF). Documentado no README do R3. O AREF do shield **não**
  está ligado a nada (medido sem alimentação).
- **UNO R3-01: pino D3 preso em LOW** (2026-10-03). Primeiro parecia o
  SW2 do shield, mas trocando os shields entre o R3 e o R4 o defeito
  ficou na placa (os dois shields estão bons). Confirmado sem shield: o
  teste da placa acusa só o D3 (`gpio`). Anotado no inventário.
- **LM35 instável nos dois shields** (2026-10-03): a saída muda sozinha
  com o tempo (multímetro no A2: 12 mV a 0,50 V; ADC: 23,3 → 39,8 °C em um
  minuto com o DHT11 estável), em R4, R3-01 e R3-02; não é mau contato.
  Hipótese: o LM35 oscila (sem amortecimento na saída). Pendente: medir
  com osciloscópio; testar o RC do datasheet (75 Ω em série + 1 µF ao GND)
  ou 1–2 kΩ em série num shield. Até lá, usar o DHT11 como referência.
  A correção ADDISCR do R4 continua, mas não resolve isto.
- **Menus do shield aceitam qualquer final de linha** (2026-10-03): os
  pontos de "continuar"/"voltar ao menu" pedem `c` (com "Sem final de
  linha", o Enter vazio não envia nada). Validado no R3 e no R4. Na mesma
  rodada: pausa na parte C do teste do RGB (branco); no teste 3, o LED
  segue o botão na hora (sem esperar o debounce) e um botão em LOW no
  repouso é avisado; LEDs de 3 mm chamados pelo pino (`PINO_LED_D12`,
  `PINO_LED_D13`). **Padrão: D12 vermelho, D13 azul** (decisão do usuário),
  confirmado nos shields testados; pode haver variação de montagem entre lotes.
  No teste 3, SW1 (D2) acende o azul (D13) e SW2 (D3) o vermelho (D12).
- Adicionar fotos e diagrama esquemático reais de `boards/esp32-s3-n16r8/`.
- ESP32-S3 UNO: variante N16R8, PSRAM com `SPIRAM_OCT` e gravação sem
  jumper já confirmadas (2026-09-29). Faltam o LED RGB, a ordem das cores,
  o script MicroPython na placa e as medições da tabela "A confirmar".
- ESP32-S3 UNO: registrar cada placa nova em `inventario/esp32-s3-uno.csv` (MAC, ID de
  128 bits, flash e PSRAM). Registrar não exige regravar o firmware. A 1ª
  placa usada em aula (2026-09-24) ainda não foi registrada.
- ESP32-S3 UNO: escrever um código de verificação nos dois formatos
  (Arduino C/C++ e MicroPython) para levantar informações da placa e
  conferir a pinagem; identificar o módulo com o ESPConnect.
- Definir o produto de referência do ESP32-S3 N16R8 DevKit.
- **Pendente: compilador RISC-V bloqueado pelo Windows (2026-09-28).** O
  Controle de Aplicativo do Windows ("Uma política de Controle de
  Aplicativo bloqueou este arquivo") impede a execução de
  `C:\arduino-data\Arduino15\packages\esp32\tools\esp-rv32\2601\libexec\gcc\riscv32-esp-elf\14.2.0\cc1plus.exe`.
  Sem ele, nada compila para ESP32-C3 (e outros chips RISC-V: C6, H2); o
  ESP32-S3 (Xtensa) compila normalmente. Decidir com o administrador da
  máquina como liberar o pacote esp32 na política de segurança, verificar
  se os computadores do laboratório têm o mesmo bloqueio, e então compilar
  `boards/esp32-c3-supermini/code/teste_esp32_c3_supermini`.
- ESP32-C3 SuperMini: testar numa placa real (LED ativo em LOW ou HIGH,
  botão BOOT, Wi-Fi com e sem `WIFI_POWER_8_5dBm`, firmware MicroPython) e
  preencher a tabela "A confirmar".
- Documentar mais placas e shields conforme forem usados em aula.
- Rodar o sketch de teste do shield 9 em 1 em uma placa real e preencher a
  tabela "A confirmar" do README do shield.
- **Shield 9 em 1 na ESP32-S3 UNO: não compatível sem modificação
  (medido em 2026-10-01).** O `VCC` do shield é o pino 5V do header; o 3V3
  e o IOREF não estão ligados a nada; não há curto 5V↔3V3. Os GPIOs recebem
  5V por pull-ups (D2/D3 ≈10k, D4 ≈3,3k, D6 ≈10k) e **direto** pelo
  potenciômetro (A0). Detalhes em "Medições do circuito" no README do
  shield. Pendente:
  1. Bloco 7, com o shield num **Arduino UNO** ligado (V DC, preta no GND):
     5V; D2 solto/apertado; D4 e D6 em repouso; A0 nos dois extremos; A1
     coberto e com lanterna; nível que liga o buzzer (Teste 4 do sketch).
     Buzzer medido como NPN, divergindo da Keyestudio (PNP).
  2. Antes da modificação: continuidade do **AREF** do shield com qualquer
     coisa (na S3 UNO o AREF vai ao RST) e o código do **receptor IR**
     (precisa aceitar 3,3V).
  3. Modificação proposta pelo usuário: desligar o pino 5V do shield e
     ligar o `VCC` ao 3V3 (preferir forma reversível; etiquetar). Perde o
     LM35. Depois: conferir 5V↔VCC aberto, 3V3↔VCC ≈0 Ω, 3V3↔GND sem
     curto; repetir o bloco 7 na própria S3 (tudo ≤3,3V); atualizar a
     compatibilidade nos READMEs do shield e da S3 UNO.
- **Fora deste repositório: lab-iot.** O passo 6 da seção 5.2 da
  especificação diz "segure o BOOT", mas a ESP32-S3 UNO não tem botão
  BOOT. Corrigir: a gravação é automática pelo CH340 (confirmada em
  2026-09-29). Só se ela falhar, usar o jumper IO0 → GND + RST (ver
  "Gravação" em `boards/esp32-s3-uno/README.md`).
- Preencher `code/` de cada item com código de teste/validação de
  periféricos, quando disponível.
