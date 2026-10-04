# Autor: Prof. Me. Joao Miguel Lac Roehe (@professorjoaomiguel)
# SPDX-License-Identifier: MIT
"""Confere os links do site publicado no GitHub Pages.

O QUE FAZ
    Percorre o site a partir da home e das páginas principais, seguindo os
    links internos, e aponta:
      - página que não abre (404 ou outro erro);
      - link com #âncora para um título que não existe na página;
      - link que leva a um arquivo .md cru (no site, o certo é a página);
      - arquivo da referência visual compartilhada (/assets/ do site do
        professor: tokens.css, topbar.css, avatar.png) que não abre. Ele
        fica fora do Lab Boards: o script só confere que dá 200, sem
        percorrer.

    Os testes do repositório conferem os links como funcionam no GitHub. O
    site é gerado pelo Jekyll a cada push, e a home e o CONTRIBUTING passam
    por um ajuste de links (_includes/markdown-da-raiz.html): um link que
    funciona no GitHub pode dar 404 no site. Este script pega isso.

COMO USAR
    Na raiz do repositório, depois que o build do Pages terminar:

        python scripts/verificar_site.py
        python scripts/verificar_site.py --base https://outro.endereco/lab-boards/

    Sai com código 1 se achar problema, listando cada um. Precisa de
    internet; por isso não roda na suíte de testes. A CI roda o script depois
    de cada build do Pages (.github/workflows/site.yml). À mão, rode depois
    que o build terminar, quando mudar o README, o CONTRIBUTING, o layout
    (_layouts/, _includes/) ou o _config.yml.

TESTES
    python -m unittest tests/test_verificar_site.py -v   (sem internet)
"""

import argparse
import html
import re
import sys
import time
import urllib.error
import urllib.parse
import urllib.request
from dataclasses import dataclass, field

BASE_PADRAO = "https://professorjoaomiguel.github.io/lab-boards/"
INICIO = ["", "INDEX.html", "GLOSSARIO.html", "IDENTIFICAR.html",
          "CONTRIBUTING.html", "AGENTS.html", "inventario/",
          ".ai/CONVENTIONS.html", ".ai/STATE.html"]

RE_ID = re.compile(r'\bid="([^"]+)"')
RE_HREF = re.compile(r'\bhref="([^"]+)"')
RE_SRC = re.compile(r'\bsrc="([^"]+)"')


def prefixo_compartilhado(base):
    """Pasta da referência visual do site do professor (mesmo domínio)."""
    return urllib.parse.urljoin(base, "/assets/")


@dataclass
class Resultado:
    """O que a varredura encontrou."""
    status: dict = field(default_factory=dict)  # url -> código HTTP
    ids: dict = field(default_factory=dict)     # url -> conjunto de ids
    links: list = field(default_factory=list)   # (origem, destino, âncora)
    paginas_visitadas: int = 0


def extrair(texto_html, url_pagina, base):
    """Devolve (ids, links) de uma página.

    Entram os href que ficam dentro de `base` e os href e src da referência
    visual compartilhada (`/assets/` do mesmo domínio, no site do
    professor). Cada link é (url_sem_âncora, âncora), com a âncora já
    decodificada.
    """
    compartilhado = prefixo_compartilhado(base)
    ids = {html.unescape(i) for i in RE_ID.findall(texto_html)}
    candidatos = [(h, True) for h in RE_HREF.findall(texto_html)]
    candidatos += [(s, False) for s in RE_SRC.findall(texto_html)]
    links = []
    for bruto, eh_href in candidatos:
        alvo = urllib.parse.urljoin(url_pagina, html.unescape(bruto))
        interno = eh_href and alvo.startswith(base)
        if not (interno or alvo.startswith(compartilhado)):
            continue
        pagina, _, ancora = alvo.partition("#")
        links.append((pagina, urllib.parse.unquote(ancora)))
    return ids, links


def buscar_na_web(url):
    """Baixa `url` sem cache; devolve (status, corpo, tipo de conteúdo)."""
    separador = "&" if "?" in url else "?"
    try:
        with urllib.request.urlopen(f"{url}{separador}nocache={time.time()}",
                                    timeout=30) as r:
            return r.status, r.read().decode("utf-8", "replace"), r.headers.get_content_type()
    except urllib.error.HTTPError as e:
        return e.code, "", ""
    except urllib.error.URLError as e:
        return f"erro ({e.reason})", "", ""


def varrer(base, inicio, buscar=buscar_na_web):
    """Percorre o site a partir das páginas `inicio` (relativas a `base`)."""
    r = Resultado()
    fila = [base + p for p in inicio]
    while fila:
        url = fila.pop()
        if url in r.status:
            continue
        status, corpo, tipo = buscar(url)
        r.status[url] = status
        if status != 200 or tipo != "text/html":
            continue
        r.paginas_visitadas += 1
        ids, links = extrair(corpo, url, base)
        r.ids[url] = ids
        for destino, ancora in links:
            r.links.append((url, destino, ancora))
            if destino not in r.status and destino.endswith((".html", "/")):
                fila.append(destino)
    # Destinos que não são página (.csv, .py, .md...): só confere se abrem.
    for _, destino, _ in r.links:
        if destino not in r.status:
            r.status[destino] = buscar(destino)[0]
    return r


def problemas(r, base):
    """Lista os problemas da varredura, um texto por problema, sem repetir."""
    def curto(url):
        if not url.startswith(base):  # referência compartilhada (/assets/)
            return urllib.parse.urlparse(url).path
        return url[len(base):] or "/"

    achados = set()
    for origem, destino, ancora in r.links:
        de = f"(em {curto(origem)})"
        if r.status.get(destino) != 200:
            achados.add(f"{r.status.get(destino)} {curto(destino)} {de}")
        elif destino.endswith(".md"):
            achados.add(f"link para .md cru: {curto(destino)} {de}")
        elif ancora and destino in r.ids and ancora not in r.ids[destino]:
            achados.add(f"âncora inexistente: {curto(destino)}#{ancora} {de}")
    return sorted(achados)


def main(argv=None):
    parser = argparse.ArgumentParser(
        prog="python scripts/verificar_site.py",
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--base", default=BASE_PADRAO,
                        help=f"endereço do site (padrão: {BASE_PADRAO})")
    args = parser.parse_args(argv)
    base = args.base if args.base.endswith("/") else args.base + "/"

    r = varrer(base, INICIO)
    achados = problemas(r, base)
    print(f"{r.paginas_visitadas} páginas, {len(r.links)} links, "
          f"{len(achados)} problema(s).")
    for a in achados:
        print(f"- {a}")
    return 1 if achados else 0


if __name__ == "__main__":
    sys.exit(main())
