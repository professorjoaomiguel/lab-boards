# Convenções para agentes de IA

Guia de referência para qualquer agente de IA (Claude, Antigravity,
Copilot, etc.) que for adicionar ou editar conteúdo neste repositório.

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
3. Preencha as seções do template (visão geral, fotos, diagrama
   esquemático, componentes, funcionalidades, referências).
4. Crie as subpastas `imagens/` (fotos e diagramas) e `code/` (código de
   teste/validação, quando existir) dentro da pasta do item.
5. Rode `python scripts/gerar_indice.py` para atualizar o `INDEX.md`. A
   geração é sempre manual — não há hook de git nem CI fazendo isso
   automaticamente.
6. Atualize `.ai/STATE.md` com o item novo.
7. Faça commit do item novo junto com o `INDEX.md` atualizado.

## Testes

`scripts/gerar_indice.py` tem testes em `scripts/test_gerar_indice.py`,
rodados com `python -m unittest scripts/test_gerar_indice.py -v`. Qualquer
mudança no script deve manter esses testes passando e seguir TDD (teste
antes da implementação).
