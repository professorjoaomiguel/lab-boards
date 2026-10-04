# lab-boards

As instruções para agentes de IA ficam no `AGENTS.md` da raiz, importado
abaixo. Para editar o repositório, siga também `.ai/CONVENTIONS.md` e
`.ai/STATE.md`.

@../AGENTS.md

## Handoff do `/remember` neste repositório

O `.remember/remember.md` é sobrescrito por inteiro a cada `/remember`. O
backlog versionado deste repositório é o `.ai/STATE.md`, então aqui a
regra global de "copiar para o Next todo item em aberto da nota anterior"
é cumprida assim:

1. Antes de escrever o handoff, leia a nota anterior inteira. Todo item em
   aberto dela que **não** esteja no `.ai/STATE.md` entra no STATE
   primeiro (com data), em commit próprio.
2. O handoff guarda só o que é da sessão: onde o trabalho parou, o que
   ficou pela metade, o commit atual e se houve push.
3. O "Next" lista no máximo os 3 próximos passos imediatos e termina com
   "Backlog completo: `.ai/STATE.md`". Não copie o resto do backlog.
4. O "Context" guarda só dicas operacionais que ainda não estão nos
   READMEs ou no CONVENTIONS.
