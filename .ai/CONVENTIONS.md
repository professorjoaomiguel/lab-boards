# Convenções para agentes de IA

Guia de referência para qualquer agente de IA (Claude, Antigravity,
Copilot, etc.) que for adicionar ou editar conteúdo neste repositório.

## Diretiva do repositório

Este repositório é **público** e tem **dois públicos**, com o mesmo peso:

- **Alunos**, que leem a documentação para usar as placas e shields em aula.
- **Agentes de IA**, que consultam a documentação para responder perguntas
  ou gerar código para essas placas.

E **dois modos de acesso**, que precisam funcionar igualmente bem:

- **Local**, com o repositório clonado (arquivos lidos direto do disco).
- **Direto pelo GitHub**, sem clonar: navegando em
  `https://github.com/professorjoaomiguel/lab-boards` ou lendo o arquivo
  bruto em
  `https://raw.githubusercontent.com/professorjoaomiguel/lab-boards/main/<caminho>`.

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
   `autor: "Prof. Joao Miguel Roehe (@professorjoaomiguel)"`, e o README
   termina com o rodapé de autoria e licença do template. Arquivos de
   código começam com um comentário de cabeçalho com o autor e
   `SPDX-License-Identifier: MIT`. Documentação: CC BY-NC 4.0 (`LICENSE`).
   Código: MIT (`LICENSE-CODE`). Citação: `CITATION.cff`. Nunca publique o
   e-mail do autor; use apenas o identificador `@professorjoaomiguel`.

## Estrutura do repositório

- `boards/<slug>/` — placas de desenvolvimento (ESP32, Arduino, etc.)
- `shields/<slug>/` — shields e módulos que acoplam nas placas (teclado,
  display, etc.)
- `templates/item-README.md` — template a ser copiado para criar um item novo.
- `scripts/gerar_indice.py` — gera `INDEX.md` a partir do front matter de
  todos os itens.
- `docs/superpowers/specs/` e `docs/superpowers/plans/` — histórico de
  design e planos de implementação deste repositório.

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
3. Preencha as seções do template (visão geral, tensão de operação,
   fotos, diagrama esquemático, componentes, funcionalidades, referências).
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

## Testes

`scripts/gerar_indice.py` tem testes em `scripts/test_gerar_indice.py`,
rodados com `python -m unittest scripts/test_gerar_indice.py -v`. Qualquer
mudança no script deve manter esses testes passando e seguir TDD (teste
antes da implementação).
