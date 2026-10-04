# Convenções para agentes de IA

Guia de referência para qualquer agente de IA (Claude, Antigravity,
Copilot, etc.) que for adicionar ou editar conteúdo neste repositório.

## Diretiva do repositório

Este repositório é **público** e tem **dois públicos**, com o mesmo peso:

- **Alunos**, que leem a documentação para usar as placas e shields em aula.
- **Agentes de IA**, que consultam a documentação para responder perguntas
  ou gerar código para essas placas.

E **três modos de acesso**, que precisam funcionar igualmente bem:

- **Local**, com o repositório clonado (arquivos lidos direto do disco).
- **Direto pelo GitHub**, sem clonar: navegando em
  `https://github.com/professorjoaomiguel/lab-boards` ou lendo o arquivo
  bruto em
  `https://raw.githubusercontent.com/professorjoaomiguel/lab-boards/main/<caminho>`.
- **Site (GitHub Pages)**, em `https://professorjoaomiguel.github.io/lab-boards/`:
  o Jekyll padrão do GitHub transforma cada `.md` em página (layout em
  `_layouts/default.html`, configuração em `_config.yml`). Por isso, **nenhum
  `.md` pode conter chave dupla nem chave seguida de porcentagem** (as
  marcas do Liquid, a linguagem de modelos do Jekyll); o teste
  `scripts/test_glossario.py` confere. Os links relativos para `.md`
  continuam funcionando, e a pasta `.ai/` é publicada pelo `include` do
  `_config.yml`.

Todo conteúdo novo ou editado deve respeitar estas regras:

1. **Markdown puro e links relativos.** Nada de conteúdo que só aparece
   depois de rodar uma ferramenta, nem links absolutos para o próprio
   repositório. O link relativo funciona no disco e no GitHub.
2. **README do item autossuficiente.** Quem abrir só o `README.md` de um
   item (ex: pela URL bruta) precisa achar ali tudo o que importa: tensão,
   pinagem, componentes, compatibilidade e como testar.
3. **Dados em texto, não só em imagem.** Pinagem, ligações e valores vão em
   tabelas. A foto ou o diagrama complementa, mas não substitui, porque um
   agente pode não enxergar a imagem. Toda imagem tem texto alternativo
   descritivo.
4. **Separar o que foi confirmado do que é suposição.** Deixe claro o que
   veio do fabricante, o que foi testado na placa real e o que ainda está
   "a confirmar". Um agente não deve tratar um indício como fato.
5. **Front matter e `INDEX.md` sempre em dia.** São a porta de entrada
   estruturada para agentes: `INDEX.md` lista tudo, e o front matter de cada
   item traz `titulo`, `tipo` e `tags`.
6. **Linguagem didática em português (PT-BR).** O texto é escrito para o
   aluno entender; termos técnicos são explicados na primeira vez em que
   aparecem.
7. **Público significa sem segredo.** Nunca commitar senhas de Wi-Fi,
   tokens, chaves de API ou dados pessoais de alunos. Em sketches, use
   placeholders (ex: `"SUA_REDE"`). Arquivos de terceiros (datasheets,
   imagens de fabricante) entram como link, a menos que a licença permita
   redistribuir.

O ponto de entrada para agentes que **consultam** o repositório é o
[`AGENTS.md`](../AGENTS.md) da raiz. Este arquivo (`.ai/CONVENTIONS.md`) é
para agentes que **constroem** o repositório.

8. **Autoria e licença em todo item.** O front matter leva
   `autor: "Prof. Me. Joao Miguel Lac Roehe (@professorjoaomiguel)"`, e o README
   termina com o rodapé de autoria e licença do template. Arquivos de
   código começam com um comentário de cabeçalho com o autor e
   `SPDX-License-Identifier: MIT`. Documentação: CC BY-NC 4.0 (`LICENSE`).
   Código: MIT (`LICENSE-CODE`). Citação: `CITATION.cff`. Nunca publique o
   e-mail do autor; use apenas o identificador `@professorjoaomiguel`.

## Estrutura do repositório

- `boards/<slug>/` — placas de desenvolvimento (ESP32, Arduino, etc.)
- `shields/<slug>/` — shields e módulos que acoplam nas placas (teclado,
  display, etc.)
- `inventario/` — inventário das unidades físicas (placas e shields): quais
  existem, etiqueta, dono (professor ou SENAI) e o que se sabe de cada uma.
  Fica separado do código de teste. Ver `inventario/README.md`.
- `GLOSSARIO.md` — termos técnicos em ordem alfabética (ver "Glossário").
- `IDENTIFICAR.md` — guia "Que placa é essa?" pelas características
  visíveis; todo item novo que dê para reconhecer a olho entra nele.
- `CONTRIBUTING.md` — como manter o repositório (item novo, scripts,
  testes). O `README.md` da raiz é voltado ao aluno.
- `templates/item-README.md` — template a ser copiado para criar um item novo.
- `scripts/gerar_indice.py` — gera `INDEX.md` a partir do front matter de
  todos os itens.
- `scripts/gerar_inventario.py` — gera `inventario/relatorio.html` a partir
  dos CSVs do inventário (rodar sempre que um CSV mudar).
- `scripts/verificar_state.py` — confere links, a seção "Resolvido" e o
  tamanho do `.ai/STATE.md`.
- `scripts/serial_placa.py` — lista as placas ligadas, roda os sketches de
  teste pela serial e registra placas em `inventario/` (`--help`).
- `docs/` — material local do mantenedor (histórico de design, pesquisa).
  **Não é publicado:** está no `.gitignore`.

## Slugs

O nome da pasta de cada item é um slug: minúsculo, com hífens no lugar de
espaços, sem acentos (ex: `esp32-s3-n16r8`, `teclado-matricial-4x4`).

## Adicionar um item novo

1. Copie `templates/item-README.md` para
   `boards/<slug>/README.md` ou `shields/<slug>/README.md`.
2. Preencha o front matter:
   - `titulo`: nome de exibição do item, entre aspas.
   - `tipo`: `placa` ou `shield`.
   - `tags`: lista entre colchetes, minúsculas, sem espaços dentro de cada
     tag (ex: `[esp32, wifi, i2c]`). Não deixe comentários na mesma linha
     de um valor de front matter que não seja `tags` — o parser do script
     de índice remove comentários (`# ...`) de qualquer valor, mas evite
     depender disso além do que o template já usa.
   - Toda placa e todo shield leva a tag de tensão lógica: `5v` ou `3v3`.
     Assim o `INDEX.md` agrupa os itens por tensão.
3. Preencha as seções do template, nesta ordem: **Resumo rápido** (tensão,
   placa na IDE, driver USB, LED embutido, botões, cuidado nº 1; só dados
   confirmados, o resto "a confirmar"), visão geral, tensão de operação,
   fotos (diga quais faltam), diagrama esquemático, componentes,
   funcionalidades, **Como programar**, código de teste, referências e, no
   fim, **Para o professor / histórico de testes** (terminal, inventário,
   resultados nas placas reais, investigações longas). O README é lido
   primeiro pelo aluno: o conteúdo do professor fica no fim.
4. Crie as subpastas `imagens/` (fotos e diagramas) e `code/` (código de
   teste/validação, quando existir) dentro da pasta do item.
5. Rode `python scripts/gerar_indice.py` para atualizar o `INDEX.md`. A
   geração é sempre manual — não há hook de git nem CI fazendo isso
   automaticamente.
6. Atualize `.ai/STATE.md` com o item novo.
7. Faça commit do item novo junto com o `INDEX.md` atualizado.

## Tensão de operação (obrigatório)

A seção **Tensão de operação** nunca pode ficar vazia. Misturar placa e
shield de tensões diferentes é a forma mais comum de queimar um
microcontrolador em aula:

- Um shield de 5V sobre uma placa de 3,3V (ex: ESP32) coloca 5V nos
  pinos de entrada do microcontrolador, que só suportam até 3,6V. Isso pode
  queimar a porta ou o chip inteiro.
- Um shield de 3,3V sobre uma placa de 5V pode receber 5V nas saídas da
  placa e queimar os sensores do shield.

Em **Referências**, toda placa leva o link para o datasheet de cada
microcontrolador que ela tem (principal e auxiliares, como a ponte USB).

Sempre informe a tensão lógica, se as entradas toleram 5V e a corrente
máxima por pino, e diga explicitamente com quais placas/shields do
repositório o item é compatível. Quando um dado não puder ser confirmado
pela serigrafia ou pelo datasheet, escreva que ele precisa ser medido
(multímetro) em vez de supor.

## Glossário

O [`GLOSSARIO.md`](../GLOSSARIO.md) explica os termos técnicos, em ordem
alfabética, um título `###` por termo (o título vira a âncora do link).

- Ao usar num README um termo técnico que o glossário ainda não tem,
  acrescente-o na letra certa: uma definição geral de 1 a 3 frases e,
  quando houver, uma linha **Neste repositório:** com link para a seção
  do README que tem o dado específico.
- O glossário traz o **conceito**; valores de uma placa (pinos, tensões
  medidas) ficam no README dela. Não copie dados de README para cá.
- Em cada README, a **primeira** ocorrência de um termo do glossário leva
  o link (`[ADC](../../GLOSSARIO.md#adc)`); as seguintes, não.
- `scripts/test_glossario.py` confere a ordem alfabética, as âncoras
  repetidas e os links de todos os `.md` (inclusive os que apontam para o
  glossário).

## Código de teste (sketches)

Dois tipos de teste, cada um no seu lugar:

- **Teste da placa** (`boards/<slug>/code/`): **automático**, só pela
  serial e com o LED da própria placa, com a **placa sozinha** (sem shield).
  Antes de acionar pinos, procura resistores externos neles e, se achar,
  não aciona nada e avisa. Não pede interação do usuário além do comando
  para começar.
- **Teste conjunto placa + shield** (`shields/<slug>/code/`): informa sobre
  as duas (a placa embaixo e o shield). Sempre envolve alguém apertando,
  girando, olhando e ouvindo (testes guiados), e pode ter uma opção
  automática com resumo.

Regras comuns a todos os sketches de teste:

- **Um sketch dedicado por placa** (sem `#ifdef` para várias placas), com
  uma trava `#error` que impede compilar para a placa errada.
- **115200 baud** e qualquer opção de final de linha do Monitor Serial
  (mensagem termina em `\r`/`\n` ou após 200 ms sem caracteres).
- Só começa com um comando (`c`, ou a opção do menu); abrir a porta mostra
  um aviso. Enquanto espera, o LED do D13 pisca duas vezes rápidas a cada
  2 s ("firmware de teste gravado").
- Saída para o script (`INICIO;`, `RESULTADO;`, `FIM;`) e um **RESUMO**
  legível no fim, um teste por linha.
- **Código bem documentado:** cabeçalho com o que faz, cada teste, como
  usar e cuidados de segurança; comentários explicando o porquê das
  decisões não óbvias, de preferência com o que foi medido na placa real.

## Scripts (Python e PowerShell)

Todo script do repositório é documentado **no próprio arquivo**, junto com
o código, de forma que a ajuda apareça pela linha de comando. Não criar um
README separado só para explicar um script. Tudo em português (PT-BR).

Todo script deve ter:

- **Cabeçalho de autoria e licença** nas primeiras linhas: autor e
  `SPDX-License-Identifier: MIT`.
- **Ajuda completa** no formato padrão da linguagem (abaixo), com: o que o
  script faz, como usar, cada parâmetro e pelo menos um exemplo.
- **Ajuda para cada função**, com os parâmetros, o retorno e os erros que
  ela pode gerar.
- **Comentários que explicam o porquê** das decisões que não são óbvias.
  Não comente o que o código já diz sozinho.

### Python

- O **docstring do módulo** (o texto entre `"""` no topo) é a ajuda
  principal, com as seções O QUE FAZ, COMO USAR e, se houver, TESTES.
- Linha de comando com `argparse`, usando o docstring do módulo como
  `description`, para `python scripts/<script>.py --help` mostrar a ajuda.
  A função `main(argv=None)` recebe os argumentos, para poder ser testada.
- Cada função tem docstring no formato Google (`Args:`, `Returns:`,
  `Raises:`).
- Referência: `scripts/gerar_indice.py`.

### PowerShell

- **Ajuda baseada em comentários** (comment-based help) no topo do
  arquivo, para `Get-Help .\scripts\<script>.ps1 -Full` mostrar a ajuda:

  ```powershell
  # Autor: Prof. Me. Joao Miguel Lac Roehe (@professorjoaomiguel)
  # SPDX-License-Identifier: MIT
  <#
  .SYNOPSIS
      Uma linha dizendo o que o script faz.
  .DESCRIPTION
      Explicação completa: o que faz, quando usar, o que é alterado.
  .PARAMETER Porta
      O que o parâmetro significa e qual o valor padrão.
  .EXAMPLE
      .\scripts\exemplo.ps1 -Porta COM3
      O que acontece ao rodar este exemplo.
  .NOTES
      Requisitos (ex: arduino-cli no PATH) e observações.
  #>
  [CmdletBinding()]
  param(
      [Parameter(Mandatory)]
      [string]$Porta
  )
  ```

- Um bloco `.PARAMETER` para cada parâmetro do `param()`, e pelo menos um
  `.EXAMPLE`.
- Parâmetros tipados no `param()`, com `[CmdletBinding()]`.
- Funções internas também têm ajuda baseada em comentários (pelo menos
  `.SYNOPSIS` e `.PARAMETER`).

## Testes

`scripts/gerar_indice.py` tem testes em `scripts/test_gerar_indice.py`,
rodados com `python -m unittest scripts/test_gerar_indice.py -v`.
`scripts/gerar_inventario.py` tem testes em `scripts/test_gerar_inventario.py`.
`scripts/serial_placa.py` (testes de placa pela serial e inventário) tem
testes em `scripts/test_serial_placa.py`, que não precisam de placa ligada.
`scripts/verificar_state.py` confere o `.ai/STATE.md` (links e títulos de
destino, idade da seção "Resolvido", limite de linhas); o teste
`scripts/test_verificar_state.py` roda essa checagem no STATE real, então
um link quebrado no STATE faz a suíte falhar. Qualquer
mudança no script deve manter esses testes passando e seguir TDD (teste
antes da implementação).

## Handoff do `/remember`

O plugin `remember` do Claude Code guarda um *handoff* em
`.remember/remember.md` (local, fora do git), sobrescrito por inteiro a
cada `/remember`. O backlog versionado deste repositório é o
`.ai/STATE.md`, então a regra de "não perder item em aberto da nota
anterior" é cumprida assim:

1. Antes de escrever o handoff, leia a nota anterior inteira. Todo item em
   aberto dela que **não** esteja no `.ai/STATE.md` entra no STATE
   primeiro (com data), em commit próprio.
2. O handoff guarda só o que é da sessão: onde o trabalho parou, o que
   ficou pela metade, o commit atual e se houve push.
3. O "Next" lista no máximo os 3 próximos passos imediatos e termina com
   "Backlog completo: `.ai/STATE.md`". Não copie o resto do backlog.
4. O "Context" guarda só dicas operacionais que ainda não estão nos
   READMEs ou neste arquivo.
