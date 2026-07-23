# Lab Dev Boards — Scaffold Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Scaffold the `lab_dev_boards` documentation repository: root docs, item template, the tag-based index generator, and a first real board entry that proves the whole pipeline end-to-end.

**Architecture:** Plain Markdown files organized under `boards/` and `shields/`, each item carrying YAML front matter (`titulo`, `tipo`, `tags`). A small dependency-free Python script reads that front matter across the repo and regenerates a root `INDEX.md` grouped by tag. No static site generator, no CI automation — generation is a manual command run after adding or editing an item.

**Tech Stack:** Markdown, Python 3 standard library only (`pathlib`, `unittest`) — no `pyyaml` or other third-party packages.

## Global Constraints

- Documentação escrita em português (todo conteúdo Markdown gerado).
- Markdown puro navegado no GitHub — sem MkDocs/Docusaurus ou qualquer build step.
- `scripts/gerar_indice.py` não pode depender de bibliotecas externas (só stdlib).
- A geração do `INDEX.md` é manual (`python scripts/gerar_indice.py`) — sem git hook nem CI.
- Código de teste/validação de periféricos fica dentro da pasta do próprio item (`<item>/code/`), não em uma árvore separada no topo do repo.
- Sem `AGENTS.md`/`CLAUDE.md` dedicado por enquanto — convenções ficam só no README raiz.
- `boards/` e `shields/` são pastas separadas por tipo (não uma pasta única `devices/`).

---

### Task 1: Estrutura de pastas e documentação raiz

**Files:**
- Create: `README.md`
- Create: `templates/item-README.md`
- Create: `shields/README.md`

**Interfaces:**
- Produces: o template em `templates/item-README.md` é a fonte copiada para criar qualquer novo item (consumido diretamente por humanos/agentes na Task 3 e em itens futuros).

- [ ] **Step 1: Criar o README raiz**

Crie `README.md` na raiz do repositório com este conteúdo exato:

```markdown
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

1. Copie [`templates/item-README.md`](templates/item-README.md) para
   `boards/<slug-do-item>/README.md` ou `shields/<slug-do-item>/README.md`.
2. Preencha o front matter (`titulo`, `tipo`, `tags`) e as seções do
   template.
3. Crie as subpastas `imagens/` (fotos e diagramas) e `code/` (código de
   teste/validação, quando existir) dentro da pasta do item.
4. Rode `python scripts/gerar_indice.py` para atualizar o `INDEX.md`.
5. Faça commit do item novo junto com o `INDEX.md` atualizado.
```

- [ ] **Step 2: Criar o template de item**

Crie `templates/item-README.md` com este conteúdo exato:

```markdown
---
titulo: "Nome do item"
tipo: placa            # placa | shield
tags: [tag1, tag2]
---

# Nome do item

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

- [ ] **Step 3: Criar o placeholder de shields**

Crie `shields/README.md` com este conteúdo exato:

```markdown
# Shields

Ainda sem itens documentados. Para adicionar um shield (teclado, display,
etc.), siga o processo descrito no [README raiz](../README.md): copie
`templates/item-README.md` para `shields/<slug-do-shield>/README.md`.
```

- [ ] **Step 4: Verificar que os três arquivos existem com o conteúdo esperado**

Run: `grep -l "Como adicionar um item novo" README.md && grep -l "titulo: \"Nome do item\"" templates/item-README.md && grep -l "Ainda sem itens documentados" shields/README.md`
Expected: os três caminhos impressos (`README.md`, `templates/item-README.md`, `shields/README.md`), sem erro.

- [ ] **Step 5: Commit**

```bash
git add README.md templates/item-README.md shields/README.md
git commit -m "Add root docs, item template, and shields placeholder"
```

---

### Task 2: Script de geração do índice (TDD)

**Files:**
- Create: `scripts/test_gerar_indice.py`
- Create: `scripts/gerar_indice.py`

**Interfaces:**
- Consumes: nenhuma (primeira peça de código do repo).
- Produces:
  - `parse_front_matter(readme_path: Path) -> dict` — retorna `{"titulo": str, "tipo": str, "tags": list[str]}` a partir do front matter YAML simples de um `README.md`.
  - `find_items(root: Path = REPO_ROOT) -> list[dict]` — cada item é `{"titulo": str, "tipo": str, "tags": list[str], "link": str}`, varrendo `boards/*/README.md` e `shields/*/README.md` sob `root`.
  - `generate_index(items: list[dict]) -> str` — markdown com tabela geral e seções por tag.
  - `main() -> None` — chama `find_items()`, `generate_index()`, e escreve `INDEX.md` na raiz do repo.
  - Usados pela Task 3 via `python scripts/gerar_indice.py` (chama `main()`).

- [ ] **Step 1: Escrever os testes (devem falhar — o script ainda não existe)**

Crie `scripts/test_gerar_indice.py`:

```python
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import gerar_indice as gi


class TestParseFrontMatter(unittest.TestCase):
    def test_parses_titulo_tipo_e_tags(self):
        with tempfile.TemporaryDirectory() as tmp:
            readme = Path(tmp) / "README.md"
            readme.write_text(
                '---\n'
                'titulo: "ESP32 DevKit V1"\n'
                'tipo: placa\n'
                'tags: [esp32, wifi, bluetooth]\n'
                '---\n'
                '# ESP32 DevKit V1\n',
                encoding="utf-8",
            )
            data = gi.parse_front_matter(readme)
            self.assertEqual(data["titulo"], "ESP32 DevKit V1")
            self.assertEqual(data["tipo"], "placa")
            self.assertEqual(data["tags"], ["esp32", "wifi", "bluetooth"])


class TestFindItems(unittest.TestCase):
    def test_encontra_itens_em_boards_e_shields(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            board_dir = root / "boards" / "esp32-devkit-v1"
            board_dir.mkdir(parents=True)
            (board_dir / "README.md").write_text(
                '---\n'
                'titulo: "ESP32 DevKit V1"\n'
                'tipo: placa\n'
                'tags: [esp32, wifi]\n'
                '---\n'
                '# ESP32\n',
                encoding="utf-8",
            )
            shield_dir = root / "shields" / "display-oled"
            shield_dir.mkdir(parents=True)
            (shield_dir / "README.md").write_text(
                '---\n'
                'titulo: "Display OLED"\n'
                'tipo: shield\n'
                'tags: [display, i2c]\n'
                '---\n'
                '# Display\n',
                encoding="utf-8",
            )

            items = gi.find_items(root=root)

            self.assertEqual(len(items), 2)
            titles = {item["titulo"] for item in items}
            self.assertEqual(titles, {"ESP32 DevKit V1", "Display OLED"})
            esp32_item = next(item for item in items if item["titulo"] == "ESP32 DevKit V1")
            self.assertEqual(esp32_item["link"], "boards/esp32-devkit-v1/README.md")


class TestGenerateIndex(unittest.TestCase):
    def test_agrupa_itens_por_tag(self):
        items = [
            {
                "titulo": "ESP32 DevKit V1",
                "tipo": "placa",
                "tags": ["esp32", "wifi"],
                "link": "boards/esp32-devkit-v1/README.md",
            },
            {
                "titulo": "Display OLED",
                "tipo": "shield",
                "tags": ["display", "i2c"],
                "link": "shields/display-oled/README.md",
            },
        ]
        content = gi.generate_index(items)
        self.assertIn("### esp32", content)
        self.assertIn(
            "- [ESP32 DevKit V1](boards/esp32-devkit-v1/README.md)", content
        )
        self.assertIn("### display", content)
        self.assertIn(
            "| ESP32 DevKit V1 | placa | esp32, wifi |"
            " [boards/esp32-devkit-v1/README.md](boards/esp32-devkit-v1/README.md) |",
            content,
        )


if __name__ == "__main__":
    unittest.main()
```

- [ ] **Step 2: Rodar os testes e confirmar que falham**

Run: `python -m unittest scripts/test_gerar_indice.py -v`
Expected: FAIL logo na importação, com `ModuleNotFoundError: No module named 'gerar_indice'`.

- [ ] **Step 3: Implementar `scripts/gerar_indice.py`**

Crie `scripts/gerar_indice.py`:

```python
"""Gera o INDEX.md do repositorio a partir do front matter dos itens em boards/ e shields/."""
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parent.parent
SECTIONS = ["boards", "shields"]


def parse_front_matter(readme_path):
    text = readme_path.read_text(encoding="utf-8")
    lines = text.splitlines()
    if not lines or lines[0].strip() != "---":
        raise ValueError(f"{readme_path} nao comeca com front matter '---'")

    end = None
    for i in range(1, len(lines)):
        if lines[i].strip() == "---":
            end = i
            break
    if end is None:
        raise ValueError(f"{readme_path} nao tem front matter fechado com '---'")

    data = {}
    for line in lines[1:end]:
        line = line.strip()
        if not line or ":" not in line:
            continue
        key, value = line.split(":", 1)
        key = key.strip()
        value = value.strip()
        if key == "tags":
            value = value.strip("[]")
            data[key] = [
                t.strip().strip('"').strip("'") for t in value.split(",") if t.strip()
            ]
        else:
            data[key] = value.strip('"').strip("'")
    return data


def find_items(root=REPO_ROOT):
    items = []
    for section in SECTIONS:
        section_dir = root / section
        if not section_dir.is_dir():
            continue
        for item_dir in sorted(section_dir.iterdir()):
            if not item_dir.is_dir():
                continue
            readme = item_dir / "README.md"
            if not readme.is_file():
                continue
            front_matter = parse_front_matter(readme)
            items.append(
                {
                    "titulo": front_matter.get("titulo", item_dir.name),
                    "tipo": front_matter.get("tipo", section),
                    "tags": front_matter.get("tags", []),
                    "link": f"{section}/{item_dir.name}/README.md",
                }
            )
    return items


def generate_index(items):
    lines = [
        "# Indice",
        "",
        "Gerado automaticamente por `scripts/gerar_indice.py`. Nao editar a mao.",
        "",
        "## Todos os itens",
        "",
        "| Nome | Tipo | Tags | Link |",
        "|------|------|------|------|",
    ]
    for item in items:
        tags = ", ".join(item["tags"])
        lines.append(
            f"| {item['titulo']} | {item['tipo']} | {tags} |"
            f" [{item['link']}]({item['link']}) |"
        )
    lines.append("")

    tags_map = {}
    for item in items:
        for tag in item["tags"]:
            tags_map.setdefault(tag, []).append(item)

    lines.append("## Por tag")
    lines.append("")
    for tag in sorted(tags_map.keys()):
        lines.append(f"### {tag}")
        lines.append("")
        for item in tags_map[tag]:
            lines.append(f"- [{item['titulo']}]({item['link']})")
        lines.append("")

    return "\n".join(lines).rstrip() + "\n"


def main():
    items = find_items()
    content = generate_index(items)
    (REPO_ROOT / "INDEX.md").write_text(content, encoding="utf-8")
    print(f"INDEX.md atualizado com {len(items)} item(ns).")


if __name__ == "__main__":
    main()
```

- [ ] **Step 4: Rodar os testes e confirmar que passam**

Run: `python -m unittest scripts/test_gerar_indice.py -v`
Expected: `OK` com 3 testes passando (`test_parses_titulo_tipo_e_tags`, `test_encontra_itens_em_boards_e_shields`, `test_agrupa_itens_por_tag`).

- [ ] **Step 5: Commit**

```bash
git add scripts/gerar_indice.py scripts/test_gerar_indice.py
git commit -m "Add dependency-free index generator with tests"
```

---

### Task 3: Primeiro item real (ESP32-S3 N16R8) e geração do índice

**Files:**
- Create: `boards/esp32-s3-n16r8/README.md`
- Create: `boards/esp32-s3-n16r8/imagens/.gitkeep`
- Create: `boards/esp32-s3-n16r8/code/.gitkeep`
- Create: `INDEX.md` (gerado pelo script da Task 2)

**Interfaces:**
- Consumes: `templates/item-README.md` (Task 1) como base do conteúdo; `python scripts/gerar_indice.py` (Task 2, função `main()`) para gerar `INDEX.md`.

- [ ] **Step 1: Criar a pasta do item com README preenchido**

Crie `boards/esp32-s3-n16r8/README.md` com este conteúdo exato:

```markdown
---
titulo: "ESP32-S3 N16R8 DevKit"
tipo: placa
tags: [esp32, esp32-s3, wifi, bluetooth, devkit, usb-c]
---

# ESP32-S3 N16R8 DevKit

## Visão geral
Placa de desenvolvimento baseada no módulo ESP32-S3 (dual-core Xtensa LX7),
com 16MB de memória flash e 8MB de PSRAM (daí o sufixo "N16R8"). Usada em
aula para projetos que exigem mais memória do que o ESP32 clássico, como
processamento de imagem, buffers de tela e projetos com Wi-Fi/Bluetooth
simultâneos.

Produto de referência: https://makerselectronics.com/product/esp32-s3-n16r8-development-board-2

## Fotos
_Foto ainda não adicionada — colocar o arquivo em `imagens/` e referenciar
aqui com `![foto](imagens/nome-do-arquivo.jpg)`._

## Diagrama esquemático / Pinout
_Diagrama ainda não adicionado — colocar o arquivo em `imagens/` e
referenciar aqui com `![esquemático](imagens/nome-do-arquivo.png)`._

## Componentes principais
- Módulo ESP32-S3 (dual-core Xtensa LX7, Wi-Fi 802.11 b/g/n, Bluetooth 5 LE)
- 16MB de memória flash
- 8MB de PSRAM
- Conector USB-C (programação e alimentação)
- Botões BOOT e RESET

## Funcionalidades / Periféricos
- Wi-Fi e Bluetooth Low Energy integrados
- GPIOs disponíveis para I2C, SPI, UART, PWM e ADC
- Suporte nativo a USB (OTG) via ESP32-S3

## Código de teste e validação
(preenchido futuramente — ver pasta `code/`)

## Referências
- Produto: https://makerselectronics.com/product/esp32-s3-n16r8-development-board-2
```

- [ ] **Step 2: Criar as subpastas vazias `imagens/` e `code/`**

```bash
mkdir -p boards/esp32-s3-n16r8/imagens boards/esp32-s3-n16r8/code
touch boards/esp32-s3-n16r8/imagens/.gitkeep boards/esp32-s3-n16r8/code/.gitkeep
```

- [ ] **Step 3: Rodar o gerador de índice**

Run: `python scripts/gerar_indice.py`
Expected: `INDEX.md atualizado com 1 item(ns).` e o arquivo `INDEX.md` criado na raiz do repositório.

- [ ] **Step 4: Verificar o conteúdo gerado**

Run: `grep -c "ESP32-S3 N16R8 DevKit" INDEX.md`
Expected: `7` — uma ocorrência na tabela geral em "Todos os itens", mais uma ocorrência em cada uma das 6 seções de tag (`bluetooth`, `devkit`, `esp32`, `esp32-s3`, `usb-c`, `wifi`), já que é o único item e carrega as 6 tags. Confirme também com `grep "### esp32-s3" INDEX.md` que a tag composta aparece como seção própria.

- [ ] **Step 5: Commit**

```bash
git add boards/esp32-s3-n16r8 INDEX.md
git commit -m "Add first board entry (ESP32-S3 N16R8) and generated index"
```
