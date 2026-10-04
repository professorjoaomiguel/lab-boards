# Autor: Prof. Me. Joao Miguel Lac Roehe (@professorjoaomiguel)
# SPDX-License-Identifier: MIT
"""Testes de scripts/verificar_site.py (sem internet: o site é falso).

Rodar com: python -m unittest scripts/test_verificar_site.py -v
"""

import contextlib
import io
import os
import sys
import unittest

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import verificar_site as vsite  # noqa: E402

BASE = "https://exemplo.github.io/lab/"


def site_falso(paginas):
    """Devolve uma função `buscar(url)` que serve o dicionário `paginas`."""
    def buscar(url):
        if url in paginas:
            corpo, tipo = paginas[url]
            return 200, corpo, tipo
        return 404, "", ""
    return buscar


class TestExtrair(unittest.TestCase):
    def test_ids_e_links_internos(self):
        html = ('<h2 id="tens&#227;o">T</h2><h3 id="x">X</h3>'
                '<a href="a.html">a</a> <a href="../b.html#tens%C3%A3o">b</a>'
                '<a href="https://outro.site/x">fora</a> <a href="#x">aqui</a>')
        ids, links = vsite.extrair(html, BASE + "pasta/p.html", BASE)
        self.assertEqual(ids, {"tensão", "x"})
        self.assertIn((BASE + "pasta/a.html", ""), links)
        self.assertIn((BASE + "b.html", "tensão"), links)
        self.assertIn((BASE + "pasta/p.html", "x"), links)
        self.assertEqual(len(links), 3)  # o link externo fica de fora


class TestVarrer(unittest.TestCase):
    def test_site_sem_problemas(self):
        buscar = site_falso({
            BASE: ('<a href="a.html#s">a</a>', "text/html"),
            BASE + "a.html": ('<h2 id="s">S</h2><a href="./">home</a>', "text/html"),
        })
        resultado = vsite.varrer(BASE, [""], buscar)
        self.assertEqual(vsite.problemas(resultado, BASE), [])
        self.assertEqual(resultado.paginas_visitadas, 2)

    def test_acha_404_ancora_e_md_cru(self):
        buscar = site_falso({
            BASE: ('<a href="a.html">a</a><a href="sumiu.html">x</a>'
                   '<a href="CONTRIBUTING.md">c</a>', "text/html"),
            BASE + "a.html": ('<a href="./#nao-existe">h</a>', "text/html"),
            BASE + "CONTRIBUTING.md": ("# texto", "text/markdown"),
        })
        problemas = vsite.problemas(vsite.varrer(BASE, [""], buscar), BASE)
        self.assertEqual(len(problemas), 3)
        texto = "\n".join(problemas)
        self.assertIn("404 sumiu.html", texto)
        self.assertIn("#nao-existe", texto)
        self.assertIn("CONTRIBUTING.md", texto)

    def test_arquivo_que_nao_e_html_nao_e_varrido(self):
        buscar = site_falso({
            BASE: ('<a href="dados.csv">csv</a>', "text/html"),
            BASE + "dados.csv": ('<a href="nao-segue.html">x</a>', "text/csv"),
        })
        self.assertEqual(vsite.problemas(vsite.varrer(BASE, [""], buscar), BASE), [])


class TestLinhaDeComando(unittest.TestCase):
    def test_help(self):
        saida = io.StringIO()
        with contextlib.redirect_stdout(saida):
            with self.assertRaises(SystemExit) as ctx:
                vsite.main(["--help"])
        self.assertEqual(ctx.exception.code, 0)
        self.assertIn("--base", saida.getvalue())


if __name__ == "__main__":
    unittest.main()
