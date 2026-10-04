# Estado do projeto

O que já foi feito, o que vale e o que falta neste repositório. É o
backlog oficial: qualquer agente (ou pessoa) deve conseguir retomar o
trabalho do zero lendo só este arquivo e os links dele.

## Como manter este arquivo

- **Uma linha por assunto, com link.** O detalhe técnico (medições,
  hipóteses, código) fica no README do item; aqui vai o resumo e o link
  para a seção. Se o detalhe ainda não tem lugar num README, crie a seção
  lá primeiro.
- **Em aberto:** cada item começa com a data em que surgiu. Ao concluir,
  tire de "Em aberto" e ponha uma linha em "Resolvido" (data + o que
  mudou + onde ficou documentado). Nunca apague um item em aberto sem
  concluir ou sem o usuário descartar.
- **Decisões:** só as que valem hoje. Quando uma decisão nova substitui
  outra, edite a antiga (ou apague e cite na nova "substitui ..."), para
  não haver duas regras conflitantes.
- **Mesmo commit:** atualize o STATE no mesmo commit da mudança que o
  afeta (item novo, teste rodado, decisão tomada).
- **Resolvido:** guarda só os últimos ~30 dias; o histórico completo está
  no `git log`.
- **Conferir:** `python scripts/verificar_state.py` (links, idade do
  Resolvido, limite de 300 linhas). Também roda na suíte de testes.
- **Se crescer demais** (passar do limite, ou um tópico dominar o
  arquivo): mover o detalhamento para `.ai/state/<tópico ou placa>.md` e
  deixar aqui só uma linha com o ponteiro.
- **Handoff do `/remember`:** não copia o backlog; aponta para cá. Item em
  aberto que só exista no handoff entra aqui antes (regra em
  `.ai/CONVENTIONS.md`, seção "Handoff do `/remember`").

## Itens documentados

| Item | Tipo | Status |
|------|------|--------|
| [Arduino UNO R3](../boards/arduino-uno-r3/README.md) | placa | Teste da placa (automático) e teste conjunto com o shield; R3-01 e R3-02 no inventário; medição do Vcc, clone CH340 e fotos pendentes |
| [Arduino UNO R4 (Minima / WiFi)](../boards/arduino-uno-r4/README.md) | placa | Teste da placa (automático) e teste conjunto com o shield; R4M-01 no inventário; teste sem shield, R4 WiFi e fotos pendentes |
| [ESP32-S3 UNO (TZT D1 ESP32-S3 N16R8)](../boards/esp32-s3-uno/README.md) | placa | Pinagem completa, sketch C++ e MicroPython, `1e:20` no inventário; N16R8 e PSRAM octal confirmadas; LED RGB, "a confirmar" e foto pendentes |
| [ESP32-C3 SuperMini](../boards/esp32-c3-supermini/README.md) | placa | Pinagem e testes nos dois formatos; sketch **nunca compilado** (compilador RISC-V bloqueado); placa real, foto e "a confirmar" pendentes |
| [ESP32-S3 N16R8 DevKit](../boards/esp32-s3-n16r8/README.md) | placa | Só a ficha; sem código, fotos nem produto de referência definido |
| [Shield Multifunção 9 em 1 (UNO)](../shields/uno-shield-9in1/README.md) | shield | Testado no R3 e no R4 (sketch por placa); LM35 instável; modificação para 3,3V em estudo |

## Em aberto

### Aguardando decisão ou terceiros

- (2026-10-03) **Rotina de teste e identificação das placas:** o usuário
  vai definir uma rotina mais clara e objetiva. Até lá, não ampliar os
  testes; só corrigir defeitos.
- (2026-09-28) **Compilador RISC-V bloqueado pelo Windows:** o Controle
  de Aplicativo bloqueia o `cc1plus.exe` do `esp-rv32`
  (`C:\arduino-data\Arduino15\packages\esp32\tools\esp-rv32\2601\...`).
  Nada compila para ESP32-C3/C6/H2; o S3 (Xtensa) compila. Decidir com o
  administrador da máquina (não mexer nas configurações de segurança) e
  verificar se os outros computadores do laboratório têm o mesmo bloqueio.

### Inventário

- (2026-10-03) Colar as etiquetas físicas: **nenhuma foi colada**. A
  R3-01 tem o D3 com defeito, então é a mais urgente.
- (2026-10-03) Preencher o `dono` dos shields S9-01 a S9-05 (todos "a
  confirmar"). Testar o S9-05.
- (2026-10-03) Registrar cada placa nova conectada, **sozinha** e uma por
  vez (o laboratório tem só uma USB livre):
  `python scripts/serial_placa.py auto --porta COMx --gravar --registrar`,
  perguntar o dono, regerar o relatório e commitar CSV + relatório.
  Faltam as outras R4 (comparar os IDs para ver quais trechos mudam por
  lote) e os R3 do SENAI (2 ou 3 tipos).
- (2026-09-24) ESP32-S3 UNO: confirmar se a 1ª placa usada em aula é a
  `1e:20`; se não for, registrar (não exige regravar o firmware).

### Investigações de hardware

- (2026-10-03) **LM35 instável** nos shields S9-01 e S9-02 (a saída muda
  sozinha, em R4, R3-01 e R3-02). Medir com osciloscópio; testar o RC do
  datasheet num shield. Ver [LM35 instável](../shields/uno-shield-9in1/README.md#lm35-instável-2026-10-03).
  Até lá, usar o DHT11 como referência em aula.
- (2026-10-03) **S9-04 no R4:** o ADC leu ~203 mV e o multímetro 28 mV no
  A2 (com o S9-02, 180/480 mV contra 19 mV). Ou o multímetro mede outra
  coisa, ou a leitura com a descarga (ADDISCR) está errada. Investigar
  junto com o LM35 instável.
- (2026-10-03) **ADC do R4 (ADDISCR):** entender por que o erro persiste
  até o reset; conferir se `analogReference()`/`analogReadResolution()`
  apagam o ADDISCR; reconferir o S9-01 (0,47 V no A2 com o ADC parado).
  Ver [ADC do UNO R4](../boards/arduino-uno-r4/README.md#adc-leitura-errada-de-sensores-que-não-absorvem-corrente).
- (2026-10-03) Buzzer do shield: medir a frequência do apito natural (app
  de afinador).

### Testes por placa

- (2026-10-03) **UNO R4:** teste da placa sem shield (`gpio`, `dac`) e com
  jumper D0↔D1 (`serial1`); teste conjunto novo (`auto --shield`; a
  gravação falhou por LIBUSB, resolver com RESET duplo ou religando);
  testar uma UNO R4 WiFi. Ver "A confirmar" no README do R4.
- (2026-10-03) **UNO R3:** medir o Vcc com multímetro; testar um clone
  com CH340 (registro manual no inventário); decidir se o R3 ganha um
  teste interativo próprio (hoje o menu do shield faz esse papel).
- (2026-09-29) **ESP32-S3 UNO:** LED RGB e a ordem das cores, script
  MicroPython na placa, tabela "A confirmar", texto que o ESPConnect
  mostra. Código de verificação nos dois formatos (C++ e MicroPython)
  para levantar as informações da placa e conferir a pinagem.
- (2026-09-28) **ESP32-C3 SuperMini** (depende do compilador): compilar
  `boards/esp32-c3-supermini/code/teste_esp32_c3_supermini`; na placa
  real, LED ativo em LOW ou HIGH, botão BOOT, Wi-Fi com e sem
  `WIFI_POWER_8_5dBm`, MicroPython; preencher "A confirmar".

### Shield 9 em 1 na ESP32-S3 UNO (modificação para 3,3V)

- (2026-10-01) O shield não é compatível sem modificação: leva 5V aos
  GPIOs pelos pull-ups e pelo potenciômetro. Ver "Medições do circuito" e
  "Modificação para 3,3V" no [README do shield](../shields/uno-shield-9in1/README.md).
  O AREF do shield não está ligado a nada (medido). Passos:
  1. Bloco 7 com o shield num UNO ligado: 5V; D2 solto/apertado; D4 e D6
     em repouso; A0 nos extremos; A1 coberto e com lanterna.
  2. Identificar o receptor IR (precisa aceitar 3,3V).
  3. Modificar um shield (preferir forma reversível): desligar o pino 5V,
     ligar `VCC` ao 3V3, etiquetar e dar uma linha própria no inventário.
     Conferir sem energia: 5V↔VCC aberto, 3V3↔VCC ≈0 Ω, 3V3↔GND sem
     curto. Perde o LM35.
  4. Repetir o bloco 7 na S3 (tudo ≤3,3V), testar IR, buzzer e
     MicroPython; atualizar a compatibilidade nos READMEs do shield e da
     S3 UNO.

### Conteúdo e infraestrutura

- (2026-10-04) **Fotos para o professor tirar** (pasta `imagens/` de cada
  item; os nomes já estão na seção "Fotos" de cada README):
  - UNO R3: `vista-de-cima.jpg`, `conector-usb.jpg` (USB-B e o chip da
    ponte: 16U2 ou CH340), `serigrafia.jpg` (verso). Uma do original e,
    quando houver, uma do clone CH340.
  - UNO R4: `vista-de-cima.jpg` (Minima; WiFi quando houver),
    `conector-usb.jpg`, `serigrafia.jpg` (verso).
  - ESP32-S3 UNO: `vista-de-cima.jpg`, `modulo.jpg` (gravação N16R8/N8R2
    na tampa), `serigrafia.jpg` (nomes `IOxx`).
  - ESP32-C3 SuperMini: `vista-de-cima.jpg`, `serigrafia.jpg` (números dos
    GPIOs), `conector-usb.jpg` (USB-C e botões).
  - ESP32-S3 N16R8 DevKit: `vista-de-cima.jpg`, `modulo.jpg`,
    `serigrafia.jpg` (modelo da placa: ajuda a definir o produto de
    referência). Diagrama de pinagem também pendente.
  - Shield 9 em 1: já tem `frente-verso.jpg` e `pinout-anotado.jpg`.
  Ao colocar a foto, troque o texto "Ainda sem foto" pela imagem, com
  texto alternativo descritivo.
- Definir o produto de referência do ESP32-S3 N16R8 DevKit.
- Documentar mais placas e shields conforme forem usados em aula.
- (2026-10-04) CI no GitHub Actions: rodar os testes de `scripts/` (inclui
  a checagem do STATE) e conferir se `INDEX.md` e `relatorio.html` foram
  regerados.
- (2026-10-04) Futuro, só se for o caso: dividir o STATE em arquivos por
  tópico ou por placa (`.ai/state/`), com o STATE como índice; e migrar o
  backlog para GitHub Issues se mais pessoas passarem a contribuir (custo:
  perde o arquivo único que os agentes leem offline).

### Fora deste repositório

- (2026-09-29) **lab-iot:** o passo 6 da seção 5.2 da especificação diz
  "segure o BOOT", mas a ESP32-S3 UNO não tem botão BOOT. A gravação é
  automática pelo CH340; só se falhar, usar o jumper IO0 → GND + RST (ver
  "Gravação" em `boards/esp32-s3-uno/README.md`).

## Decisões vigentes

- **Inventário separado do código** (2026-10-03): `inventario/` com um CSV
  por tipo de placa e um de shields; colunas `etiqueta_colada` e `dono`
  (professor / SENAI / a confirmar) e, no R3, `variante` (ATmega16U2 ou
  CH340). Etiquetas sequenciais (`R3-NN`, `R4M-NN`, `R4W-NN`, `S9-NN`),
  nunca um pedaço do ID (trechos do ID se repetem no mesmo lote). Chave:
  número de série USB (R3 16U2, R4 Minima = ID de 128 bits do RA4M1) ou
  MAC (ESP32). Placas do professor: ESP32-S3 UNO, UNO R4 Minima e R3
  (16U2 e CH340); do SENAI: R3 (2 ou 3 tipos); shields dos dois.
- **Relatório do inventário** (2026-10-03): `scripts/gerar_inventario.py`
  gera `inventario/relatorio.html` (arquivo único, abre offline). Regerar
  e commitar junto com o CSV a cada mudança.
- **Teste da placa × teste conjunto** (2026-10-03; substitui a regra
  anterior de "automático + interativo por placa"): o teste da placa
  (`boards/<slug>/code`) é automático, só serial + LED, placa sozinha; o
  teste conjunto placa + shield fica na pasta do shield, com menu guiado e
  opção `a` automática, um sketch por placa (`teste_shield_9em1_uno_r3`,
  `..._r4`; só o R4 liga a descarga do ADC). Todos em 115200 baud. Regras
  em `.ai/CONVENTIONS.md`, seção "Código de teste".
- **Código dedicado por placa** (2026-10-03): cada placa tem os próprios
  sketches, sem um sketch genérico com `#ifdef`.
- **Testes pela IDE para alunos** (2026-10-03): os sketches funcionam só
  com a IDE do Arduino e o Monitor Serial; terminam com um RESUMO legível
  antes do `FIM`; aceitam qualquer final de linha; só começam com o
  comando `c`. O `scripts/serial_placa.py` é ferramenta do professor
  (triagem, inventário).
- **Padrões do shield 9 em 1** (2026-10-03): LED D12 vermelho, D13 azul;
  buzzer ativo, liga em HIGH; RGB D9 vermelho, D10 azul, D11 verde. Os
  fabricantes divergem; vale o teste real (README do shield).
- **Tensão de operação obrigatória** (2026-09-24): seção e tag `5v`/`3v3`
  em todo item. Ver `.ai/CONVENTIONS.md`.
- **Diretiva do repositório** (2026-09-28; atualizada em 2026-10-04):
  público, para alunos e agentes de IA, com **três modos de acesso**:
  local, pelo GitHub (navegação e URL bruta) e pelo site no GitHub Pages
  (`https://professorjoaomiguel.github.io/lab-boards/`). `AGENTS.md` é a
  entrada; regras em `.ai/CONVENTIONS.md`, seção "Diretiva do repositório".
- **GitHub Pages** (2026-10-04, decisão do usuário): branch `main`, raiz,
  Jekyll padrão, **com** o inventário publicado. `_config.yml` publica a
  `.ai/` (`include`); o Jekyll do Pages não transforma `README.md` e
  `CONTRIBUTING.md` em página, e front matter neles aparece no GitHub,
  então `index.html` (a home) e `CONTRIBUTING.html` incluem o texto deles
  (`_includes/markdown-da-raiz.html` ajusta os links); layout próprio
  em `_layouts/default.html` (visual do site do professor). Todo push na
  `main` republica o site.
- **Licença e autoria** (2026-09-28): documentação CC BY-NC 4.0
  (`LICENSE`), código MIT (`LICENSE-CODE`), citação em `CITATION.cff`.
  Autor: Prof. Me. Joao Miguel Lac Roehe (@professorjoaomiguel), sem e-mail
  público; nome do projeto: "Lab Boards" (ambos iguais ao site do
  professor desde 2026-10-04). No `CITATION.cff`, given-names `Joao Miguel`,
  family-names `Roehe`.
- **Scripts documentados no próprio arquivo** (2026-09-28): Python com
  docstrings e `--help`; PowerShell com ajuda por comentários. Ver
  `.ai/CONVENTIONS.md`, seção "Scripts".
- **`.ai/` como fonte única para agentes** (2026-07-23): estrutura, template,
  tags e índice estão descritos no `.ai/CONVENTIONS.md`.
- **Internos fora do repositório público** (2026-10-04, decisão do
  usuário): `docs/` (histórico de design e planos, pesquisa) fica só na
  máquina do mantenedor, no `.gitignore` (continua no histórico do git).
  O `.claude/` fica, porque o `.claude/CLAUDE.md` importa o `AGENTS.md`
  para quem clona; a regra do handoff do `/remember` foi para o
  CONVENTIONS.

## Resolvido (últimos ~30 dias)

- 2026-10-04: testes movidos de `scripts/` para `tests/` (sugestão do
  usuário); `scripts/` fica só com as ferramentas.
- 2026-10-04: `scripts/verificar_site.py` (varredura do site publicado:
  404, âncora inexistente, `.md` cru), com testes sem internet.
- 2026-10-04: GitHub Pages no ar em
  `https://professorjoaomiguel.github.io/lab-boards/`, com o relatório do
  inventário em `inventario/relatorio.html`; descrição, homepage e tópicos
  do repositório no GitHub. Conferido por varredura: 19 páginas, 457
  links e âncoras, sem link quebrado.
- 2026-10-04: READMEs dos itens no padrão aluno primeiro: "Resumo rápido"
  no topo, "Como programar" (agora também no UNO R3 e no R4) e conteúdo do
  professor (terminal, inventário, resultados) na seção final "Para o
  professor / histórico de testes". Template e CONVENTIONS atualizados.
- 2026-10-04: README voltado ao aluno (tabela das placas, tensão,
  glossário, "Que placa é essa?", "Usado em", agente de IA); manutenção
  movida para [`CONTRIBUTING.md`](../CONTRIBUTING.md); guia
  [`IDENTIFICAR.md`](../IDENTIFICAR.md) (sem fotos); AGENTS aponta para
  glossário, IDENTIFICAR e marca `inventario/` e `scripts/` como
  ferramentas do professor.
- 2026-10-04: link "← Site do professor" no README, no INDEX e no relatório
  do inventário (pelos geradores, com teste); contatos do README apontam
  para o site (Instagram e Facebook removidos).
- 2026-10-04: autor "Prof. Me. Joao Miguel Lac Roehe" e nome "Lab Boards"
  em todos os arquivos (READMEs, front matter, sketches, scripts, licença).
- 2026-10-04: `docs/superpowers/` saiu do repositório (`git rm --cached`);
  regra do handoff do `/remember` movida para o CONVENTIONS.
- 2026-10-04: [`GLOSSARIO.md`](../GLOSSARIO.md) com ~50 termos, regra no
  CONVENTIONS e `tests/test_glossario.py` (ordem, âncoras e links de
  todos os `.md`); 1ª ocorrência dos termos linkada nos 6 READMEs.
- 2026-10-04: `scripts/verificar_state.py` + teste; handoff do
  `/remember` passa a apontar para o STATE (`.claude/CLAUDE.md`).
- 2026-10-04: faxina do STATE: separado em Em aberto / Decisões vigentes /
  Resolvido, com regras de manutenção no topo.
- 2026-10-03: donos registrados: R3-01 SENAI, R3-02 professor, R4M-01
  professor, S3 `1e:20` professor.
- 2026-10-03: relatório HTML do inventário (`gerar_inventario.py`).
- 2026-10-03: **R3-01 com D3 preso em LOW**: o defeito é da placa (não do
  shield), confirmado pelo teste sem shield. Anotado no inventário.
- 2026-10-03: R3-02 registrada; teste da placa sem shield com os 16 pinos
  GPIO OK. Na R3-01, o mesmo teste falhou só no D3.
- 2026-10-03: teste automático dedicado do UNO R3
  (`teste_uno_r3_automatico`), validado.
- 2026-10-03: ADC do UNO R3: o LM35 lido logo depois de outro canal sai
  alto e acerta em ~100 ms; depois de `analogReference(INTERNAL)`, ~0,5 s.
  Documentado no README do R3.
- 2026-10-03: ADC do UNO R4 lê errado sensores que não absorvem corrente:
  corrigido com `R_ADC0->ADDISCR = 0x0F` (README do R4). Pendências na
  seção "Investigações".
- 2026-10-03: buzzer do shield é ativo e liga em HIGH (antes apitava sem
  parar no menu); conferido em dois shields.
- 2026-10-03: menus do shield aceitam qualquer final de linha (pedem `c`);
  teste 3 com LED seguindo o botão na hora; validado no R3 e no R4.
- 2026-10-03: sketches do shield rejeitam a 1ª leitura do DHT11 (zerada).
- 2026-10-03: sketches interativos das placas removidos (ver a decisão
  "Teste da placa × teste conjunto").
- 2026-10-01: circuito do shield medido (VCC = 5V do header; pull-ups de
  10k em D2/D3/D6 e de 3,3k em D4; AREF solto); incompatível com a S3 UNO
  sem modificação.
- 2026-09-29: ESP32-S3 UNO: N16R8, PSRAM `SPIRAM_OCT` e gravação sem
  jumper confirmadas.
