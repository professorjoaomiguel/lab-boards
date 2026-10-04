# Autor: Prof. Joao Miguel Roehe (@professorjoaomiguel)
# SPDX-License-Identifier: MIT
"""Confere se o .ai/STATE.md segue as próprias regras de manutenção.

Checagens:

- links relativos apontam para arquivos que existem e, quando têm
  ``#âncora``, para um título que existe no arquivo de destino;
- a seção "Resolvido" só guarda entradas dos últimos ~30 dias (contados a
  partir da entrada mais recente, para o resultado não depender da data de
  hoje);
- o arquivo não passa do limite de linhas. Passou: é hora de mover o
  detalhamento para arquivos por tópico e deixar no STATE só o ponteiro.

Uso:
    python scripts/verificar_state.py            # confere o .ai/STATE.md
    python scripts/verificar_state.py outro.md   # confere outro arquivo

Sai com código 1 se houver problema, listando cada um.
"""

import argparse
import re
import sys
from datetime import date, timedelta
from pathlib import Path

RAIZ = Path(__file__).resolve().parent.parent
LIMITE_LINHAS = 300
DIAS_RESOLVIDO = 31

RE_LINK = re.compile(r"\[[^\]]*\]\(([^)\s]+)\)")
RE_TITULO = re.compile(r"^#{1,6}\s+(.+?)\s*#*\s*$")
RE_ENTRADA = re.compile(r"^- (\d{4}-\d{2}-\d{2}):")


def ancora(titulo):
    """Converte um título na âncora que o GitHub gera para ele."""
    texto = titulo.strip().lower()
    texto = re.sub(r"[^\w\- ]", "", texto)
    return texto.replace(" ", "-")


def ancoras_do_arquivo(caminho):
    """Devolve o conjunto de âncoras dos títulos de um arquivo Markdown."""
    ancoras = set()
    for linha in Path(caminho).read_text(encoding="utf-8").splitlines():
        m = RE_TITULO.match(linha)
        if m:
            ancoras.add(ancora(m.group(1)))
    return ancoras


def links_quebrados(caminho):
    """Lista os links relativos de ``caminho`` que não levam a lugar nenhum."""
    caminho = Path(caminho)
    problemas = []
    texto = caminho.read_text(encoding="utf-8")
    for alvo in RE_LINK.findall(texto):
        if re.match(r"^[a-z]+:", alvo):  # http:, https:, mailto:
            continue
        arquivo, _, frag = alvo.partition("#")
        destino = (caminho.parent / arquivo).resolve() if arquivo else caminho
        if not destino.exists():
            problemas.append(f"link para arquivo inexistente: {alvo}")
        elif frag and destino.suffix == ".md" and frag not in ancoras_do_arquivo(destino):
            problemas.append(f"link para título inexistente: {alvo}")
    return problemas


def resolvidos_antigos(texto, dias=DIAS_RESOLVIDO):
    """Lista as entradas de "Resolvido" mais velhas que ``dias``."""
    entradas = []
    na_secao = False
    for linha in texto.splitlines():
        if linha.startswith("## "):
            na_secao = linha.startswith("## Resolvido")
        elif na_secao:
            m = RE_ENTRADA.match(linha)
            if m:
                entradas.append(date.fromisoformat(m.group(1)))
    if not entradas:
        return []
    corte = max(entradas) - timedelta(days=dias)
    return [f"entrada de {d.isoformat()} em Resolvido tem mais de {dias} dias: "
            "apague (o histórico fica no git log)"
            for d in entradas if d < corte]


def tamanho_excedido(texto, limite=LIMITE_LINHAS):
    """Avisa se o texto passou de ``limite`` linhas."""
    n = len(texto.splitlines())
    if n <= limite:
        return []
    return [f"{n} linhas (limite {limite}): mova o detalhamento para arquivos "
            "por tópico e deixe no STATE só o ponteiro"]


def verificar(caminho):
    """Roda todas as checagens e devolve a lista de problemas."""
    texto = Path(caminho).read_text(encoding="utf-8")
    return (links_quebrados(caminho) + resolvidos_antigos(texto)
            + tamanho_excedido(texto))


def main(argv=None):
    parser = argparse.ArgumentParser(
        description="Confere links, a seção Resolvido e o tamanho do .ai/STATE.md.")
    parser.add_argument("arquivo", nargs="?", default=str(RAIZ / ".ai" / "STATE.md"),
                        help="arquivo a conferir (padrão: .ai/STATE.md)")
    args = parser.parse_args(argv)
    problemas = verificar(args.arquivo)
    for p in problemas:
        print(f"- {p}")
    if not problemas:
        print("STATE ok")
    return 1 if problemas else 0


if __name__ == "__main__":
    sys.exit(main())
