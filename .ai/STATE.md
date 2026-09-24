# Estado do projeto

Log vivo do que já foi feito e do que falta neste repositório. Atualize
este arquivo sempre que adicionar um item novo ou tomar uma decisão
estrutural.

## Itens documentados

| Item | Tipo | Status |
|------|------|--------|
| [ESP32-S3 N16R8 DevKit](../boards/esp32-s3-n16r8/README.md) | placa | Estrutura completa; fotos e diagrama ainda pendentes |
| [Arduino UNO R3](../boards/arduino-uno-r3/README.md) | placa | Estrutura e datasheets completos; fotos pendentes |
| [Arduino UNO R4 (Minima / WiFi)](../boards/arduino-uno-r4/README.md) | placa | Estrutura e datasheets completos; fotos pendentes |
| [Shield Multifunção 9 em 1 (UNO)](../shields/uno-shield-9in1/README.md) | shield | Pinagem, fotos e componentes completos; código de teste pendente |

## Decisões tomadas

- Estrutura do repositório, template de item, sistema de tags e script de
  índice: ver `docs/superpowers/specs/2026-07-23-repositorio-documentacao-placas-design.md`.
- Plano de implementação do scaffold inicial: ver
  `docs/superpowers/plans/2026-07-23-lab-dev-boards-scaffold.md`.
- Seção **Tensão de operação** obrigatória e tag `5v`/`3v3` em todo item,
  para evitar queimar portas misturando placas e shields de tensões
  diferentes (2026-09-24). Ver `.ai/CONVENTIONS.md`.
- `.ai/` como SSoT para agentes de IA: ver o adendo no spec acima
  (2026-07-23).

## Próximos passos

- Adicionar fotos de `boards/arduino-uno-r3/` e `boards/arduino-uno-r4/`.
- Adicionar fotos e diagrama esquemático reais de `boards/esp32-s3-n16r8/`.
- Documentar mais placas e shields conforme forem usados em aula.
- Escrever código de teste de cada periférico do shield 9 em 1
  (`shields/uno-shield-9in1/code/`).
- Preencher `code/` de cada item com código de teste/validação de
  periféricos, quando disponível.
