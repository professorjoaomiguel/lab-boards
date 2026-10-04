# Autor: Prof. Joao Miguel Roehe (@professorjoaomiguel)
# SPDX-License-Identifier: MIT
"""Testes de scripts/verificar_state.py.

Rodar com: python -m unittest scripts/test_verificar_state.py -v
"""

import os
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import verificar_state as vs  # noqa: E402

RAIZ = Path(__file__).resolve().parent.parent


def escrever(pasta, nome, texto):
    caminho = Path(pasta) / nome
    caminho.parent.mkdir(parents=True, exist_ok=True)
    caminho.write_text(texto, encoding="utf-8")
    return caminho


class TestAncora(unittest.TestCase):
    def test_regras_do_github(self):
        self.assertEqual(vs.ancora("LM35 instável (2026-10-03)"),
                         "lm35-instável-2026-10-03")
        self.assertEqual(
            vs.ancora("ADC: leitura errada de sensores que não absorvem corrente"),
            "adc-leitura-errada-de-sensores-que-não-absorvem-corrente")
        self.assertEqual(vs.ancora("Código de teste (`code/`)"),
                         "código-de-teste-code")


class TestLinksQuebrados(unittest.TestCase):
    def setUp(self):
        self.pasta = tempfile.mkdtemp()
        escrever(self.pasta, "item/README.md", "# Item\n\n## Seção boa\n")

    def test_links_validos_passam(self):
        md = escrever(self.pasta, "a/STATE.md",
                      "## Topo\n\n[ok](../item/README.md) "
                      "[sec](../item/README.md#seção-boa) [aqui](#topo) "
                      "[web](https://exemplo.com/x)\n")
        self.assertEqual(vs.links_quebrados(md), [])

    def test_arquivo_inexistente(self):
        md = escrever(self.pasta, "a/STATE.md", "[x](../nao/existe.md)\n")
        problemas = vs.links_quebrados(md)
        self.assertEqual(len(problemas), 1)
        self.assertIn("nao/existe.md", problemas[0])

    def test_ignora_links_dentro_de_codigo(self):
        md = escrever(self.pasta, "a/STATE.md",
                      "Use `![foto](imagens/x.jpg)` aqui.\n\n"
                      "```\n[y](../nao/existe.md)\n```\n")
        self.assertEqual(vs.links_quebrados(md), [])

    def test_ancora_inexistente(self):
        md = escrever(self.pasta, "a/STATE.md",
                      "[x](../item/README.md#seção-ruim) [y](#sumiu)\n")
        self.assertEqual(len(vs.links_quebrados(md)), 2)


class TestResolvidosAntigos(unittest.TestCase):
    TEXTO = ("## Resolvido\n\n"
             "- 2026-10-04: novo\n"
             "- 2026-09-10: ainda dentro\n"
             "- 2026-08-01: velho demais\n"
             "\n## Outra\n\n- 2020-01-01: fora da seção\n")

    def test_so_os_velhos_da_secao_resolvido(self):
        problemas = vs.resolvidos_antigos(self.TEXTO, dias=30)
        self.assertEqual(len(problemas), 1)
        self.assertIn("2026-08-01", problemas[0])

    def test_sem_secao_nao_reclama(self):
        self.assertEqual(vs.resolvidos_antigos("# Nada\n", dias=30), [])


class TestTamanho(unittest.TestCase):
    def test_limite_de_linhas(self):
        self.assertEqual(vs.tamanho_excedido("x\n" * 10, limite=10), [])
        self.assertEqual(len(vs.tamanho_excedido("x\n" * 11, limite=10)), 1)


class TestStateReal(unittest.TestCase):
    """O .ai/STATE.md do repositório tem de passar em todas as checagens."""

    def test_state_do_repositorio(self):
        self.assertEqual(vs.verificar(RAIZ / ".ai" / "STATE.md"), [])


if __name__ == "__main__":
    unittest.main()
