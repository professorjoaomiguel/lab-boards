# Design: Repositório de Documentação de Placas e Shields

**Data:** 2026-07-23
**Status:** Aprovado

## Objetivo

Repositório para documentar placas de desenvolvimento (ESP32, Arduino, etc.) e
shields (teclado, display, etc.) usados em aulas: fotos, diagramas
esquemáticos, componentes e funcionalidades. No futuro, cada item também
recebe código de teste/validação de periféricos. Função principal:
documentação e referência — não é, em si, um projeto de firmware.

## Público

Uso principal: o professor e seus alunos, como material de apoio durante as
aulas práticas. Documentação escrita em português.

## Formato

Markdown puro, navegado diretamente no GitHub — sem gerador de site estático
(sem MkDocs/Docusaurus). Prioriza simplicidade de manutenção sobre navegação
polida.

Todo o conteúdo (documentação de cada placa/shield, e futuramente o código de
teste) será construído com o auxílio de agentes de IA (Claude, Antigravity,
Copilot). Isso reforça a necessidade de um template padronizado e previsível
por item, mesmo sem um guia dedicado para agentes por enquanto (ver
"Fora de escopo").

## Estrutura de pastas

```
/
├── README.md              # visão geral do repo, como navegar, como adicionar um item
├── INDEX.md                # gerado automaticamente — tabela de todos os itens por tag
├── boards/
│   └── <slug-da-placa>/
│       ├── README.md
│       ├── imagens/
│       └── code/            # vazio até que testes de periféricos sejam adicionados
├── shields/
│   └── <slug-do-shield>/
│       ├── README.md
│       ├── imagens/
│       └── code/
└── scripts/
    └── gerar_indice.py      # lê o front matter de cada item e regenera INDEX.md
```

- `boards/` e `shields/` são separados por tipo (não uma pasta única `devices/`).
- Cada item vive em sua própria pasta, nomeada com um slug (ex:
  `esp32-devkit-v1`, `teclado-matricial-4x4`).
- O código de teste/validação de cada item fica dentro da própria pasta do
  item (`code/`), não em uma árvore separada no topo do repo — mantém
  documentação e código do mesmo item juntos.

## Template de cada item

Cada `README.md` de item começa com front matter YAML e segue seções fixas:

```markdown
---
titulo: "ESP32 DevKit V1"
tipo: placa            # placa | shield
tags: [esp32, wifi, bluetooth, devkit]
---

# ESP32 DevKit V1

## Visão geral
Breve descrição: o que é, para que serve, contexto de uso em aula.

## Fotos
![foto](imagens/foto-01.jpg)

## Diagrama esquemático / Pinout
![esquemático](imagens/esquematico.png)

## Componentes principais
- ...

## Funcionalidades / Periféricos
- ...

## Código de teste e validação
(preenchido futuramente — ver pasta `code/`)

## Referências
- Datasheet: link
- Loja/fabricante: link
```

O front matter (`titulo`, `tipo`, `tags`) é a única parte lida por ferramentas
(o script de índice); o resto do arquivo é texto livre seguindo as seções
acima.

## Sistema de tags e índice

- Tags ficam no front matter de cada item (`tags: [esp32, wifi, i2c, ...]`).
- `scripts/gerar_indice.py` varre `boards/*/README.md` e `shields/*/README.md`,
  lê o front matter de cada um, e regenera `INDEX.md` contendo:
  - uma tabela geral (nome, tipo, tags, link para a pasta do item);
  - uma seção por tag, listando os itens que a possuem.
- O script não depende de bibliotecas externas (parser de front matter
  simples, sem exigir `pip install pyyaml`) e roda com
  `python scripts/gerar_indice.py`.
- A geração é manual: quem adiciona ou edita um item (pessoa ou agente de IA)
  roda o script depois. Não há hook de git nem CI automatizando isso por
  enquanto.

## README raiz

Explica: o propósito do repositório, como navegar (`INDEX.md`, pastas
`boards/`/`shields/`), e o passo a passo para adicionar um item novo (copiar o
template, preencher as seções e o front matter, rodar o script de índice).

## Fora de escopo (por enquanto)

- Gerador de site estático (MkDocs/Docusaurus) — descartado em favor de
  Markdown puro no GitHub.
- Automação da geração do índice (git hook/CI) — geração manual por enquanto.
- Código de teste/validação de periféricos em si — a estrutura (`code/`)
  existe desde já, mas o conteúdo é preenchido em uma etapa futura.

## Primeiro item de referência

A placa ESP32-S3 N16R8
(https://makerselectronics.com/product/esp32-s3-n16r8-development-board-2) foi
citada como exemplo de placa real a documentar quando a implementação
começar a popular `boards/`.

## Adendo (2026-07-23): pasta `.ai/` como SSoT para agentes de IA

Depois da implementação inicial (Tasks 1-3), o usuário decidiu que o
repositório terá uma pasta `.ai/` como fonte única de verdade (SSoT) para
agentes de IA (Claude, Antigravity, Copilot) que forem construir o conteúdo
do repositório junto com o usuário. Isso substitui a decisão original de
"nenhum guia dedicado para agentes por enquanto".

```
.ai/
├── README.md         # ponto de entrada: o que é essa pasta, aponta pros outros dois arquivos
├── CONVENTIONS.md      # guia detalhado pra agentes: estrutura de pastas, regras do template
│                        # de item, sistema de tags, quando/como rodar scripts/gerar_indice.py
└── STATE.md            # log vivo: itens já documentados, decisões tomadas, próximos passos
```

- `.ai/CONVENTIONS.md` é a referência canônica e detalhada para agentes —
  cobre o mesmo terreno que a seção "Como adicionar um item novo" do README
  raiz, mas com mais profundidade (formato exato do front matter, convenção
  de slugs, quando rodar o script de índice). O README raiz continua com a
  versão curta para humanos e passa a apontar para `.ai/CONVENTIONS.md`
  como a referência completa, evitando duplicar e divergir o mesmo
  conteúdo em dois lugares.
- `.ai/STATE.md` é atualizado conforme o repositório evolui: lista dos
  itens já documentados (com status), decisões arquiteturais tomadas (com
  link para os specs/plans relevantes em `docs/superpowers/`), e próximos
  passos pendentes — para qualquer agente retomar o contexto do zero.
- `.ai/README.md` é só o ponto de entrada, explicando o propósito da pasta
  e apontando para os outros dois arquivos.
