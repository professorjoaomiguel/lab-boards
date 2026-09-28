# Instruções para agentes de IA

Este repositório documenta as placas de desenvolvimento e shields usados em
aula (Arduino, ESP32 e afins). Ele é público e foi feito para ser lido
tanto por alunos quanto por agentes de IA, com o repositório clonado ou
direto pelo GitHub.

## Como acessar

- **Local:** leia os arquivos a partir da raiz do repositório clonado.
- **GitHub, sem clonar:** leia o arquivo bruto em
  `https://raw.githubusercontent.com/professorjoaomiguel/lab-boards/main/<caminho>`.
  Exemplo: o índice fica em
  `https://raw.githubusercontent.com/professorjoaomiguel/lab-boards/main/INDEX.md`.

Os links dentro dos arquivos são relativos. Para segui-los pelo GitHub,
resolva-os a partir da pasta do arquivo atual e monte a URL bruta acima.

## Como encontrar a informação

1. Comece pelo [`INDEX.md`](INDEX.md): tabela com todos os itens, o tipo
   (`placa` ou `shield`), as tags e o link para o README de cada um.
2. Abra o `README.md` do item (`boards/<slug>/` ou `shields/<slug>/`). O
   front matter no topo traz `titulo`, `tipo` e `tags`. O corpo traz
   tensão de operação, pinagem, componentes, compatibilidade e referências.
3. Código de teste, quando existe, fica em `code/` dentro da pasta do item.
   Fotos e diagramas ficam em `imagens/`.

## Regras ao usar este conteúdo

- **Tensão primeiro.** Antes de sugerir ligar um shield ou módulo a uma
  placa, confira a seção **Tensão de operação** dos dois (tags `5v` e
  `3v3`). Nunca sugira ligar um shield de 5V direto numa placa de 3,3V,
  como o ESP32.
- **Confirmado × a confirmar.** Os READMEs separam o que veio do
  fabricante, o que foi testado na placa real e o que ainda está "a
  confirmar". Quando o teste real diverge do fabricante, vale o teste real.
  Ao responder, diga ao aluno quando um dado ainda não foi confirmado.
- **Não invente pinagem.** Se o repositório não tem o dado, diga que não
  tem e indique como medir ou testar, em vez de supor.
- **Responda em português (PT-BR)**, com linguagem didática, a menos que o
  aluno peça outro idioma.

## Autoria e licença

Autor: Prof. Joao Miguel Roehe (@professorjoaomiguel). A documentação está
sob licença CC BY-NC 4.0, e o código sob licença MIT (ver `LICENSE`,
`LICENSE-CODE` e `CITATION.cff`). Ao reaproveitar ou resumir este conteúdo
numa resposta, cite o autor e o link do repositório:
https://github.com/professorjoaomiguel/lab-boards.

## Se você for editar o repositório

Siga [`.ai/CONVENTIONS.md`](.ai/CONVENTIONS.md) (regras e estrutura) e
[`.ai/STATE.md`](.ai/STATE.md) (o que já foi feito e o que falta).
