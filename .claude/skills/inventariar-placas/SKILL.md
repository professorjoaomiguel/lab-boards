---
name: inventariar-placas
description: Testa e registra no inventário as placas físicas do laboratório (Arduino UNO R4 Minima/WiFi e UNO R3), uma por vez, com scripts/serial_placa.py — grava o sketch de teste, lê o ID único, atualiza o CSV de inventario/, sugere o texto da etiqueta, regera o relatório e faz commit. Use sempre que o professor disser que ligou, conectou ou trocou uma placa, quiser testar, retestar, registrar, catalogar ou etiquetar placas do lab, perguntar o que colar na etiqueta, ou disser só "vai"/"próxima" no meio de uma rodada de placas — mesmo sem citar o script ou o inventário.
---

# Inventariar placas do laboratório

Rotina para testar placas físicas e registrar cada unidade em
`inventario/`. Ela foi feita na prática (2026-10-04, quatro UNO R4 Minima)
e guarda as lições daquela rodada. O script faz a parte mecânica; esta
skill cuida do que fica em volta: uma placa por vez, desconfiar de
resultado estranho antes de gravar no inventário, etiqueta, dono, relatório
e commits.

Leia antes, se ainda não leu nesta sessão: `inventario/README.md` (colunas
do CSV e como cada modelo se identifica) e a seção "Inventário" do README
da placa (`boards/<slug>/README.md`).

## Quando o script não serve

`serial_placa.py --registrar` só reconhece placas com número de série USB:
**UNO R4** (ID de 128 bits do RA4M1) e **UNO R3 com ATmega16U2**. Para
**UNO R3 com CH340**, **ESP32-S3 UNO** e **shields**, o registro é à mão,
como explica o `inventario/README.md`. Nesses casos, siga o README e use
desta skill só os passos de etiqueta, relatório e commit.

## Antes da primeira placa

1. `python scripts/serial_placa.py verificar` confere Python, pyserial,
   arduino-cli e os pacotes de placa. Se faltar algo, resolva antes.
2. Combine com o professor:
   - **Placa sozinha:** sem shield, sem jumper e sem fio nos pinos. O teste
     `gpio` liga os pinos como saída, e um módulo ligado neles pode
     queimar (o R4 aguenta só 8 mA por pino).
   - **Jumper D0↔D1:** com ele o teste `serial1` roda; sem ele, fica
     "pulado". Use a mesma escolha em todas as placas da rodada, para os
     resultados serem comparáveis.
   - **Dono:** `professor`, `SENAI` ou `a confirmar`. Pergunte uma vez se
     vale para todas as placas da rodada.
   - **Modelo:** R4 Minima, R4 WiFi ou R3. A R4 WiFi ainda não foi testada
     no laboratório; trate o resultado dela com atenção redobrada.

## Para cada placa

O laboratório tem uma USB livre: uma placa por vez. Quando o professor
disser que ligou a placa (ou só "vai"), rode:

```bash
python scripts/serial_placa.py auto --gravar --registrar
```

Sem `--porta`, o script usa a única placa reconhecida. Se houver mais de
uma, rode `listar` e passe `--porta COMx`. Se a gravação falhar por DFU ou
LIBUSB, peça ao professor para apertar o RESET duas vezes rápido e rode de
novo.

O final da saída diz uma de duas coisas:

- `Placa NOVA registrada como R4M-NN`: o script criou a linha com dono
  `a confirmar`, `etiqueta_colada` = `não` e `entrou_em` vazio. Pergunte
  ao professor o dono e quando a placa chegou ao laboratório (basta o
  mês: `AAAA-MM`) e preencha no CSV.
- `Placa R4M-NN já registrada: último teste atualizado`: a placa já
  existia. Avise o professor, porque ele pode ter ligado a mesma placa sem
  querer.

### Leia o resultado antes de aceitar

O script grava no CSV qualquer resultado. Antes de fazer commit, olhe o
RESUMO e compare com a placa anterior e com o último teste dessa mesma
placa (`git diff inventario/`).

- **FALHA:** pode ser defeito da placa ou do teste. Repita uma vez. Se a
  falha continuar, anote no `obs` (ex: "D3 não segura HIGH, 2026-10-04")
  e conte ao professor. Não apague o `obs`, que foi escrito à mão.
- **PULADO numa placa sem nada ligado:** suspeite do **teste**, não da
  placa. Foi assim que apareceu o pull-up de 4,7 kΩ em A4/A5 do R4 Minima
  (o manual da Arduino diz que ele não existe). O teste antigo achava que
  havia um shield e pulava o GPIO em toda placa. Se a mesma coisa for
  pulada em duas placas limpas, pare a rodada e investigue: meça com um
  sketch de diagnóstico, leia o código do teste e procure no manual da
  placa. Não deixe no inventário um resultado pior causado por um teste
  errado. Restaure a linha anterior (`git show HEAD:inventario/<csv>`).
- **Resultado diferente entre placas do mesmo modelo:** vale contar ao
  professor, mesmo sem falha (ex: AVCC 4,6 V numa e 4,9 V nas outras).

### Etiqueta

O texto da etiqueta é a etiqueta do CSV (`R4M-03`) mais um trecho curto do
identificador, que serve para conferir que a etiqueta está na placa certa.
A chave do inventário continua sendo o identificador completo, lido pelo
script.

Para escolher o trecho, compare os IDs de **todas** as placas do mesmo
modelo no CSV e use o trecho mais curto que seja diferente em todas. Nas
UNO R4 Minima, o começo e o fim do ID se repetem entre placas do mesmo
lote. Em 2026-10-04, os caracteres 17 a 20 (contando do 1) eram diferentes
nas quatro placas (`538E`, `D01F`, `AA2F`, `5B1E`). Confira de novo a cada
placa nova: um lote novo pode repetir esse trecho.

Para o R4, com o trecho atual (caracteres 17 a 20):

```bash
python -c "import csv; [print(r['etiqueta'], r['id_unico'][16:20]) for r in csv.DictReader(open('inventario/arduino-uno-r4.csv', encoding='utf-8'))]"
```

Nos outros modelos o identificador é diferente (número de série USB no R3
com 16U2, MAC no ESP32-S3: veja os nomes das colunas no CSV). Faça a mesma
comparação, sem copiar o `[16:20]`.

Formato: `R4M-03 · AA2F`. Quando o professor disser que colou, mude
`etiqueta_colada` para `sim`.

### Fechar a placa

1. Ajuste o CSV, se precisar (dono, `entrou_em`, `obs`).
2. `python scripts/gerar_inventario.py` (o relatório vem dos CSVs; nunca
   edite o HTML à mão).
3. Commit **local** do CSV junto com o `relatorio.html`, um por placa, ex:
   `Inventory: register R4M-03 (UNO R4 Minima, professor)`. Não faça push
   sem pedir: é regra do professor.
4. Diga qual é a próxima placa (as que faltam) e espere ele trocar.

## Se precisar mudar o sketch de teste no meio da rodada

1. Mude o sketch (`boards/<slug>/code/teste_*_automatico/`), **aumente o
   `VERSAO`** e compile com o FQBN da placa (README da placa, "Resumo
   rápido"), ex: `arduino-cli compile -b arduino:renesas_uno:minima
   <pasta do sketch>`.
2. Explique a mudança no comentário do código e no README da placa
   (tabela do "Teste da placa"). Se o achado contradiz o fabricante, ele
   entra no README como "achado na placa real", com a data e a medição.
   No repositório, o teste real vale mais que o fabricante.
3. Faça commit da mudança do sketch separado do inventário.
4. **Teste de novo todas as placas da rodada** com a versão nova. Assim
   todas ficam no inventário com a mesma versão de teste.

## Fim da rodada

1. Resumo para o professor: tabela com etiqueta, texto da etiqueta,
   resultado e o que chamou atenção.
2. `.ai/STATE.md`:
   - Atualize a linha da placa na tabela de status.
   - Tire de "Em aberto" o que foi feito (ex: "faltam as outras R4").
   - Ponha uma linha em "Resolvido" com a data, o que mudou e onde ficou
     documentado.
   - Rode `python scripts/verificar_state.py`.
3. README da placa: tabela "A confirmar na placa real".
4. `python -m pytest -q tests` e commit final.
5. Pergunte se pode fazer push. As etiquetas físicas ainda não coladas
   continuam como pendência em "Inventário" no STATE.

## Por que é assim

- **Uma por vez e sem nada nos pinos:** o ID único e o teste dos pinos só
  fazem sentido com uma placa sozinha. Um shield faz o teste pular os
  pinos (é a proteção) ou, pior, recebe sinais que não esperava.
- **Commits pequenos, por placa:** se uma placa der problema, o histórico
  mostra exatamente o que mudou e quando. É também a preferência do
  professor.
- **Suspeitar do teste:** um inventário cheio de "pulado" ou "falha" que
  vieram do teste, e não da placa, engana quem consultar depois. Esse
  inventário é público e serve de referência para os alunos.
