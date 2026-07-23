"""Gera o INDEX.md do repositorio a partir do front matter dos itens em boards/ e shields/."""
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parent.parent
SECTIONS = ["boards", "shields"]


def _strip_inline_comment(value):
    value = value.strip()
    if value[:1] in ('"', "'"):
        quote = value[0]
        end = value.find(quote, 1)
        if end != -1:
            return value[: end + 1]
        return value
    return value.split("#", 1)[0].strip()


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
        value = _strip_inline_comment(value)
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
        "# Índice",
        "",
        "Gerado automaticamente por `scripts/gerar_indice.py`. Não editar à mão.",
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
