---
---
[← Voltar ao README](README.md)

# Como manter o Lab Boards

Este arquivo é para quem **mantém** o repositório (o professor e agentes de
IA que editam). Para consultar as placas, comece pelo [README](README.md).

O guia completo de convenções (front matter, slugs, tensão de operação,
código de teste, scripts, glossário) é o
[`.ai/CONVENTIONS.md`](.ai/CONVENTIONS.md). O que já foi feito e o que falta
está no [`.ai/STATE.md`](.ai/STATE.md).

## Como adicionar um item novo

1. Copie [`templates/item-README.md`](templates/item-README.md) para
   `boards/<slug-do-item>/README.md` ou `shields/<slug-do-item>/README.md`.
2. Preencha o front matter (`titulo`, `tipo`, `tags`, com a tag `5v` ou
   `3v3`) e as seções do template, incluindo a **Tensão de operação**.
3. Crie as subpastas `imagens/` e `code/` dentro da pasta do item.
4. Rode `python scripts/gerar_indice.py` para atualizar o `INDEX.md`.
5. Inclua o item na tabela "Procurando uma placa?" do [README](README.md) e,
   se der para reconhecê-lo a olho, no [`IDENTIFICAR.md`](IDENTIFICAR.md).
6. Termos técnicos novos entram no [`GLOSSARIO.md`](GLOSSARIO.md).
7. Atualize o [`.ai/STATE.md`](.ai/STATE.md) com o item novo.
8. Faça commit do item novo junto com o `INDEX.md` atualizado.

## Ferramentas do professor

- `scripts/serial_placa.py`: lista as placas ligadas, roda os sketches de
  teste pela serial e registra as unidades em [`inventario/`](inventario/README.md)
  (`--help`).
- `scripts/gerar_inventario.py`: gera o `inventario/relatorio.html` a partir
  dos CSVs (rodar sempre que um CSV mudar).
- `scripts/verificar_state.py`: confere links, a seção "Resolvido" e o
  tamanho do `.ai/STATE.md`.

## Testes

Antes de cada commit:

```
python scripts/verificar_state.py
python -m pytest -q scripts
```

Os testes conferem os geradores, o STATE, o glossário e todos os links
relativos dos arquivos `.md`.
