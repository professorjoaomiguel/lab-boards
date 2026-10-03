# Autor: Prof. Joao Miguel Roehe (@professorjoaomiguel)
# SPDX-License-Identifier: MIT
"""Testes de scripts/serial_placa.py (sem precisar de placa ligada).

Rodar com: python -m unittest scripts/test_serial_placa.py -v
"""

import contextlib
import csv
import io
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



class FalsoProcesso:
    """Resposta falsa de subprocess.run para os testes de verificar_ambiente."""

    def __init__(self, returncode=0, stdout=""):
        self.returncode = returncode
        self.stdout = stdout
        self.stderr = ""


def executor(cli_existe=True, cores=("arduino:renesas_uno", "arduino:avr")):
    """Cria um subprocess.run falso para o arduino-cli."""
    def executar(comando, **_kwargs):
        if not cli_existe:
            raise FileNotFoundError("arduino-cli")
        if comando[1] == "version":
            return FalsoProcesso(stdout="arduino-cli  Version: 1.2.0")
        lista = [{"id": c} for c in cores]
        import json
        return FalsoProcesso(stdout=json.dumps({"platforms": lista}))
    return executar


def importador(pyserial_existe=True):
    def importar(nome):
        if not pyserial_existe:
            raise ImportError(nome)
        class Modulo:
            VERSION = "3.5"
        return Modulo
    return importar


class TestVerificarAmbiente(unittest.TestCase):
    def estados(self, itens):
        return {item: estado for item, estado, _ in itens}

    def test_tudo_instalado(self):
        itens = sp.verificar_ambiente(executor(), importador())
        estados = self.estados(itens)
        self.assertEqual(estados["python"], "OK")
        self.assertEqual(estados["pyserial"], "OK")
        self.assertEqual(estados["arduino-cli"], "OK")
        self.assertEqual(estados["pacote arduino:renesas_uno"], "OK")

    def test_sem_pyserial_e_falha(self):
        itens = sp.verificar_ambiente(executor(), importador(False))
        estado, detalhe = [(e, d) for i, e, d in itens if i == "pyserial"][0]
        self.assertEqual(estado, "FALHA")
        self.assertIn("pip install pyserial", detalhe)

    def test_sem_arduino_cli_e_so_aviso(self):
        # O arduino-cli só é preciso para --gravar.
        itens = sp.verificar_ambiente(executor(cli_existe=False), importador())
        estados = self.estados(itens)
        self.assertEqual(estados["arduino-cli"], "AVISO")
        self.assertNotIn("pacote arduino:renesas_uno", estados)

    def test_pacote_da_placa_faltando(self):
        itens = sp.verificar_ambiente(executor(cores=("arduino:avr",)), importador())
        estado, detalhe = [(e, d) for i, e, d in itens if i == "pacote arduino:renesas_uno"][0]
        self.assertEqual(estado, "AVISO")
        self.assertIn("arduino-cli core install arduino:renesas_uno", detalhe)



class FalsaConexao:
    """Porta serial falsa: devolve linhas prontas e guarda o que foi escrito."""

    def __init__(self, linhas):
        self.linhas = [l.encode("utf-8") for l in linhas]
        self.escrito = b""

    def readline(self):
        return self.linhas.pop(0) if self.linhas else b""

    def write(self, dados):
        self.escrito += dados


class TestExecutarAuto(unittest.TestCase):
    def test_comando_e_marcador_do_sketch_do_shield(self):
        # O sketch conjunto (placa + shield) mostra um menu e roda o teste
        # automático com a opção "a".
        conexao = FalsaConexao([
            "Placa: UNO R3, ATmega328P, Vcc 4871 mV\n",
            "Digite a opção e envie:\n",
            "FIM;ok=0;falha=0;aviso=0;pulado=0\n",
        ])
        with contextlib.redirect_stdout(io.StringIO()):
            sp.executar_auto(conexao, 5, comando=b"a\n", marcador="Digite a opção")
        self.assertEqual(conexao.escrito, b"a\n")

    def test_so_manda_comecar_depois_das_boas_vindas(self):
        # No UNO R3, o que chega antes das boas-vindas cai no bootloader.
        conexao = FalsaConexao(["# reiniciando\n"])
        escrito_antes = []
        original = conexao.readline

        def readline():
            escrito_antes.append(conexao.escrito)
            return original()
        conexao.readline = readline
        conexao.linhas += ["> Envie c para começar\n".encode("utf-8"),
                           b"FIM;ok=0;falha=0;aviso=0;pulado=0\n"]
        with contextlib.redirect_stdout(io.StringIO()):
            sp.executar_auto(conexao, 5)
        self.assertEqual(escrito_antes[:2], [b"", b""])
        self.assertEqual(conexao.escrito, b"c\n")

    def test_manda_comecar_e_le_ate_o_fim(self):
        conexao = FalsaConexao([
            "> Envie c para começar\n",
            "INICIO;teste_uno_r4_automatico;2\n",
            "ID_UNICO;ABC\n",
            "RESULTADO;clock;OK;48 MHz\n",
            "  [  OK  ] Clock do processador ..... 48 MHz\n",
            "FIM;ok=1;falha=0;aviso=0;pulado=0\n",
            "RESULTADO;depois_do_fim;OK;nao deve ser lido\n",
        ])
        with contextlib.redirect_stdout(io.StringIO()):
            resultados, fim, id_unico = sp.executar_auto(conexao, 5)
        # O sketch espera o comando "c" antes de testar (seguro na IDE).
        self.assertEqual(conexao.escrito, b"c\n")
        self.assertEqual([r["teste"] for r in resultados], ["clock"])
        self.assertEqual(fim["ok"], 1)
        self.assertEqual(id_unico, "ABC")


if __name__ == "__main__":
    unittest.main()
