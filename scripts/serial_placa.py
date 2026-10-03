# Autor: Prof. Joao Miguel Roehe (@professorjoaomiguel)
# SPDX-License-Identifier: MIT
"""Conversa com as placas do laboratório pela porta serial: lista, testa e registra.

O QUE FAZ
    Ajuda a testar placas Arduino ligadas na USB, sem abrir a IDE:

    verificar   Confere se o computador está pronto: versão do Python,
                pyserial, arduino-cli e pacotes de placa instalados, e
                quais placas estão ligadas. Rode este primeiro.
    listar      Mostra as portas seriais, com VID:PID, número de série USB
                e a placa reconhecida (quando é uma das placas conhecidas).
    auto        Abre a porta, recebe o resultado do sketch de teste
                automático e termina sozinho. Opcionalmente grava o sketch
                antes (--gravar) e registra a placa no inventário
                (--registrar). Código de saída: 0 = sem falhas, 1 = há
                falhas, 2 = erro (porta, gravação, tempo esgotado).
    interativo  Terminal simples: mostra o que a placa envia e manda para
                ela cada linha digitada (com Enter). Serve para o sketch de
                teste com ajuda do usuário. Ctrl+C para sair.

    Os sketches de teste imprimem linhas num formato fixo, separado por ";":
        INICIO;<sketch>;<versão>
        ID_UNICO;<id do chip>
        RESULTADO;<teste>;<OK|FALHA|AVISO|PULADO|INFO>;<detalhe>
        FIM;ok=<n>;falha=<n>;aviso=<n>;pulado=<n>
    Linhas começando com "#" são só explicação para quem lê.

    Placas reconhecidas pela USB (VID:PID):
        2341:0069  UNO R4 Minima   (número de série USB = ID único do RA4M1)
        2341:1002  UNO R4 WiFi     (número de série USB = o do ESP32-S3)
        2341:0043, 2341:0001, 2A03:0043, 1A86:7523 (CH340)  UNO R3
    O CH340 também está na ESP32-S3 UNO; se a detecção errar, use --placa.

COMO USAR
    Preparar o computador (uma vez):
        1. Instale o Python 3.8 ou mais novo (python.org; no Windows, marque
           "Add python.exe to PATH" no instalador).
        2. pip install pyserial
        3. Só para --gravar: instale o arduino-cli e o pacote da placa
           (ex: arduino-cli core install arduino:renesas_uno).
        4. python scripts/serial_placa.py verificar

    python scripts/serial_placa.py listar
    python scripts/serial_placa.py auto
    python scripts/serial_placa.py auto --porta COM8 --gravar --registrar
    python scripts/serial_placa.py interativo --gravar
    python scripts/serial_placa.py auto --log teste.txt

    Sem --porta, o script usa a única placa reconhecida que estiver ligada.
    Feche o Monitor Serial da IDE antes: só um programa por vez usa a porta.

    Requisitos: Python 3.8+, pyserial (pip install pyserial) e, para
    --gravar, o arduino-cli no PATH com o pacote da placa instalado.

TESTES
    python -m unittest scripts/test_serial_placa.py -v
"""

import argparse
import csv
import datetime
import importlib
import json
import os
import subprocess
import sys
import threading
import time
from dataclasses import dataclass
from typing import Optional

RAIZ = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

# Placas conhecidas. "sketch_auto" e "sketch_interativo" são pastas relativas
# à raiz do repositório; None = ainda não existe sketch dedicado.
PLACAS = {
    "uno-r4-minima": {
        "nome": "UNO R4 Minima",
        "usb": [(0x2341, 0x0069)],
        "fqbn": "arduino:renesas_uno:minima",
        "sketch_auto": "boards/arduino-uno-r4/code/teste_uno_r4_automatico",
        "sketch_interativo": "boards/arduino-uno-r4/code/teste_uno_r4_interativo",
        "inventario": "boards/arduino-uno-r4/inventario.csv",
    },
    "uno-r4-wifi": {
        "nome": "UNO R4 WiFi",
        "usb": [(0x2341, 0x1002)],
        "fqbn": "arduino:renesas_uno:unor4wifi",
        "sketch_auto": "boards/arduino-uno-r4/code/teste_uno_r4_automatico",
        "sketch_interativo": "boards/arduino-uno-r4/code/teste_uno_r4_interativo",
        "inventario": "boards/arduino-uno-r4/inventario.csv",
    },
    "uno-r3": {
        "nome": "UNO R3",
        "usb": [(0x2341, 0x0043), (0x2341, 0x0001), (0x2A03, 0x0043), (0x1A86, 0x7523)],
        "fqbn": "arduino:avr:uno",
        "sketch_auto": None,
        "sketch_interativo": "shields/uno-shield-9in1/code/teste_shield_9em1_uno_r3",
        "inventario": None,
    },
}

# Prefixo da etiqueta física de cada modelo no inventário (ex: R4M-01).
PREFIXO_ETIQUETA = {"UNO R4 Minima": "R4M", "UNO R4 WiFi": "R4W"}

COLUNAS_INVENTARIO = ["etiqueta", "modelo", "id_unico", "registrado_em",
                      "ultimo_teste", "resultado", "obs"]

# No R4, a velocidade é ignorada (USB nativa). 1200 baud NÃO pode ser usado:
# abrir a porta em 1200 baud faz as placas Arduino entrarem no bootloader.
BAUD_PADRAO = 115200


class ErroPorta(Exception):
    """Porta serial não encontrada, ambígua ou sem placa reconhecida."""


class ErroGravacao(Exception):
    """O arduino-cli não conseguiu compilar ou gravar o sketch."""


@dataclass
class Porta:
    """Uma porta serial do computador.

    Attributes:
        dispositivo: nome da porta (ex: "COM8" ou "/dev/ttyACM0").
        vid: Vendor ID da USB, ou None se não for USB.
        pid: Product ID da USB, ou None.
        serie: número de série USB, ou None.
        descricao: texto que o sistema mostra para a porta.
    """
    dispositivo: str
    vid: Optional[int]
    pid: Optional[int]
    serie: Optional[str]
    descricao: str


# =============================================================================
#  Funções puras (testadas em test_serial_placa.py)
# =============================================================================

def analisar_linha(linha):
    """Interpreta uma linha enviada por um sketch de teste.

    Args:
        linha: texto recebido da serial, com ou sem quebra de linha.

    Returns:
        dict com a chave "tipo" ("inicio", "id_unico", "resultado" ou
        "fim") e os campos daquele tipo; ou None se a linha for comentário,
        texto livre ou estiver incompleta.
    """
    linha = linha.strip()
    if linha.startswith("RESULTADO;"):
        partes = linha.split(";", 3)  # o detalhe pode conter ";"
        if len(partes) < 4:
            return None
        return {"tipo": "resultado", "teste": partes[1],
                "estado": partes[2], "detalhe": partes[3]}
    if linha.startswith("FIM;"):
        contagem = {}
        for campo in linha.split(";")[1:]:
            chave, _, valor = campo.partition("=")
            if not valor.isdigit():
                return None
            contagem[chave] = int(valor)
        return {"tipo": "fim", **contagem}
    if linha.startswith("ID_UNICO;"):
        return {"tipo": "id_unico", "id": linha.split(";", 1)[1]}
    if linha.startswith("INICIO;"):
        partes = linha.split(";")
        if len(partes) < 3:
            return None
        return {"tipo": "inicio", "sketch": partes[1], "versao": partes[2]}
    return None


def identificar_placa(vid, pid):
    """Diz quais placas conhecidas usam este VID:PID na USB.

    Args:
        vid: Vendor ID (int) ou None.
        pid: Product ID (int) ou None.

    Returns:
        Lista de chaves de PLACAS (vazia se nenhuma bater).
    """
    return [chave for chave, placa in PLACAS.items() if (vid, pid) in placa["usb"]]


def escolher_porta(portas, porta=None, placa=None):
    """Escolhe a porta a usar e qual placa está nela.

    Args:
        portas: lista de Porta (normalmente de listar_portas()).
        porta: nome da porta pedida pelo usuário, ou None para detectar.
        placa: chave de PLACAS pedida pelo usuário, ou None para detectar.

    Returns:
        Tupla (Porta, chave_da_placa).

    Raises:
        ErroPorta: porta inexistente, nenhuma placa reconhecida, ou mais de
            uma placa ligada sem --porta.
    """
    if porta:
        achadas = [p for p in portas if p.dispositivo.lower() == porta.lower()]
        if not achadas:
            raise ErroPorta(f"porta {porta} não encontrada (rode o comando listar)")
        escolhida = achadas[0]
        if placa:
            return escolhida, placa
        chaves = identificar_placa(escolhida.vid, escolhida.pid)
        if not chaves:
            raise ErroPorta(f"placa em {porta} não reconhecida; indique com --placa")
        return escolhida, chaves[0]

    conhecidas = [(p, identificar_placa(p.vid, p.pid)) for p in portas]
    conhecidas = [(p, c) for p, c in conhecidas if c]
    if not conhecidas:
        raise ErroPorta("nenhuma placa reconhecida ligada (rode o comando listar)")
    if len(conhecidas) > 1:
        nomes = ", ".join(p.dispositivo for p, _ in conhecidas)
        raise ErroPorta(f"mais de uma placa ligada ({nomes}); escolha com --porta")
    p, chaves = conhecidas[0]
    return p, placa or chaves[0]


def resumir(resultados):
    """Conta os resultados por estado (INFO não conta).

    Args:
        resultados: lista de dicts com a chave "estado".

    Returns:
        dict com as chaves ok, falha, aviso e pulado.
    """
    contagem = {"ok": 0, "falha": 0, "aviso": 0, "pulado": 0}
    for r in resultados:
        chave = r["estado"].lower()
        if chave in contagem:
            contagem[chave] += 1
    return contagem


def texto_resumo(contagem):
    """Formata a contagem como "ok=N falha=N aviso=N pulado=N".

    Args:
        contagem: dict devolvido por resumir().

    Returns:
        str com a contagem, usada na coluna "resultado" do inventário.
    """
    return " ".join(f"{k}={contagem[k]}" for k in ("ok", "falha", "aviso", "pulado"))


def atualizar_inventario(caminho, id_unico, modelo, resultado, data):
    """Registra uma placa no inventário CSV, ou atualiza o último teste dela.

    Uma placa nova ganha a próxima etiqueta do modelo (R4M-01, R4M-02...).
    O ID único não vira etiqueta porque trechos dele se repetem entre chips
    do mesmo lote. Uma placa já registrada só tem "ultimo_teste" e
    "resultado" atualizados; as outras colunas (inclusive "obs", escrita à
    mão) ficam como estão.

    Args:
        caminho: arquivo CSV com o cabeçalho COLUNAS_INVENTARIO.
        id_unico: ID único do chip (a chave do inventário).
        modelo: nome do modelo (ex: "UNO R4 Minima").
        resultado: texto do resumo do teste (ex: "ok=10 falha=0 ...").
        data: data do teste, no formato AAAA-MM-DD.

    Returns:
        Tupla (etiqueta, nova), em que nova é True se a placa foi incluída.

    Raises:
        OSError: o arquivo não pôde ser lido ou gravado.
    """
    with open(caminho, newline="", encoding="utf-8") as f:
        linhas = list(csv.DictReader(f))

    etiqueta, nova = None, False
    for linha in linhas:
        if linha["id_unico"] == id_unico:
            linha["ultimo_teste"] = data
            linha["resultado"] = resultado
            etiqueta = linha["etiqueta"]
            break
    else:
        prefixo = PREFIXO_ETIQUETA.get(modelo, "PLACA")
        mesmo_modelo = [l for l in linhas if l["etiqueta"].startswith(prefixo + "-")]
        etiqueta = f"{prefixo}-{len(mesmo_modelo) + 1:02d}"
        linhas.append({"etiqueta": etiqueta, "modelo": modelo, "id_unico": id_unico,
                       "registrado_em": data, "ultimo_teste": data,
                       "resultado": resultado, "obs": ""})
        nova = True

    with open(caminho, "w", newline="", encoding="utf-8") as f:
        escritor = csv.DictWriter(f, fieldnames=COLUNAS_INVENTARIO, lineterminator="\n")
        escritor.writeheader()
        escritor.writerows(linhas)
    return etiqueta, nova


def verificar_ambiente(executar=subprocess.run, importar=importlib.import_module):
    """Confere o que o script precisa no computador.

    Args:
        executar: função no formato de subprocess.run (trocada nos testes).
        importar: função no formato de importlib.import_module (idem).

    Returns:
        Lista de tuplas (item, estado, detalhe), com estado "OK", "FALHA"
        (o script não funciona sem isso) ou "AVISO" (só algumas funções
        ficam indisponíveis, como o --gravar).
    """
    itens = []
    versao = sys.version.split()[0]
    if sys.version_info >= (3, 8):
        itens.append(("python", "OK", versao))
    else:
        itens.append(("python", "FALHA", f"{versao}: instale o Python 3.8 ou mais novo"))

    try:
        modulo = importar("serial")
        itens.append(("pyserial", "OK", getattr(modulo, "VERSION", "instalado")))
    except ImportError:
        itens.append(("pyserial", "FALHA", "não instalado: rode  pip install pyserial"))

    # O arduino-cli só é usado pelo --gravar: a falta dele é só um aviso.
    try:
        saida = executar(["arduino-cli", "version"], capture_output=True, text=True)
        itens.append(("arduino-cli", "OK", saida.stdout.strip()))
    except FileNotFoundError:
        itens.append(("arduino-cli", "AVISO",
                      "não encontrado no PATH: sem ele, o --gravar não funciona "
                      "(grave pela IDE do Arduino)"))
        return itens

    saida = executar(["arduino-cli", "core", "list", "--format", "json"],
                     capture_output=True, text=True)
    try:
        dados = json.loads(saida.stdout or "{}")
        # O formato mudou entre versões do arduino-cli: lista pura ou
        # {"platforms": [...]}; o id pode estar em "id" ou em "metadata".
        lista = dados.get("platforms", []) if isinstance(dados, dict) else dados
        instalados = {p.get("id") or p.get("metadata", {}).get("id") for p in lista}
    except (ValueError, AttributeError):
        instalados = set()
    pacotes = sorted({PLACAS[c]["fqbn"].rsplit(":", 1)[0] for c in PLACAS})
    for pacote in pacotes:
        if pacote in instalados:
            itens.append((f"pacote {pacote}", "OK", "instalado"))
        else:
            itens.append((f"pacote {pacote}", "AVISO",
                          f"não instalado: rode  arduino-cli core install {pacote}"))
    return itens


# =============================================================================
#  Funções que mexem com o hardware
# =============================================================================

def listar_portas():
    """Lista as portas seriais do computador.

    Returns:
        Lista de Porta, em ordem de nome.

    Raises:
        SystemExit: o pyserial não está instalado.
    """
    try:
        from serial.tools import list_ports
    except ImportError:
        sys.exit("erro: instale o pyserial (pip install pyserial)")
    portas = [Porta(p.device, p.vid, p.pid, p.serial_number, p.description)
              for p in list_ports.comports()]
    return sorted(portas, key=lambda p: p.dispositivo)


def gravar_sketch(chave, pasta, porta):
    """Compila e grava um sketch com o arduino-cli, e espera a porta voltar.

    Args:
        chave: chave de PLACAS (define o FQBN).
        pasta: pasta do sketch, relativa à raiz do repositório.
        porta: porta serial da placa (ex: "COM8").

    Raises:
        ErroGravacao: o arduino-cli não existe ou devolveu erro.
    """
    comando = ["arduino-cli", "compile", "--fqbn", PLACAS[chave]["fqbn"],
               "--upload", "--port", porta, os.path.join(RAIZ, pasta)]
    print(f"# Gravando {pasta} em {porta}...")
    try:
        saida = subprocess.run(comando, capture_output=True, text=True,
                               encoding="utf-8", errors="replace")
    except FileNotFoundError:
        raise ErroGravacao("arduino-cli não encontrado no PATH")
    if saida.returncode != 0:
        raise ErroGravacao((saida.stdout + saida.stderr).strip()[-800:])

    # Depois da gravação, a placa reinicia e a porta some por um instante.
    limite = time.time() + 15
    while time.time() < limite:
        time.sleep(0.5)
        if any(p.dispositivo == porta for p in listar_portas()):
            time.sleep(1)  # dá tempo de o sketch começar
            return
    raise ErroGravacao(f"a porta {porta} não voltou depois da gravação")


def abrir_serial(porta, baud):
    """Abre a porta serial.

    Args:
        porta: nome da porta.
        baud: velocidade (ignorada no UNO R4, que usa USB nativa).

    Returns:
        Objeto serial.Serial aberto. Abrir ativa o sinal DTR: no UNO R4 isso
        dispara os sketches de teste; no UNO R3, reinicia a placa.

    Raises:
        ErroPorta: a porta não pôde ser aberta (em uso por outro programa?).
    """
    import serial
    try:
        return serial.Serial(porta, baud, timeout=0.2)
    except serial.SerialException as e:
        raise ErroPorta(f"não foi possível abrir {porta} (o Monitor Serial está aberto?): {e}")


def executar_auto(conexao, tempo_limite, log=None):
    """Lê o resultado de um sketch de teste automático até a linha FIM.

    Args:
        conexao: porta serial aberta.
        tempo_limite: segundos de espera pela linha FIM.
        log: arquivo aberto para gravar tudo o que chegar, ou None.

    Returns:
        Tupla (resultados, fim, id_unico): a lista de dicts RESULTADO, o
        dict da linha FIM (ou None se o tempo acabou) e o ID_UNICO (ou None).
    """
    resultados, fim, id_unico = [], None, None
    limite = time.time() + tempo_limite
    while time.time() < limite and fim is None:
        bruta = conexao.readline()
        if not bruta:
            continue
        linha = bruta.decode("utf-8", errors="replace").rstrip()
        print(linha)
        if log:
            log.write(linha + "\n")
        dado = analisar_linha(linha)
        if not dado:
            continue
        if dado["tipo"] == "resultado":
            resultados.append(dado)
        elif dado["tipo"] == "id_unico":
            id_unico = dado["id"]
        elif dado["tipo"] == "fim":
            fim = dado
    return resultados, fim, id_unico


def executar_interativo(conexao, log=None):
    """Terminal simples: placa -> tela e teclado -> placa, até Ctrl+C.

    Uma thread lê a serial e imprime; a principal lê o teclado linha a linha
    e envia com "\\n" (o mesmo que o "Nova linha" do Monitor Serial).

    Args:
        conexao: porta serial aberta.
        log: arquivo aberto para gravar o que chegar da placa, ou None.

    Returns:
        Lista de dicts RESULTADO recebidos durante a sessão.
    """
    resultados = []
    parar = threading.Event()

    def ler_placa():
        while not parar.is_set():
            try:
                bruta = conexao.readline()
            except Exception:  # placa desligada ou reiniciada no meio
                print("# conexão perdida com a placa")
                parar.set()
                return
            if not bruta:
                continue
            linha = bruta.decode("utf-8", errors="replace").rstrip()
            print(linha, flush=True)
            if log:
                log.write(linha + "\n")
            dado = analisar_linha(linha)
            if dado and dado["tipo"] == "resultado":
                resultados.append(dado)

    leitor = threading.Thread(target=ler_placa, daemon=True)
    leitor.start()
    print("# Modo interativo. Digite e tecle Enter para enviar. Ctrl+C sai.")
    try:
        while not parar.is_set():
            texto = sys.stdin.readline()
            if not texto:  # fim da entrada (ex: entrada redirecionada)
                break
            conexao.write(texto.rstrip("\r\n").encode("utf-8") + b"\n")
    except KeyboardInterrupt:
        pass
    parar.set()
    leitor.join(timeout=1)
    return resultados


# =============================================================================
#  Linha de comando
# =============================================================================

def _cmd_verificar(_args):
    """Mostra o que está pronto no computador e as placas ligadas."""
    itens = verificar_ambiente()
    for item, estado, detalhe in itens:
        print(f"{estado:6} {item:28} {detalhe}")
    if any(estado == "FALHA" for _, estado, _ in itens):
        print()
        print("Corrija os itens com FALHA antes de usar o script.")
        return 2

    print()
    portas = listar_portas()
    conhecidas = [(p, identificar_placa(p.vid, p.pid)) for p in portas]
    conhecidas = [(p, c) for p, c in conhecidas if c]
    if not conhecidas:
        print("AVISO  nenhuma placa reconhecida ligada (confira o cabo USB: alguns só carregam)")
    for p, chaves in conhecidas:
        nomes = " ou ".join(PLACAS[c]["nome"] for c in chaves)
        print(f"OK     placa em {p.dispositivo:20} {nomes}")
    print()
    print("Tudo pronto." if conhecidas else "Computador pronto; falta ligar uma placa.")
    return 0


def _cmd_listar(_args):
    """Imprime as portas seriais e a placa reconhecida em cada uma."""
    portas = listar_portas()
    if not portas:
        print("Nenhuma porta serial encontrada.")
        return 0
    for p in portas:
        usb = f"{p.vid:04X}:{p.pid:04X}" if p.vid is not None else "-        "
        chaves = identificar_placa(p.vid, p.pid)
        placa = " ou ".join(PLACAS[c]["nome"] for c in chaves) or "-"
        print(f"{p.dispositivo:8} {usb}  {placa:16} série={p.serie or '-'}  {p.descricao}")
    return 0


def _preparar(args, tipo_sketch):
    """Escolhe a porta e grava o sketch, se pedido. Devolve (Porta, chave)."""
    porta, chave = escolher_porta(listar_portas(), args.porta, args.placa)
    print(f"# {PLACAS[chave]['nome']} em {porta.dispositivo}")
    if args.gravar:
        pasta = PLACAS[chave][tipo_sketch]
        if not pasta:
            raise ErroGravacao(f"ainda não há sketch dedicado ({tipo_sketch}) "
                               f"para {PLACAS[chave]['nome']}")
        gravar_sketch(chave, pasta, porta.dispositivo)
    return porta, chave


def _cmd_auto(args):
    """Roda o teste automático e, se pedido, registra no inventário."""
    porta, chave = _preparar(args, "sketch_auto")
    log = open(args.log, "w", encoding="utf-8") if args.log else None
    try:
        with abrir_serial(porta.dispositivo, args.baud) as conexao:
            resultados, fim, id_unico = executar_auto(conexao, args.tempo, log)
    finally:
        if log:
            log.close()

    if fim is None:
        print(f"erro: a linha FIM não chegou em {args.tempo} s. O sketch de teste "
              "automático está gravado? (use --gravar)", file=sys.stderr)
        return 2

    contagem = resumir(resultados)
    if contagem != {k: fim.get(k) for k in contagem}:
        print("# aviso: a contagem recebida não bate com a linha FIM (linha perdida?)")
    print(f"\n# Resumo: {texto_resumo(contagem)}")

    # No R4 Minima, o número de série USB É o ID único do RA4M1: os dois
    # devem bater. Se não baterem, a porta escolhida é de outra placa.
    if id_unico and chave == "uno-r4-minima" and porta.serie and porta.serie != id_unico:
        print(f"# aviso: série USB ({porta.serie}) diferente do ID_UNICO ({id_unico})")

    if args.registrar:
        caminho = PLACAS[chave]["inventario"]
        if not caminho:
            print(f"# {PLACAS[chave]['nome']} ainda não tem inventário: nada registrado")
        elif not id_unico:
            print("# o sketch não enviou ID_UNICO: nada registrado")
        else:
            hoje = datetime.date.today().isoformat()
            etiqueta, nova = atualizar_inventario(
                os.path.join(RAIZ, caminho), id_unico, PLACAS[chave]["nome"],
                texto_resumo(contagem), hoje)
            if nova:
                print(f"# Placa NOVA registrada como {etiqueta}: cole essa etiqueta nela.")
            else:
                print(f"# Placa {etiqueta} já registrada: último teste atualizado.")
            print(f"# Inventário: {caminho}")

    return 1 if contagem["falha"] else 0


def _cmd_interativo(args):
    """Abre o terminal interativo com a placa."""
    porta, _chave = _preparar(args, "sketch_interativo")
    log = open(args.log, "w", encoding="utf-8") if args.log else None
    try:
        with abrir_serial(porta.dispositivo, args.baud) as conexao:
            resultados = executar_interativo(conexao, log)
    finally:
        if log:
            log.close()
    if resultados:
        print(f"\n# Resumo: {texto_resumo(resumir(resultados))}")
    return 1 if resumir(resultados)["falha"] else 0


def main(argv=None):
    """Ponto de entrada da linha de comando.

    Args:
        argv: lista de argumentos (sem o nome do programa), ou None para usar
            sys.argv. Existe para que main() possa ser testada.

    Returns:
        Código de saída: 0 = ok, 1 = há falhas nos testes, 2 = erro.
    """
    # O console do Windows pode não aceitar todos os caracteres (ex: "µ").
    for fluxo in (sys.stdout, sys.stderr):
        if hasattr(fluxo, "reconfigure"):
            fluxo.reconfigure(errors="replace")

    parser = argparse.ArgumentParser(
        prog="serial_placa.py", description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = parser.add_subparsers(dest="comando", required=True)

    sub.add_parser("verificar", help="confere Python, pyserial, arduino-cli e placas ligadas")
    sub.add_parser("listar", help="lista as portas seriais e as placas reconhecidas")

    for nome, ajuda in (("auto", "roda o teste automático e mostra o resumo"),
                        ("interativo", "terminal para o teste com ajuda do usuário")):
        p = sub.add_parser(nome, help=ajuda)
        p.add_argument("--porta", help="porta serial (ex: COM8). Padrão: detectar")
        p.add_argument("--placa", choices=sorted(PLACAS),
                       help="força o modelo da placa, se a detecção pela USB errar")
        p.add_argument("--baud", type=int, default=BAUD_PADRAO,
                       help=f"velocidade da serial (padrão {BAUD_PADRAO})")
        p.add_argument("--gravar", action="store_true",
                       help="grava o sketch de teste dedicado da placa antes (arduino-cli)")
        p.add_argument("--log", help="arquivo onde salvar tudo o que a placa enviar")
        if nome == "auto":
            p.add_argument("--tempo", type=float, default=60,
                           help="segundos de espera pelo fim do teste (padrão 60)")
            p.add_argument("--registrar", action="store_true",
                           help="registra a placa (ou atualiza o último teste) no inventário")

    args = parser.parse_args(argv)
    comandos = {"verificar": _cmd_verificar, "listar": _cmd_listar, "auto": _cmd_auto, "interativo": _cmd_interativo}
    try:
        return comandos[args.comando](args)
    except (ErroPorta, ErroGravacao) as e:
        print(f"erro: {e}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    sys.exit(main())
