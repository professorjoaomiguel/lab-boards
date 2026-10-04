# Autor: Prof. Me. Joao Miguel Lac Roehe (@professorjoaomiguel)
# SPDX-License-Identifier: MIT
"""Testes de scripts/gerar_inventario.py.

Rodar com: python -m unittest tests/test_gerar_inventario.py -v
"""

import json
import os
import re
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), "scripts"))

import gerar_inventario as gi  # noqa: E402


def escrever(pasta, nome, texto):
    (Path(pasta) / nome).write_text(texto, encoding="utf-8")


class TestSituacao(unittest.TestCase):
    def test_resumo_do_teste(self):
        self.assertEqual(gi.situacao("ok=5 falha=0 aviso=0 pulado=0"), "ok")
        self.assertEqual(gi.situacao("ok=4 falha=1 aviso=0 pulado=0"), "com falha")
        self.assertEqual(gi.situacao("ok=7 falha=0 aviso=1 pulado=0"), "atenção")

    def test_texto_livre_dos_shields(self):
        self.assertEqual(gi.situacao("tudo OK"), "ok")
        self.assertEqual(gi.situacao("LM35 instável"), "atenção")
        self.assertEqual(gi.situacao("a confirmar"), "atenção")
        self.assertEqual(gi.situacao("não testado"), "não testado")
        self.assertEqual(gi.situacao(""), "não testado")


class TestLerInventarios(unittest.TestCase):
    def setUp(self):
        self.pasta = tempfile.mkdtemp()
        escrever(self.pasta, "arduino-uno-r3.csv",
                 "etiqueta,etiqueta_colada,modelo,variante,dono,id_unico,registrado_em,"
                 "ultimo_teste,resultado,obs\n"
                 "R3-01,não,UNO R3,ATmega16U2,SENAI,ABC,2026-10-03,2026-10-03,"
                 "ok=4 falha=1 aviso=0 pulado=0,D3 preso\n")
        escrever(self.pasta, "esp32-s3-uno.csv",
                 "etiqueta,etiqueta_colada,dono,mac,unique_id_128,registrado_em,obs\n"
                 "1e:20,sim,professor,e0:72:a1:d4:1e:20,463d,2026-09-29,PSRAM ok\n")
        escrever(self.pasta, "shield-9em1.csv",
                 "etiqueta,etiqueta_colada,dono,como_reconhecer,testado_em,resultado,obs\n"
                 "S9-03,não,a confirmar,,2026-10-03,tudo OK,LM35 estável\n")

    def test_le_todos_os_arquivos_com_campos_comuns(self):
        itens = gi.ler_inventarios(self.pasta)
        self.assertEqual(len(itens), 3)
        por_etiqueta = {i["etiqueta"]: i for i in itens}

        r3 = por_etiqueta["R3-01"]
        self.assertEqual(r3["tipo"], "Arduino UNO R3")
        self.assertEqual(r3["identificador"], "ABC")
        self.assertEqual(r3["situacao"], "com falha")
        self.assertEqual(r3["dono"], "SENAI")

        s3 = por_etiqueta["1e:20"]
        self.assertEqual(s3["tipo"], "ESP32-S3 UNO")
        self.assertEqual(s3["identificador"], "e0:72:a1:d4:1e:20")
        self.assertEqual(s3["etiqueta_colada"], "sim")
        # Colunas próprias do arquivo vão para "detalhes".
        self.assertEqual(s3["detalhes"]["unique_id_128"], "463d")

        shield = por_etiqueta["S9-03"]
        self.assertEqual(shield["tipo"], "Shield 9 em 1")
        self.assertEqual(shield["situacao"], "ok")
        self.assertEqual(shield["ultimo_teste"], "2026-10-03")

    def test_ignora_arquivos_que_nao_sao_csv(self):
        escrever(self.pasta, "README.md", "# nada")
        self.assertEqual(len(gi.ler_inventarios(self.pasta)), 3)

    def test_arquivo_desconhecido_usa_o_nome_do_arquivo_como_tipo(self):
        escrever(self.pasta, "placa-nova.csv", "etiqueta,dono\nX-01,SENAI\n")
        itens = {i["etiqueta"]: i for i in gi.ler_inventarios(self.pasta)}
        self.assertEqual(itens["X-01"]["tipo"], "placa-nova")


class TestGerarHtml(unittest.TestCase):
    def test_dados_embutidos_e_escapados(self):
        itens = [{"tipo": "Arduino UNO R3", "etiqueta": "R3-01",
                  "obs": "cuidado </script><b>", "situacao": "ok"}]
        html = gi.gerar_html(itens, "2026-10-03")
        self.assertIn("<!doctype html>", html.lower())
        self.assertIn("2026-10-03", html)
        # O texto do CSV não pode fechar o <script> dos dados.
        self.assertNotIn("</script><b>", html)
        inicio = html.index('id="dados"')
        inicio = html.index(">", inicio) + 1
        fim = html.index("</script>", inicio)
        dados = json.loads(html[inicio:fim].replace("<\\/", "</"))
        self.assertEqual(dados[0]["obs"], "cuidado </script><b>")

    def test_link_para_o_site_do_professor(self):
        html = gi.gerar_html([], "2026-10-03")
        self.assertIn('<a class="topbar-back" href="https://professorjoaomiguel.github.io/">', html)

    def test_referencia_visual_central(self):
        # Mesmos arquivos e mesma barra do resto do site (DESIGN.md do site
        # do professor), com links absolutos: o relatório também abre do disco.
        html = gi.gerar_html([], "2026-10-03")
        for trecho in ('<link rel="stylesheet" href="/assets/tokens.css">',
                       '<link rel="stylesheet" href="/assets/topbar.css">',
                       '<link rel="icon" type="image/png" href="/assets/avatar.png">',
                       '<nav class="topbar"', '<div class="topbar-inner">',
                       '<a class="brand" href="https://professorjoaomiguel.github.io/lab-boards/">',
                       '<ul class="topnav">',
                       'href="https://professorjoaomiguel.github.io/lab-boards/INDEX.html"',
                       'href="https://professorjoaomiguel.github.io/lab-boards/IDENTIFICAR.html"',
                       'href="https://professorjoaomiguel.github.io/lab-boards/GLOSSARIO.html"'):
            self.assertIn(trecho, html)

    def test_cores_e_fontes_centrais_tem_reserva_para_offline(self):
        # Sem o tokens.css (arquivo aberto do disco), todo var() de token
        # central precisa de um valor de reserva.
        estilo = gi.gerar_html([], "2026-10-03").split("<style>")[1].split("</style>")[0]
        centrais = ("--bg-primary", "--bg-secondary", "--text-primary", "--text-secondary",
                    "--border", "--accent", "--font", "--mono", "--gutter")
        usos = re.findall(r"var\((--[\w-]+)(,?)", estilo)
        sem_reserva = [nome for nome, virgula in usos if nome in centrais and not virgula]
        self.assertEqual(sem_reserva, [])
        self.assertIn("var(--font, system-ui", estilo)  # Outfit no site, do sistema offline


class TestMain(unittest.TestCase):
    def test_gera_o_arquivo(self):
        pasta = tempfile.mkdtemp()
        escrever(pasta, "shield-9em1.csv", "etiqueta,dono,resultado\nS9-01,SENAI,tudo OK\n")
        saida = Path(pasta) / "relatorio.html"
        self.assertEqual(gi.main(["--pasta", pasta, "--saida", str(saida)]), 0)
        self.assertIn("S9-01", saida.read_text(encoding="utf-8"))


if __name__ == "__main__":
    unittest.main()
