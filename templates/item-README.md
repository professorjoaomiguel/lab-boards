---
titulo: "Nome do item"
tipo: placa            # placa | shield
autor: "Prof. Me. Joao Miguel Lac Roehe (@professorjoaomiguel)"
tags: [tag1, tag2, 5v]
---

# Nome do item

## Resumo rápido

| Característica | Valor |
|---|---|
| Tensão lógica | 5V ou 3,3V |
| Placa na IDE | nome no menu Ferramentas > Placa (FQBN `pacote:arquitetura:placa`) |
| Driver USB | qual chip de USB e se precisa de driver |
| LED embutido | pino e nível que acende (HIGH ou LOW) |
| Botões | quais existem (RESET, BOOT...) |
| Cuidado nº 1 | o erro mais provável de queimar ou travar a placa |

Só dados que este README confirma; o que falta, escreva "a confirmar".

## Visão geral
Breve descrição: o que é, para que serve, contexto de uso em aula.

## Tensão de operação
| Característica | Valor |
|---|---|
| Tensão lógica dos pinos | 5V ou 3,3V |
| Alimentação | ex: USB 5V, Vin 7–12V |
| Tolera 5V nas entradas? | sim / não |
| Corrente máxima por pino | ex: 20 mA |

Compatibilidade: com quais placas/shields pode ser usado com segurança, e
o que acontece se misturar 5V com 3,3V.

## Fotos
![Placa vista de cima](imagens/vista-de-cima.jpg)

Fotos previstas, na pasta `imagens/`: `vista-de-cima.jpg`,
`conector-usb.jpg` e `serigrafia.jpg`. Enquanto não houver foto, diga
quais faltam (a pinagem fica sempre em texto).

## Diagrama esquemático / Pinout
![esquemático](imagens/esquematico.png)

## Componentes principais
- ...

## Funcionalidades / Periféricos
- ...

## Como programar

### Arduino (C/C++): Arduino IDE
Pacote a instalar, placa e opções no menu Ferramentas, porta e driver.

### Arduino (C/C++): arduino-cli
FQBN e comandos `compile`/`upload`.

## Código de teste e validação
(preenchido futuramente — ver pasta `code/`)

## Referências
- Datasheet do microcontrolador: link
- Loja/fabricante: link

## Para o professor / histórico de testes
Ferramentas de terminal, inventário e resultados dos testes nas placas
reais. Fica no fim, fora do caminho do aluno. Apague a seção se não houver
conteúdo.

---

**Autor:** Prof. Me. Joao Miguel Lac Roehe ([@professorjoaomiguel](https://github.com/professorjoaomiguel)). Documentação sob licença [CC BY-NC 4.0](https://creativecommons.org/licenses/by-nc/4.0/deed.pt-br); código em `code/` sob licença MIT. Veja como citar no [README principal](../../README.md).
