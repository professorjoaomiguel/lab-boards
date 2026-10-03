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
| [Arduino UNO R3](../boards/arduino-uno-r3/README.md) | placa | Estrutura e datasheets completos; fotos pendentes |
| [Arduino UNO R4 (Minima / WiFi)](../boards/arduino-uno-r4/README.md) | placa | Testes automático e interativo (com Shield 9 em 1), inventário por ID único (R4M-01 registrada); fotos e parte dos "a confirmar" pendentes |
| [Shield Multifunção 9 em 1 (UNO)](../shields/uno-shield-9in1/README.md) | shield | Completo, com sketch de teste; itens "a confirmar" pendentes de teste na placa |

## Decisões tomadas

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

- Adicionar fotos de `boards/arduino-uno-r3/` e `boards/arduino-uno-r4/`.
- **UNO R4: registrar as outras placas, uma de cada vez** (o laboratório
  só tem uma USB livre). Para cada uma: `python scripts/serial_placa.py
  auto --gravar --registrar`, colar a etiqueta `R4M-NN` que o script
  indicar e commitar o `inventario.csv`. Comparar os IDs entre placas para
  confirmar quais trechos mudam (lote/wafer).
- UNO R4: rodar o teste automático **sem shield** (testes `dac` e `gpio`
  completo) e com jumper D0↔D1 (`serial1`); rodar o teste interativo
  respondendo de verdade; medir o pino 5V. Preencher "A confirmar" no
  README do R4. Nenhuma UNO R4 WiFi testada ainda.
- **LM35 com defeito no Shield 9 em 1 em uso** (2026-10-03): o multímetro
  mediu 0,47 V no A2 com a sala a 26 °C, o mesmo que o ADC do UNO R4 leu.
  A leitura do código está certa. Um shield novo foi pedido. Quando ele
  chegar: rodar o teste automático do R4 (esperado: `lm35` e `temperatura`
  OK) e conferir se o A2 ainda sobe (~0,1 V) quando é lido logo depois do
  A1 (possível oscilação do LM35).
- **UNO R3: teste automático dedicado adiado** (2026-10-03), para quando
  houver um UNO R3 ligado. Criar `boards/arduino-uno-r3/code/teste_uno_r3_automatico`
  no mesmo formato de saída do R4 (sem RTC, DAC, Serial1 nem ID único; dá
  para medir o Vcc pela referência interna de 1,1V) e preencher
  `sketch_auto` de `uno-r3` em `scripts/serial_placa.py`.
- Adicionar fotos e diagrama esquemático reais de `boards/esp32-s3-n16r8/`.
- ESP32-S3 UNO: variante N16R8, PSRAM com `SPIRAM_OCT` e gravação sem
  jumper já confirmadas (2026-09-29). Faltam o LED RGB, a ordem das cores,
  o script MicroPython na placa e as medições da tabela "A confirmar".
- ESP32-S3 UNO: registrar cada placa nova em `inventario.csv` (MAC, ID de
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
