# Autor: Prof. Joao Miguel Roehe (@professorjoaomiguel)
# SPDX-License-Identifier: MIT
"""Gera o relatório HTML do inventário a partir dos CSVs de inventario/.

O QUE FAZ
    Lê todos os arquivos `.csv` da pasta `inventario/` (um por tipo de placa
    e um de shields) e escreve `inventario/relatorio.html`: uma página com
    todas as unidades numa tabela só, com filtros (tipo, dono, situação,
    etiqueta colada), busca livre, contadores e colunas ordenáveis.

    O HTML é um arquivo só, com os dados embutidos: abre com dois cliques,
    sem internet e sem servidor. (Um HTML que lesse os CSVs sozinho não
    funcionaria aberto direto do disco: o navegador bloqueia a leitura de
    arquivos locais por segurança.)

    Os CSVs continuam sendo a fonte: o relatório é só uma forma de olhar.
    Não edite o HTML à mão; edite o CSV e gere de novo.

COMO USAR
    Na raiz do repositório:

        python scripts/gerar_inventario.py          # gera inventario/relatorio.html
        python scripts/gerar_inventario.py --help   # mostra esta ajuda

    Rode sempre que mudar um CSV do inventário e faça commit dos dois
    juntos. A geração é manual: não há hook de git nem CI.

SITUAÇÃO DE CADA UNIDADE
    Calculada a partir da coluna `resultado`:
      - "ok=.. falha=N .."   -> "com falha" se N > 0
      - "ok=.. aviso=N .."   -> "atenção" se N > 0, senão "ok"
      - "tudo OK"            -> "ok"
      - vazio / "não testado" -> "não testado"
      - outro texto           -> "atenção" (ex: "LM35 instável")

TESTES
    python -m unittest scripts/test_gerar_inventario.py -v
"""
import argparse
import csv
import datetime
import json
import re
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parent.parent
PASTA_PADRAO = REPO_ROOT / "inventario"

# Nome de exibição de cada arquivo do inventário. Um arquivo novo que não
# esteja aqui aparece com o próprio nome (sem o .csv).
TIPOS = {
    "arduino-uno-r4": "Arduino UNO R4",
    "arduino-uno-r3": "Arduino UNO R3",
    "esp32-s3-uno": "ESP32-S3 UNO",
    "shield-9em1": "Shield 9 em 1",
}

# Colunas que têm lugar próprio no relatório. As demais de cada arquivo
# (ex: flash e PSRAM do ESP32-S3) aparecem em "detalhes".
COLUNAS_COMUNS = {"etiqueta", "etiqueta_colada", "modelo", "variante", "dono",
                  "id_unico", "mac", "registrado_em", "ultimo_teste", "testado_em",
                  "resultado", "obs"}


def situacao(resultado):
    """Classifica uma unidade pelo resumo do último teste.

    Args:
        resultado: texto da coluna `resultado` (ex: "ok=5 falha=0 aviso=0
            pulado=0", "tudo OK", "LM35 instável" ou vazio).

    Returns:
        "ok", "atenção", "com falha" ou "não testado".
    """
    texto = (resultado or "").strip().lower()
    if not texto or texto == "não testado":
        return "não testado"
    falhas = re.search(r"falha=(\d+)", texto)
    if falhas:
        if int(falhas.group(1)) > 0:
            return "com falha"
        avisos = re.search(r"aviso=(\d+)", texto)
        return "atenção" if avisos and int(avisos.group(1)) > 0 else "ok"
    if texto in ("ok", "tudo ok"):
        return "ok"
    return "atenção"


def ler_inventarios(pasta=PASTA_PADRAO):
    """Lê todos os CSVs da pasta e devolve as unidades num formato comum.

    Args:
        pasta: pasta com os CSVs do inventário.

    Returns:
        Lista de dicts, um por unidade, com as chaves tipo, modelo,
        etiqueta, etiqueta_colada, dono, variante, identificador,
        registrado_em, ultimo_teste, resultado, situacao, obs, arquivo e
        detalhes (dict com as colunas próprias do arquivo).

    Raises:
        OSError: um arquivo não pôde ser lido.
    """
    itens = []
    for caminho in sorted(Path(pasta).glob("*.csv")):
        tipo = TIPOS.get(caminho.stem, caminho.stem)
        with open(caminho, newline="", encoding="utf-8") as f:
            for linha in csv.DictReader(f):
                linha = {k: (v or "").strip() for k, v in linha.items() if k}
                itens.append({
                    "tipo": tipo,
                    "modelo": linha.get("modelo") or tipo,
                    "etiqueta": linha.get("etiqueta", ""),
                    "etiqueta_colada": linha.get("etiqueta_colada", ""),
                    "dono": linha.get("dono", "") or "a confirmar",
                    "variante": linha.get("variante", ""),
                    "identificador": linha.get("id_unico") or linha.get("mac", ""),
                    "registrado_em": linha.get("registrado_em", ""),
                    "ultimo_teste": linha.get("ultimo_teste") or linha.get("testado_em", ""),
                    "resultado": linha.get("resultado", ""),
                    "situacao": situacao(linha.get("resultado", "")),
                    "obs": linha.get("obs", ""),
                    "arquivo": caminho.name,
                    "detalhes": {k: v for k, v in linha.items()
                                 if k not in COLUNAS_COMUNS and v},
                })
    return itens


def gerar_html(itens, data):
    """Monta a página do relatório com os dados embutidos.

    Args:
        itens: lista devolvida por ler_inventarios().
        data: data da geração (texto, ex: "2026-10-03").

    Returns:
        O HTML completo (str).
    """
    # "</" vira "<\/" dentro do JSON: assim nenhum texto do CSV consegue
    # fechar a tag <script> dos dados (o JSON continua o mesmo).
    dados = json.dumps(itens, ensure_ascii=False, indent=1).replace("</", "<\\/")
    return MODELO_HTML.replace("__DADOS__", dados).replace("__DATA__", data)


def main(argv=None):
    """Ponto de entrada da linha de comando.

    Args:
        argv: lista de argumentos (sem o nome do programa), ou None para usar
            sys.argv. Existe para que main() possa ser testada.

    Returns:
        0 se o relatório foi gerado.
    """
    parser = argparse.ArgumentParser(
        prog="gerar_inventario.py", description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--pasta", default=str(PASTA_PADRAO),
                        help="pasta com os CSVs (padrão: inventario/)")
    parser.add_argument("--saida", default=None,
                        help="arquivo HTML gerado (padrão: <pasta>/relatorio.html)")
    args = parser.parse_args(argv)

    itens = ler_inventarios(args.pasta)
    saida = Path(args.saida) if args.saida else Path(args.pasta) / "relatorio.html"
    html = gerar_html(itens, datetime.date.today().isoformat())
    saida.write_text(html, encoding="utf-8", newline="\n")
    print(f"{saida} gerado com {len(itens)} unidade(s).")
    return 0


# Página do relatório. Tudo inline (sem CDN): funciona offline.
MODELO_HTML = r"""<!doctype html>
<html lang="pt-BR">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Inventário do laboratório</title>
<style>
  :root {
    --fundo: #f7f7f5; --cartao: #ffffff; --texto: #1d1d1b; --suave: #6b6b66;
    --borda: #dcdcd6; --destaque: #2f5d8a;
    --ok: #2e7d32; --ok-fundo: #e6f2e7;
    --atencao: #8a5a00; --atencao-fundo: #fbf0d9;
    --falha: #b3261e; --falha-fundo: #f9e3e1;
    --nt: #555; --nt-fundo: #ececea;
  }
  @media (prefers-color-scheme: dark) {
    :root {
      --fundo: #161615; --cartao: #1f1f1d; --texto: #ececea; --suave: #a3a39c;
      --borda: #3a3a36; --destaque: #8fb8e0;
      --ok: #8fd18f; --ok-fundo: #1f3320;
      --atencao: #e8c06a; --atencao-fundo: #3a2f17;
      --falha: #f0928b; --falha-fundo: #3d1f1d;
      --nt: #c4c4bd; --nt-fundo: #2c2c29;
    }
  }
  * { box-sizing: border-box; }
  body { margin: 0; background: var(--fundo); color: var(--texto);
         font: 15px/1.45 system-ui, -apple-system, "Segoe UI", sans-serif; }
  main { max-width: 1280px; margin: 0 auto; padding: 24px 16px 48px; }
  h1 { font-size: 1.5rem; margin: 0 0 4px; }
  .sub { color: var(--suave); margin: 0 0 20px; }
  .cartoes { display: grid; grid-template-columns: repeat(auto-fill, minmax(150px, 1fr));
             gap: 10px; margin-bottom: 18px; }
  .cartao { background: var(--cartao); border: 1px solid var(--borda); border-radius: 10px;
            padding: 10px 12px; cursor: pointer; }
  .cartao b { display: block; font-size: 1.5rem; font-variant-numeric: tabular-nums; }
  .cartao span { color: var(--suave); font-size: .85rem; }
  .filtros { display: flex; flex-wrap: wrap; gap: 10px; align-items: end; margin-bottom: 14px; }
  .filtros label { display: flex; flex-direction: column; font-size: .8rem; color: var(--suave); gap: 3px; }
  select, input, button { font: inherit; color: var(--texto); background: var(--cartao);
           border: 1px solid var(--borda); border-radius: 8px; padding: 6px 8px; }
  input[type=search] { min-width: 220px; }
  button { cursor: pointer; }
  .tabela { overflow-x: auto; background: var(--cartao); border: 1px solid var(--borda); border-radius: 10px; }
  table { border-collapse: collapse; width: 100%; min-width: 980px; }
  th, td { text-align: left; padding: 8px 10px; border-bottom: 1px solid var(--borda); vertical-align: top; }
  th { font-size: .8rem; color: var(--suave); cursor: pointer; white-space: nowrap; user-select: none; }
  th[aria-sort=ascending]::after { content: " ▲"; }
  th[aria-sort=descending]::after { content: " ▼"; }
  tr:last-child td { border-bottom: 0; }
  .mono { font-family: ui-monospace, "Cascadia Mono", Consolas, monospace; font-size: .85rem; word-break: break-all; }
  .data { white-space: nowrap; word-break: normal; }
  .selo { display: inline-block; padding: 1px 8px; border-radius: 999px; font-size: .8rem; white-space: nowrap; }
  .s-ok { color: var(--ok); background: var(--ok-fundo); }
  .s-atencao { color: var(--atencao); background: var(--atencao-fundo); }
  .s-falha { color: var(--falha); background: var(--falha-fundo); }
  .s-nt { color: var(--nt); background: var(--nt-fundo); }
  .suave { color: var(--suave); }
  a { color: var(--destaque); }
  details summary { cursor: pointer; color: var(--destaque); }
  dl { margin: 6px 0 0; display: grid; grid-template-columns: auto 1fr; gap: 2px 10px; font-size: .85rem; }
  dt { color: var(--suave); }
  dd { margin: 0; word-break: break-all; }
  .vazio { padding: 24px; text-align: center; color: var(--suave); }
  footer { margin-top: 16px; color: var(--suave); font-size: .85rem; }
  @media print { .filtros, .cartoes { display: none; } body { background: #fff; } }
</style>
</head>
<body>
<main>
  <h1>Inventário do laboratório</h1>
  <p class="sub">Placas e shields registrados em <code>inventario/</code> · gerado em __DATA__ ·
    <span id="contagem"></span></p>

  <section class="cartoes" id="cartoes" aria-label="Resumo"></section>

  <section class="filtros" aria-label="Filtros">
    <label>Tipo <select id="f-tipo"></select></label>
    <label>Dono <select id="f-dono"></select></label>
    <label>Situação <select id="f-situacao"></select></label>
    <label>Etiqueta colada <select id="f-colada"></select></label>
    <label>Busca <input type="search" id="f-busca" placeholder="etiqueta, número de série, observação…"></label>
    <button type="button" id="limpar">Limpar filtros</button>
  </section>

  <div class="tabela">
    <table>
      <thead><tr id="cabecalho"></tr></thead>
      <tbody id="linhas"></tbody>
    </table>
    <div class="vazio" id="vazio" hidden>Nenhuma unidade com esses filtros.</div>
  </div>

  <footer>Fonte: os arquivos CSV de <code>inventario/</code>. Para mudar algo, edite o CSV e
    rode <code>python scripts/gerar_inventario.py</code>. Repositório:
    <a href="https://github.com/professorjoaomiguel/lab-boards">professorjoaomiguel/lab-boards</a>.</footer>
</main>

<script type="application/json" id="dados">__DADOS__</script>
<script>
(function () {
  const itens = JSON.parse(document.getElementById("dados").textContent);
  const SITUACOES = ["ok", "atenção", "com falha", "não testado"];
  const CLASSE = {"ok": "s-ok", "atenção": "s-atencao", "com falha": "s-falha", "não testado": "s-nt"};
  const COLUNAS = [
    ["modelo", "Tipo / modelo"], ["etiqueta", "Etiqueta"], ["etiqueta_colada", "Colada"],
    ["dono", "Dono"], ["variante", "Variante"], ["identificador", "Identificador"],
    ["ultimo_teste", "Último teste"], ["situacao", "Situação"], ["obs", "Observações"],
    ["detalhes", "Detalhes"]
  ];
  const filtros = {
    tipo: document.getElementById("f-tipo"), dono: document.getElementById("f-dono"),
    situacao: document.getElementById("f-situacao"), colada: document.getElementById("f-colada"),
    busca: document.getElementById("f-busca")
  };
  let ordem = {coluna: "modelo", sentido: 1};

  function opcoes(select, valores) {
    select.innerHTML = "";
    [["", "todos"]].concat(valores.map(v => [v, v || "(vazio)"])).forEach(([v, t]) => {
      const o = document.createElement("option"); o.value = v; o.textContent = t; select.appendChild(o);
    });
  }
  const unicos = chave => [...new Set(itens.map(i => i[chave]))].sort();
  opcoes(filtros.tipo, unicos("tipo"));
  opcoes(filtros.dono, unicos("dono"));
  opcoes(filtros.situacao, SITUACOES.filter(s => itens.some(i => i.situacao === s)));
  opcoes(filtros.colada, unicos("etiqueta_colada"));

  // Lembra os filtros na URL (#tipo=...), para compartilhar uma visão.
  function lerUrl() {
    const p = new URLSearchParams(location.hash.slice(1));
    for (const k in filtros) if (p.has(k)) filtros[k].value = p.get(k);
  }
  function gravarUrl() {
    const p = new URLSearchParams();
    for (const k in filtros) if (filtros[k].value) p.set(k, filtros[k].value);
    history.replaceState(null, "", p.toString() ? "#" + p.toString() : location.pathname);
  }

  function texto(i) {
    return [i.tipo, i.modelo, i.etiqueta, i.dono, i.variante, i.identificador, i.resultado, i.obs,
            Object.entries(i.detalhes || {}).flat().join(" ")].join(" ").toLowerCase();
  }
  function filtrados() {
    const busca = filtros.busca.value.trim().toLowerCase();
    return itens.filter(i =>
      (!filtros.tipo.value || i.tipo === filtros.tipo.value) &&
      (!filtros.dono.value || i.dono === filtros.dono.value) &&
      (!filtros.situacao.value || i.situacao === filtros.situacao.value) &&
      (!filtros.colada.value || i.etiqueta_colada === filtros.colada.value) &&
      (!busca || texto(i).includes(busca)));
  }

  function celula(tr, conteudo, classe) {
    const td = document.createElement("td");
    if (classe) td.className = classe;
    if (conteudo instanceof Node) td.appendChild(conteudo); else td.textContent = conteudo || "";
    tr.appendChild(td);
  }
  function detalhes(i) {
    const extra = Object.assign({}, i.detalhes, {
      "registrado em": i.registrado_em, "resultado": i.resultado, "arquivo": i.arquivo});
    const d = document.createElement("details");
    const s = document.createElement("summary"); s.textContent = "ver"; d.appendChild(s);
    const dl = document.createElement("dl");
    Object.entries(extra).forEach(([k, v]) => {
      if (!v) return;
      const dt = document.createElement("dt"); dt.textContent = k;
      const dd = document.createElement("dd"); dd.textContent = v;
      dl.append(dt, dd);
    });
    d.appendChild(dl);
    return d;
  }

  function desenhar() {
    const lista = filtrados().sort((a, b) => {
      const x = (a[ordem.coluna] || "") + a.etiqueta, y = (b[ordem.coluna] || "") + b.etiqueta;
      return x.localeCompare(y, "pt-BR", {numeric: true}) * ordem.sentido;
    });
    const corpo = document.getElementById("linhas");
    corpo.innerHTML = "";
    lista.forEach(i => {
      const tr = document.createElement("tr");
      celula(tr, i.modelo);
      celula(tr, i.etiqueta, "mono");
      celula(tr, i.etiqueta_colada);
      celula(tr, i.dono, i.dono === "a confirmar" ? "suave" : "");
      celula(tr, i.variante);
      celula(tr, i.identificador, "mono");
      celula(tr, i.ultimo_teste, "mono data");
      const selo = document.createElement("span");
      selo.className = "selo " + (CLASSE[i.situacao] || "s-nt"); selo.textContent = i.situacao;
      celula(tr, selo);
      celula(tr, i.obs);
      celula(tr, detalhes(i));
      corpo.appendChild(tr);
    });
    document.getElementById("vazio").hidden = lista.length > 0;
    document.getElementById("contagem").textContent =
      lista.length + " de " + itens.length + " unidade(s) na tabela";
    document.querySelectorAll("#cabecalho th").forEach(th => {
      th.setAttribute("aria-sort", th.dataset.coluna === ordem.coluna
        ? (ordem.sentido > 0 ? "ascending" : "descending") : "none");
    });
    gravarUrl();
  }

  function cartoes() {
    const area = document.getElementById("cartoes");
    const contar = f => itens.filter(f).length;
    const lista = [
      ["Total", itens.length, () => {}],
      ["ok", contar(i => i.situacao === "ok"), () => filtros.situacao.value = "ok"],
      ["atenção", contar(i => i.situacao === "atenção"), () => filtros.situacao.value = "atenção"],
      ["com falha", contar(i => i.situacao === "com falha"), () => filtros.situacao.value = "com falha"],
      ["não testado", contar(i => i.situacao === "não testado"), () => filtros.situacao.value = "não testado"],
      ["sem etiqueta colada", contar(i => i.etiqueta_colada !== "sim"), () => filtros.colada.value = "não"],
      ["dono a confirmar", contar(i => i.dono === "a confirmar"), () => filtros.dono.value = "a confirmar"]
    ];
    lista.forEach(([nome, n, acao]) => {
      const c = document.createElement("button");
      c.type = "button"; c.className = "cartao";
      c.innerHTML = "<b></b><span></span>";
      c.querySelector("b").textContent = n; c.querySelector("span").textContent = nome;
      c.addEventListener("click", () => {
        for (const k in filtros) filtros[k].value = "";
        acao(); desenhar();
      });
      area.appendChild(c);
    });
  }

  const cab = document.getElementById("cabecalho");
  COLUNAS.forEach(([chave, nome]) => {
    const th = document.createElement("th");
    th.textContent = nome; th.dataset.coluna = chave; th.scope = "col";
    if (chave !== "detalhes") th.addEventListener("click", () => {
      ordem = {coluna: chave, sentido: ordem.coluna === chave ? -ordem.sentido : 1};
      desenhar();
    });
    cab.appendChild(th);
  });
  Object.values(filtros).forEach(f => f.addEventListener("input", desenhar));
  document.getElementById("limpar").addEventListener("click", () => {
    for (const k in filtros) filtros[k].value = "";
    desenhar();
  });

  lerUrl();
  cartoes();
  desenhar();
})();
</script>
</body>
</html>
"""


if __name__ == "__main__":
    raise SystemExit(main())
