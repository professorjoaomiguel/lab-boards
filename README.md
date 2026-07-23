# Lab Dev Boards

Documentação de referência das placas de desenvolvimento e shields usados em
aula: ESP32, Arduino e afins, e shields como teclados, displays, etc. Cada
item traz fotos, diagrama esquemático, lista de componentes e
funcionalidades e, quando disponível, código de teste/validação dos
periféricos.

## Como navegar

- [`INDEX.md`](INDEX.md) — lista todos os itens, com uma tabela geral e uma
  seção por tag. **Gerado automaticamente, não editar à mão.**
- [`boards/`](boards/) — placas de desenvolvimento (ESP32, Arduino, etc.)
- [`shields/`](shields/) — shields e módulos que acoplam nas placas (teclado,
  display, etc.)

## Como adicionar um item novo

Passo a passo resumido:

1. Copie [`templates/item-README.md`](templates/item-README.md) para
   `boards/<slug-do-item>/README.md` ou `shields/<slug-do-item>/README.md`.
2. Preencha o front matter (`titulo`, `tipo`, `tags`) e as seções do
   template.
3. Crie as subpastas `imagens/` e `code/` dentro da pasta do item.
4. Rode `python scripts/gerar_indice.py` para atualizar o `INDEX.md`.
5. Atualize [`.ai/STATE.md`](.ai/STATE.md) com o item novo.
6. Faça commit do item novo junto com o `INDEX.md` atualizado.

Para o guia completo de convenções (formato exato do front matter,
convenção de slugs, testes do script de índice), veja
[`.ai/CONVENTIONS.md`](.ai/CONVENTIONS.md) — é a referência usada por
agentes de IA que constroem este repositório.
