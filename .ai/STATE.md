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
| [Arduino UNO R4 (Minima / WiFi)](../boards/arduino-uno-r4/README.md) | placa | Estrutura e datasheets completos; fotos pendentes |
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
- Scripts documentados no próprio arquivo (2026-09-28): Python com docstrings
  e `--help` via argparse; PowerShell com ajuda baseada em comentários
  (`Get-Help`). Ver `.ai/CONVENTIONS.md`, seção "Scripts".

## Próximos passos

- Adicionar fotos de `boards/arduino-uno-r3/` e `boards/arduino-uno-r4/`.
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
- **Shield 9 em 1 na ESP32-S3 UNO (2026-09-29): compatibilidade em
  análise.** Os READMEs dizem "não compatível" porque o `VCC` do shield
  viria do pino 5V do header. O shield não gera 5V, mas a S3 UNO fornece 5V
  nesse pino, e os pull-ups, o DHT11, o IR, o potenciômetro e o LDR levariam
  esses 5V aos GPIOs. Nada foi medido ainda. Medições:
  1. Shield solto, continuidade: `VCC` das barras ↔ 5V, ↔ 3V3 e ↔ IOREF.
     5V ↔ 3V3 **não pode** dar continuidade (se der, é curto entre os
     reguladores da S3 UNO).
  2. Resistência de D2, D3, D4, D6, A0 e A1 até `VCC` e até GND (pull-up
     ou pull-down, resistor em série). Código SMD do transistor do buzzer
     (NPN ou PNP) e qual terminal vai ao `VCC`.
  3. Shield num Arduino UNO ligado: `VCC`-GND; D2/D3 com o botão solto e
     apertado; A0 nos dois extremos; A1 com luz forte.
  Com os resultados, atualizar a compatibilidade nos dois READMEs. Se
  `VCC` = 3V3, é quase todo compatível: o LM35 exige ≥4V, e um buzzer PNP
  pode não desligar com 3,3V. Se `VCC` = 5V, é preciso um conversor de
  nível ou alterar o hardware. Opcional: procurar o esquemático do KS0183.
  A análise por periférico é hipótese e não entra no README como fato.
- **Fora deste repositório: lab-iot.** O passo 6 da seção 5.2 da
  especificação diz "segure o BOOT", mas a ESP32-S3 UNO não tem botão
  BOOT. Corrigir: a gravação é automática pelo CH340 (confirmada em
  2026-09-29). Só se ela falhar, usar o jumper IO0 → GND + RST (ver
  "Gravação" em `boards/esp32-s3-uno/README.md`).
- Preencher `code/` de cada item com código de teste/validação de
  periféricos, quando disponível.
