[← Site do professor](https://professorjoaomiguel.github.io/)

# Lab Boards

Documentação de referência das placas de desenvolvimento e shields usados em
aula: ESP32, Arduino e afins, e shields como teclados, displays, etc. Cada
item traz fotos, diagrama esquemático, lista de componentes e
funcionalidades e, quando disponível, código de teste/validação dos
periféricos.

## Para quem é este repositório

Este repositório é público e serve a dois públicos:

- **Alunos:** leia pelo GitHub ou clone o repositório e comece pelo
  [`INDEX.md`](INDEX.md).
- **Agentes de IA** (Claude, Copilot, Gemini, etc.): o ponto de entrada é o
  [`AGENTS.md`](AGENTS.md), que explica como achar a informação, com o
  repositório clonado ou direto pelo GitHub. Você pode passar ao seu agente
  o link
  `https://raw.githubusercontent.com/professorjoaomiguel/lab-boards/main/AGENTS.md`.

## Como navegar

- [`INDEX.md`](INDEX.md) — lista todos os itens, com uma tabela geral e uma
  seção por tag. **Gerado automaticamente, não editar à mão.**
- [`boards/`](boards/) — placas de desenvolvimento (ESP32, Arduino, etc.)
- [`shields/`](shields/) — shields e módulos que acoplam nas placas (teclado,
  display, etc.)
- [`GLOSSARIO.md`](GLOSSARIO.md) — o que significa cada termo técnico (ADC,
  pull-up, PSRAM, CH340...), em ordem alfabética.

## ⚠️ Atenção à tensão (5V × 3,3V)

Antes de encaixar um shield em uma placa, confira a seção **Tensão de
operação** dos dois. Placas como o Arduino UNO trabalham em 5V, e placas
como o ESP32 trabalham em 3,3V. Um shield de 5V sobre uma placa de 3,3V
pode queimar as portas do microcontrolador. No `INDEX.md`, as tags `5v` e
`3v3` agrupam os itens por tensão.

## Como adicionar um item novo

Passo a passo resumido:

1. Copie [`templates/item-README.md`](templates/item-README.md) para
   `boards/<slug-do-item>/README.md` ou `shields/<slug-do-item>/README.md`.
2. Preencha o front matter (`titulo`, `tipo`, `tags`, com a tag `5v` ou
   `3v3`) e as seções do template, incluindo a **Tensão de operação**.
3. Crie as subpastas `imagens/` e `code/` dentro da pasta do item.
4. Rode `python scripts/gerar_indice.py` para atualizar o `INDEX.md`.
5. Atualize [`.ai/STATE.md`](.ai/STATE.md) com o item novo.
6. Faça commit do item novo junto com o `INDEX.md` atualizado.

Para o guia completo de convenções (formato exato do front matter,
convenção de slugs, testes do script de índice), veja
[`.ai/CONVENTIONS.md`](.ai/CONVENTIONS.md) — é a referência usada por
agentes de IA que constroem este repositório.

## Autor, licença e como citar

**Autor:** Prof. Me. Joao Miguel Lac Roehe
([@professorjoaomiguel](https://github.com/professorjoaomiguel)). Contatos e
outros materiais: [site do professor](https://professorjoaomiguel.github.io/).

**Licença:**

- **Documentação** (textos, tabelas, imagens próprias): licença
  [Creative Commons Atribuição-NãoComercial 4.0 Internacional (CC BY-NC 4.0)](https://creativecommons.org/licenses/by-nc/4.0/deed.pt-br).
  Você pode copiar, adaptar e compartilhar, desde que cite o autor e não
  use para fins comerciais. Texto completo em [`LICENSE`](LICENSE).
- **Código** (sketches em `code/` e scripts em `scripts/`): licença MIT. Texto
  completo em [`LICENSE-CODE`](LICENSE-CODE).

Datasheets e materiais de fabricantes são apenas referenciados por link e
continuam sob a licença de seus próprios autores.

**Como citar:**

> ROEHE, Joao Miguel. *Lab Boards: documentação de placas de
> desenvolvimento e shields para aula*. GitHub, 2026. Disponível em:
> https://github.com/professorjoaomiguel/lab-boards. Licença CC BY-NC 4.0.

Na página do repositório no GitHub, o botão **"Cite this repository"** gera
a citação em APA e BibTeX a partir do arquivo [`CITATION.cff`](CITATION.cff).
