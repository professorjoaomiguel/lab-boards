[← Site do professor](https://professorjoaomiguel.github.io/)

# Lab Boards

Material de consulta das **placas de desenvolvimento e shields usados no
laboratório** (Arduino, ESP32 e afins). Para cada placa você encontra a
tensão de operação, a pinagem, como programar, os cuidados e, quando
existe, um código de teste. Leia direto aqui no GitHub ou na versão em
site, [professorjoaomiguel.github.io/lab-boards](https://professorjoaomiguel.github.io/lab-boards/):
não precisa baixar nada.

A maioria das placas **ainda não tem foto** no repositório; por isso, a
pinagem e as ligações estão sempre em tabelas, em texto.

## Procurando uma placa?

| Placa | Tensão | Para que serve |
|-------|--------|----------------|
| [Arduino UNO R3](boards/arduino-uno-r3/README.md) | 5V | A placa Arduino clássica: primeiros passos com entradas e saídas, leitura analógica, PWM e serial |
| [Arduino UNO R4 (Minima / WiFi)](boards/arduino-uno-r4/README.md) | 5V | Sucessora do R3, mesmo formato e mesmos shields, processador de 32 bits |
| [ESP32-S3 UNO](boards/esp32-s3-uno/README.md) | **3,3V** | ESP32-S3 no formato do UNO: Wi-Fi, Bluetooth e IoT |
| [ESP32-S3 N16R8 DevKit](boards/esp32-s3-n16r8/README.md) | **3,3V** | ESP32-S3 estreita, de encaixar na protoboard, com bastante memória |
| [ESP32-C3 SuperMini](boards/esp32-c3-supermini/README.md) | **3,3V** | Placa minúscula com Wi-Fi e Bluetooth, para projetos pequenos de IoT |
| [Shield Multifunção 9 em 1](shields/uno-shield-9in1/README.md) | 5V | Encaixa sobre o UNO R3/R4: botões, LEDs, buzzer e sensores sem fios soltos |

A lista completa, agrupada por característica (Wi-Fi, USB-C, 5V, 3,3V...),
está no [`INDEX.md`](INDEX.md).

## ⚠️ Atenção à tensão (5V × 3,3V)

Antes de encaixar um shield em uma placa, confira a seção **Tensão de
operação** dos dois. O Arduino UNO trabalha em 5V; as placas ESP32
trabalham em 3,3V. Um shield de 5V sobre uma placa de 3,3V pode queimar as
portas do microcontrolador, **mesmo quando o encaixe é perfeito** (é o
caso do Shield 9 em 1 na ESP32-S3 UNO). No `INDEX.md`, as tags `5v` e
`3v3` agrupam os itens por tensão.

## Não conhece um termo?

O [`GLOSSARIO.md`](GLOSSARIO.md) explica, em ordem alfabética, os termos
técnicos usados aqui: ADC, pull-up, PSRAM, CH340, strapping...

## Que placa é essa?

Achou uma placa na bancada e não sabe qual é? O guia
[`IDENTIFICAR.md`](IDENTIFICAR.md) ajuda pelo que dá para ver: formato,
conector USB, módulo metálico, botões e o que está escrito nos chips.

## Usado em

| Disciplina | Placas |
|------------|--------|
| S086 - Sistemas Microprocessados | ESP32-S3 UNO, UNO R3, UNO R4 e Shield 9 em 1 |
| S122 - Internet das Coisas (previsto) | Todas as placas |
| S053 - Programação Básica (previsto) | UNO R3, UNO R4 e Shield 9 em 1 |

O material de cada disciplina está no
[site do professor](https://professorjoaomiguel.github.io/).

## Use com um agente de IA

Você pode pedir ajuda sobre estas placas a um agente de IA (Claude,
ChatGPT, Copilot, Gemini...). Passe para ele este link, que explica como
consultar o repositório e as regras de segurança (como a da tensão):

`https://raw.githubusercontent.com/professorjoaomiguel/lab-boards/main/AGENTS.md`

Exemplo de pedido: *"Leia o AGENTS.md deste link e me diga se posso ligar
o Shield 9 em 1 na ESP32-S3 UNO."*

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

## Para quem mantém

Como adicionar uma placa ou um shield, rodar os scripts e os testes:
[`CONTRIBUTING.md`](CONTRIBUTING.md). O inventário das unidades do
laboratório fica em [`inventario/`](inventario/README.md).
