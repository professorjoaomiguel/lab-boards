# Inventário de placas e shields

Lista das placas e shields físicos do laboratório, com a identificação de
cada um. Fica **separada do código de teste**: os sketches e scripts ficam
nas pastas de cada item (`boards/<slug>/code`, `shields/<slug>/code`) e em
`scripts/`; aqui fica só o registro de **quais unidades existem**, de quem
são e o que se sabe de cada uma.

O objetivo é ir montando uma lista de placas **conhecidas** (registradas,
com etiqueta) à medida que cada uma é conectada e identificada. Uma placa
que ainda não está aqui é **desconhecida**.

## Relatório

Para olhar o inventário, abra **[`relatorio.html`](relatorio.html)** (dois
cliques no arquivo, depois de clonar o repositório). Ele mostra todas as
unidades numa tabela só, com filtros por tipo, dono, situação e etiqueta
colada, busca livre, contadores e colunas ordenáveis. Os filtros ficam na
URL (ex: `relatorio.html#situacao=com+falha`), o que permite guardar ou
compartilhar uma visão.

O relatório é **gerado a partir dos CSVs**: não edite o HTML. Depois de
mudar um CSV, gere de novo e faça commit dos dois juntos:

```bash
python scripts/gerar_inventario.py
```

## Arquivos

| Arquivo | Itens | Identificador (a chave) |
|---------|-------|-------------------------|
| [`arduino-uno-r4.csv`](arduino-uno-r4.csv) | Arduino UNO R4 Minima / WiFi | ID único de 128 bits do RA4M1 (no Minima, é o número de série USB) |
| [`arduino-uno-r3.csv`](arduino-uno-r3.csv) | Arduino UNO R3 e compatíveis | Número de série USB do ATmega16U2 (só nas placas com esse chip) |
| [`esp32-s3-uno.csv`](esp32-s3-uno.csv) | ESP32-S3 UNO | Endereço MAC (gravado em eFuse) |
| [`shield-9em1.csv`](shield-9em1.csv) | Shield Multifunção 9 em 1 | Só a etiqueta física (o shield não tem número de série) |

Como ler o identificador de cada placa: seção "Inventário" do README de
cada uma ([UNO R4](../boards/arduino-uno-r4/README.md#inventário-identificar-cada-placa),
[UNO R3](../boards/arduino-uno-r3/README.md#inventário-identificar-cada-placa),
[ESP32-S3 UNO](../boards/esp32-s3-uno/README.md#identificar-cada-placa-mac-e-inventário)).

## Colunas comuns

| Coluna | Conteúdo |
|--------|----------|
| `etiqueta` | Nome curto para a etiqueta física: `R4M-01`, `R4W-01`, `R3-01`, `S9-01`… No ESP32-S3 UNO, os dois últimos bytes do MAC (ex: `1e:20`) |
| `etiqueta_colada` | `sim` quando a etiqueta já está colada na peça; `não` enquanto não estiver |
| `dono` | `professor` (do Prof. Joao Miguel), `SENAI` ou `a confirmar` |
| `variante` | UNO R3: `ATmega16U2` (original e clones de melhor qualidade) ou `CH340` (clones) |
| `registrado_em`, `ultimo_teste` | Datas (AAAA-MM-DD) |
| `resultado` | Resumo do último teste (ex: `ok=5 falha=0 aviso=0 pulado=0`) |
| `obs` | Defeitos, consertos e observações. Escrita à mão; o script não apaga |

As outras colunas são próprias de cada arquivo (ex: flash e PSRAM no
ESP32-S3 UNO; `como_reconhecer` nos shields).

## Como registrar

- **UNO R4 e UNO R3 com ATmega16U2:** com a placa sozinha conectada, o
  teste da placa registra sozinho (uma placa por vez):

  ```bash
  python scripts/serial_placa.py auto --porta COMx --gravar --registrar
  ```

  Uma placa nova ganha a próxima etiqueta, com `dono` = `a confirmar` e
  `etiqueta_colada` = `não`. Depois, edite o CSV: preencha o dono e mude
  para `sim` quando colar a etiqueta.
- **UNO R3 com CH340:** não tem número de série USB, então não dá para
  reconhecer a placa automaticamente. Registre à mão, com a etiqueta e uma
  descrição em `obs`.
- **ESP32-S3 UNO:** à mão, a partir do MAC (ver o README da placa).
- **Shields:** à mão. O shield não tem identificador eletrônico: use a
  etiqueta física e descreva como reconhecê-lo em `como_reconhecer`.

## Privacidade

O repositório é público. O inventário não guarda dados pessoais: quem está
com cada placa (aluno, turma) não entra aqui.

---

**Autor:** Prof. Joao Miguel Roehe ([@professorjoaomiguel](https://github.com/professorjoaomiguel)). Documentação sob licença [CC BY-NC 4.0](https://creativecommons.org/licenses/by-nc/4.0/deed.pt-br). Veja como citar no [README principal](../README.md).
