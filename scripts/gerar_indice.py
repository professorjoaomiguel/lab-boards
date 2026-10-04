# Autor: Prof. Me. Joao Miguel Lac Roehe (@professorjoaomiguel)
# SPDX-License-Identifier: MIT
"""Gera o INDEX.md do repositório a partir do front matter dos itens.

O QUE FAZ
    Percorre as pastas `boards/` e `shields/`, lê o front matter (o bloco
    entre `---` no topo) do `README.md` de cada item e escreve o `INDEX.md`
    na raiz do repositório com:
      - uma tabela com todos os itens (nome, tipo, tags e link);
      - uma seção por tag, listando os itens que têm aquela tag.

COMO USAR
    Na raiz do repositório:

        python scripts/gerar_indice.py          # gera o INDEX.md
        python scripts/gerar_indice.py --help   # mostra esta ajuda

    Rode sempre que criar um item ou mudar o front matter de algum. A
    geração é manual: não há hook de git nem CI fazendo isso.

FORMATO ESPERADO DO FRONT MATTER
    ---
    titulo: "Arduino UNO R3"
    tipo: placa                # placa | shield
    autor: "Prof. Me. Joao Miguel Lac Roehe (@professorjoaomiguel)"
    tags: [arduino, uno, 5v]
    ---

    - Uma chave por linha, no formato `chave: valor`.
    - `tags` é uma lista entre colchetes, separada por vírgulas.
    - Comentários `# ...` no fim da linha são ignorados.

TESTES
    python -m unittest tests/test_gerar_indice.py -v
"""
import argparse
from pathlib import Path

# Raiz do repositório: a pasta acima de scripts/. Assim o script funciona
# qualquer que seja a pasta de onde ele foi chamado.
REPO_ROOT = Path(__file__).resolve().parent.parent

# Pastas de primeiro nível que contêm itens (uma subpasta por item).
SECTIONS = ["boards", "shields"]


def _strip_inline_comment(value):
    """Remove um comentário `# ...` do fim de um valor do front matter.

    Um valor entre aspas é mantido inteiro, mesmo que contenha `#`, porque
    ali o `#` faz parte do texto (ex: `titulo: "Teclado #2"`).

    Args:
        value: o texto depois de `chave:` na linha do front matter.

    Returns:
        O valor sem o comentário e sem espaços nas pontas.
    """
    value = value.strip()
    if value[:1] in ('"', "'"):
        quote = value[0]
        end = value.find(quote, 1)
        if end != -1:
            return value[: end + 1]
        return value
    return value.split("#", 1)[0].strip()


def parse_front_matter(readme_path):
    """Lê o front matter do topo de um README.md.

    É um leitor simples, feito só para o formato deste repositório (não é
    um parser YAML completo): uma chave por linha e `tags` como lista entre
    colchetes.

    Args:
        readme_path: caminho (`Path`) do README.md do item.

    Returns:
        Um dicionário com as chaves encontradas. `tags` vira uma lista de
        strings; as demais chaves viram strings, sem aspas.

    Raises:
        ValueError: se o arquivo não começa com `---` ou se o bloco não é
            fechado por outra linha `---`.
    """
    text = readme_path.read_text(encoding="utf-8")
    lines = text.splitlines()
    if not lines or lines[0].strip() != "---":
        raise ValueError(f"{readme_path} nao comeca com front matter '---'")

    # Procura a linha `---` que fecha o bloco.
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
        # Linhas vazias ou sem `:` não são pares chave/valor.
        if not line or ":" not in line:
            continue
        key, value = line.split(":", 1)
        key = key.strip()
        value = _strip_inline_comment(value.strip())
        if key == "tags":
            # "[a, b, c]" -> ["a", "b", "c"]; "[]" -> []
            value = value.strip("[]")
            data[key] = [
                t.strip().strip('"').strip("'") for t in value.split(",") if t.strip()
            ]
        else:
            data[key] = value.strip('"').strip("'")
    return data


def find_items(root=REPO_ROOT):
    """Encontra todos os itens documentados em `boards/` e `shields/`.

    Um item é uma subpasta com um README.md. Um README.md solto na raiz da
    seção (ex: `shields/README.md`) não é um item e é ignorado.

    Args:
        root: raiz do repositório. Os testes passam uma pasta temporária.

    Returns:
        Lista de dicionários com `titulo`, `tipo`, `tags` e `link` (caminho
        relativo à raiz), em ordem de seção e depois de nome de pasta. Se o
        front matter não tiver `titulo` ou `tipo`, usa o nome da pasta e o
        nome da seção.
    """
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
                    # Barra normal: o link precisa funcionar no GitHub.
                    "link": f"{section}/{item_dir.name}/README.md",
                }
            )
    return items


def generate_index(items):
    """Monta o texto Markdown do INDEX.md.

    Args:
        items: lista no formato devolvido por `find_items`.

    Returns:
        O conteúdo completo do INDEX.md, terminando com uma única quebra de
        linha. As seções por tag saem em ordem alfabética.
    """
    lines = [
        "[← Site do professor](https://professorjoaomiguel.github.io/)",
        "",
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

    # Inverte a relação item -> tags para tag -> itens.
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


def main(argv=None):
    """Ponto de entrada da linha de comando: gera o INDEX.md na raiz.

    Args:
        argv: argumentos da linha de comando, sem o nome do script. `None`
            usa `sys.argv` (uso normal); os testes passam uma lista.
    """
    # O docstring do módulo vira o texto do --help, para a ajuda ficar num
    # lugar só. RawDescriptionHelpFormatter mantém a formatação original.
    parser = argparse.ArgumentParser(
        prog="python scripts/gerar_indice.py",
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    parser.parse_args(argv)

    items = find_items()
    content = generate_index(items)
    (REPO_ROOT / "INDEX.md").write_text(content, encoding="utf-8")
    print(f"INDEX.md atualizado com {len(items)} item(ns).")


if __name__ == "__main__":
    main()
