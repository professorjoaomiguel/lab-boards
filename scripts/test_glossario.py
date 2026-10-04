# Autor: Prof. Me. Joao Miguel Lac Roehe (@professorjoaomiguel)
# SPDX-License-Identifier: MIT
"""Testes do GLOSSARIO.md e dos links que apontam para ele.

Rodar com: python -m unittest scripts/test_glossario.py -v
"""

import os
import subprocess
import sys
import unittest
from pathlib import Path

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import verificar_state as vs  # noqa: E402

RAIZ = Path(__file__).resolve().parent.parent
GLOSSARIO = RAIZ / "GLOSSARIO.md"


def termos(texto):
    """Títulos ``###`` do glossário, na ordem em que aparecem."""
    return [linha[4:].strip() for linha in texto.splitlines()
            if linha.startswith("### ")]


def chave_alfabetica(termo):
    """Chave de ordenação sem acento e sem diferença de maiúsculas."""
    import unicodedata
    sem_acento = unicodedata.normalize("NFKD", termo)
    sem_acento = "".join(c for c in sem_acento if not unicodedata.combining(c))
    return sem_acento.casefold()


def arquivos_md_do_repositorio():
    """Arquivos .md versionados, sem docs/ (histórico) e templates/ (modelos com links de exemplo)."""
    saida = subprocess.run(["git", "ls-files", "*.md"], cwd=RAIZ,
                           capture_output=True, text=True, check=True).stdout
    return [RAIZ / f for f in saida.split() if not f.startswith(("docs/", "templates/"))]


class TestFuncoesAuxiliares(unittest.TestCase):
    def test_termos(self):
        self.assertEqual(termos("# G\n\n## A\n\n### ADC\n\ntexto\n\n### AREF\n"),
                         ["ADC", "AREF"])

    def test_chave_ignora_acento_e_caixa(self):
        self.assertEqual(chave_alfabetica("Ânodo comum"), "anodo comum")
        self.assertLess(chave_alfabetica("baud"), chave_alfabetica("Bootloader"))


class TestGlossario(unittest.TestCase):
    def setUp(self):
        self.texto = GLOSSARIO.read_text(encoding="utf-8")
        self.termos = termos(self.texto)

    def test_tem_termos(self):
        self.assertGreater(len(self.termos), 20)

    def test_ordem_alfabetica(self):
        self.assertEqual(self.termos, sorted(self.termos, key=chave_alfabetica))

    def test_sem_termos_repetidos(self):
        ancoras = [vs.ancora(t) for t in self.termos]
        self.assertEqual(len(ancoras), len(set(ancoras)))

    def test_links_do_glossario(self):
        self.assertEqual(vs.links_quebrados(GLOSSARIO), [])


class TestLinksDoRepositorio(unittest.TestCase):
    """Links relativos de todos os .md, inclusive os que apontam para o glossário."""

    def test_nenhum_link_quebrado(self):
        problemas = []
        for arquivo in arquivos_md_do_repositorio():
            problemas += [f"{arquivo.relative_to(RAIZ)}: {p}"
                          for p in vs.links_quebrados(arquivo)]
        self.assertEqual(problemas, [])


class TestPages(unittest.TestCase):
    """O GitHub Pages passa os .md pelo Liquid: `{{` e `{%` quebram a página."""

    def test_nenhum_md_tem_sintaxe_liquid(self):
        com_liquid = [str(a.relative_to(RAIZ)) for a in arquivos_md_do_repositorio()
                      if "{{" in a.read_text(encoding="utf-8")
                      or "{%" in a.read_text(encoding="utf-8")]
        self.assertEqual(com_liquid, [])

    def test_readme_e_contributing_publicados_pelos_html(self):
        # O Jekyll do Pages não transforma estes .md em página, e front
        # matter neles aparece no GitHub (como linhas horizontais). Por isso
        # ficam sem front matter, e um .html de mesmo papel inclui o texto.
        for md, pagina in (("README.md", "index.html"),
                           ("CONTRIBUTING.md", "CONTRIBUTING.html")):
            self.assertFalse((RAIZ / md).read_text(encoding="utf-8").startswith("---"), md)
            self.assertIn(f"include_relative {md}",
                          (RAIZ / pagina).read_text(encoding="utf-8"), pagina)

    def test_pasta_ai_e_publicada(self):
        config = (RAIZ / "_config.yml").read_text(encoding="utf-8")
        self.assertIn("include:\n  - .ai", config)


if __name__ == "__main__":
    unittest.main()
