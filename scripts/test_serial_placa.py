# Autor: Prof. Joao Miguel Roehe (@professorjoaomiguel)
# SPDX-License-Identifier: MIT
"""Testes de scripts/serial_placa.py (sem precisar de placa ligada).

Rodar com: python -m unittest scripts/test_serial_placa.py -v
"""

import csv
import os
import sys
import tempfile
import unittest

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import serial_placa as sp  # noqa: E402


class TestAnalisarLinha(unittest.TestCase):
    def test_resultado(self):
        r = sp.analisar_linha("RESULTADO;dht11;OK;26.9 °C, 52 %\r\n")
        self.assertEqual(r, {"tipo": "resultado", "teste": "dht11",
                             "estado": "OK", "detalhe": "26.9 °C, 52 %"})

    def test_resultado_com_ponto_e_virgula_no_detalhe(self):
        r = sp.analisar_linha("RESULTADO;x;AVISO;a;b")
        self.assertEqual(r["detalhe"], "a;b")

    def test_fim(self):
        r = sp.analisar_linha("FIM;ok=10;falha=0;aviso=2;pulado=3")
        self.assertEqual(r, {"tipo": "fim", "ok": 10, "falha": 0,
                             "aviso": 2, "pulado": 3})

    def test_id_unico(self):
        r = sp.analisar_linha("ID_UNICO;32151A295A323839538E33354B573459")
        self.assertEqual(r, {"tipo": "id_unico",
                             "id": "32151A295A323839538E33354B573459"})

    def test_inicio(self):
        r = sp.analisar_linha("INICIO;teste_uno_r4_automatico;1")
        self.assertEqual(r, {"tipo": "inicio", "sketch": "teste_uno_r4_automatico",
                             "versao": "1"})

    def test_comentario_e_lixo_sao_ignorados(self):
        self.assertIsNone(sp.analisar_linha("# Relógio"))
        self.assertIsNone(sp.analisar_linha(""))
        self.assertIsNone(sp.analisar_linha("RESULTADO;incompleto"))
        self.assertIsNone(sp.analisar_linha("FIM;ok=x"))


class TestIdentificarPlaca(unittest.TestCase):
    def test_r4_minima(self):
        self.assertEqual(sp.identificar_placa(0x2341, 0x0069), ["uno-r4-minima"])

    def test_r3_original(self):
        self.assertEqual(sp.identificar_placa(0x2341, 0x0043), ["uno-r3"])

    def test_ch340_e_ambiguo(self):
        # O CH340 aparece em clones do UNO R3 e na ESP32-S3 UNO.
        self.assertEqual(sp.identificar_placa(0x1A86, 0x7523), ["uno-r3"])

    def test_desconhecida(self):
        self.assertEqual(sp.identificar_placa(None, None), [])
        self.assertEqual(sp.identificar_placa(0x1234, 0x5678), [])


def porta(dispositivo, vid=None, pid=None, serie=None):
    return sp.Porta(dispositivo, vid, pid, serie, "teste")


class TestEscolherPorta(unittest.TestCase):
    def setUp(self):
        self.r4 = porta("COM8", 0x2341, 0x0069, "ABC")
        self.bt = porta("COM4")

    def test_uma_placa_conhecida(self):
        p, chave = sp.escolher_porta([self.bt, self.r4])
        self.assertEqual((p.dispositivo, chave), ("COM8", "uno-r4-minima"))

    def test_porta_indicada(self):
        p, chave = sp.escolher_porta([self.bt, self.r4], porta="com8")
        self.assertEqual((p.dispositivo, chave), ("COM8", "uno-r4-minima"))

    def test_placa_indicada_vence_a_deteccao(self):
        p, chave = sp.escolher_porta([self.r4], porta="COM8", placa="uno-r4-wifi")
        self.assertEqual(chave, "uno-r4-wifi")

    def test_nenhuma(self):
        with self.assertRaises(sp.ErroPorta):
            sp.escolher_porta([self.bt])

    def test_duas_placas_exige_porta(self):
        outra = porta("COM9", 0x2341, 0x0069, "DEF")
        with self.assertRaises(sp.ErroPorta):
            sp.escolher_porta([self.r4, outra])

    def test_porta_inexistente(self):
        with self.assertRaises(sp.ErroPorta):
            sp.escolher_porta([self.r4], porta="COM99")

    def test_porta_sem_placa_reconhecida(self):
        with self.assertRaises(sp.ErroPorta):
            sp.escolher_porta([self.bt], porta="COM4")


class TestInventario(unittest.TestCase):
    def setUp(self):
        self.pasta = tempfile.mkdtemp()
        self.csv = os.path.join(self.pasta, "inventario.csv")
        with open(self.csv, "w", newline="", encoding="utf-8") as f:
            f.write(",".join(sp.COLUNAS_INVENTARIO) + "\n")

    def ler(self):
        with open(self.csv, newline="", encoding="utf-8") as f:
            return list(csv.DictReader(f))

    def test_registra_placa_nova(self):
        etiqueta, nova = sp.atualizar_inventario(
            self.csv, "AAA", "UNO R4 Minima", "ok=10 falha=0", "2026-10-03")
        self.assertEqual((etiqueta, nova), ("R4M-01", True))
        linhas = self.ler()
        self.assertEqual(len(linhas), 1)
        self.assertEqual(linhas[0]["id_unico"], "AAA")
        self.assertEqual(linhas[0]["registrado_em"], "2026-10-03")
        self.assertEqual(linhas[0]["ultimo_teste"], "2026-10-03")

    def test_numera_em_sequencia_por_modelo(self):
        sp.atualizar_inventario(self.csv, "AAA", "UNO R4 Minima", "", "2026-10-03")
        sp.atualizar_inventario(self.csv, "BBB", "UNO R4 WiFi", "", "2026-10-03")
        etiqueta, _ = sp.atualizar_inventario(self.csv, "CCC", "UNO R4 Minima", "", "2026-10-03")
        self.assertEqual(etiqueta, "R4M-02")
        self.assertEqual(self.ler()[1]["etiqueta"], "R4W-01")

    def test_placa_ja_registrada_so_atualiza_o_teste(self):
        sp.atualizar_inventario(self.csv, "AAA", "UNO R4 Minima", "ok=1", "2026-10-03")
        etiqueta, nova = sp.atualizar_inventario(
            self.csv, "AAA", "UNO R4 Minima", "ok=2", "2026-11-01")
        self.assertEqual((etiqueta, nova), ("R4M-01", False))
        linhas = self.ler()
        self.assertEqual(len(linhas), 1)
        self.assertEqual(linhas[0]["registrado_em"], "2026-10-03")
        self.assertEqual(linhas[0]["ultimo_teste"], "2026-11-01")
        self.assertEqual(linhas[0]["resultado"], "ok=2")

    def test_preserva_observacao_escrita_a_mao(self):
        sp.atualizar_inventario(self.csv, "AAA", "UNO R4 Minima", "", "2026-10-03")
        linhas = self.ler()
        linhas[0]["obs"] = "pino D7 torto"
        with open(self.csv, "w", newline="", encoding="utf-8") as f:
            w = csv.DictWriter(f, fieldnames=sp.COLUNAS_INVENTARIO, lineterminator="\n")
            w.writeheader()
            w.writerows(linhas)
        sp.atualizar_inventario(self.csv, "AAA", "UNO R4 Minima", "ok=3", "2026-10-04")
        self.assertEqual(self.ler()[0]["obs"], "pino D7 torto")


class TestResumo(unittest.TestCase):
    def test_conta_estados(self):
        resultados = [{"estado": e} for e in ["OK", "OK", "FALHA", "INFO", "PULADO"]]
        self.assertEqual(sp.resumir(resultados),
                         {"ok": 2, "falha": 1, "aviso": 0, "pulado": 1})

    def test_texto_do_resumo(self):
        self.assertEqual(sp.texto_resumo({"ok": 2, "falha": 0, "aviso": 1, "pulado": 0}),
                         "ok=2 falha=0 aviso=1 pulado=0")


if __name__ == "__main__":
    unittest.main()
