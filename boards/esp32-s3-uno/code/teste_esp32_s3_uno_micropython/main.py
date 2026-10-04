# Autor: Prof. Me. Joao Miguel Lac Roehe (@professorjoaomiguel)
# SPDX-License-Identifier: MIT
"""Teste básico da placa ESP32-S3 UNO (TZT D1 ESP32-S3 N16R8) em MicroPython.

O QUE FAZ
    1. Mostra no Shell do Thonny a frequência da CPU, o tamanho da memória
       flash e a memória RAM livre. Com a PSRAM ativa, a RAM livre passa
       de alguns megabytes; sem ela, fica perto de 200 KB. Serve para
       confirmar a variante da placa (N16R8 ou N8R2) e o firmware certo.
    2. Faz o LED RGB endereçável (WS2812) da placa, no GPIO48, trocar de
       cor a cada segundo: vermelho, verde, azul, branco e apagado.

COMO USAR (Thonny)
    1. Grave o firmware MicroPython para ESP32-S3 na placa (ver o README
       da placa, seção "Como programar").
    2. Abra este arquivo no Thonny e clique em "Executar" (F5).
       Para rodar sozinho sempre que a placa ligar, salve-o na placa com
       o nome main.py (Arquivo > Salvar como... > Dispositivo MicroPython).
    3. Para parar, clique em "Parar" (Ctrl+F2).

O QUE OBSERVAR
    - "Flash: 16 MB" e "RAM livre" de vários MB (versão N16R8).
      "RAM livre" perto de 0.2 MB -> o firmware gravado não liga a PSRAM
      octal da placa (no boot aparece "quad_psram: PSRAM chip is not
      connected"). Grave a variante SPIRAM_OCT do firmware ESP32_GENERIC_S3.
    - O LED RGB trocando de cor na ordem mostrada no Shell. Se as cores
      saírem trocadas, anote no README da placa.

ATENÇÃO
    A placa trabalha em 3,3V, mesmo tendo o formato do UNO. Não encaixe
    shields de 5V nela.
"""
import gc
import time

import esp
import machine
import neopixel

# GPIO do LED RGB endereçável (WS2812) da placa: serigrafia "SW2812 (IO48)".
PINO_LED_RGB = 48

# Brilho de cada cor (0 a 255). O WS2812 é muito forte mesmo em 40.
BRILHO = 40

# Cores do teste: (nome para o Shell, (vermelho, verde, azul)).
CORES = [
    ("vermelho", (BRILHO, 0, 0)),
    ("verde", (0, BRILHO, 0)),
    ("azul", (0, 0, BRILHO)),
    ("branco", (BRILHO, BRILHO, BRILHO)),
    ("apagado", (0, 0, 0)),
]


def em_mb(num_bytes):
    """Converte bytes para megabytes (MB), com uma casa decimal.

    Args:
        num_bytes: quantidade de memória em bytes.

    Returns:
        O valor em MB (1 MB = 1024 * 1024 bytes), como float.
    """
    return round(num_bytes / (1024 * 1024), 1)


def mostrar_dados_da_placa():
    """Mostra no Shell a frequência da CPU, a flash e a RAM livre."""
    # Coleta o lixo antes de medir, para a RAM livre não sair subestimada.
    gc.collect()
    print()
    print("=== ESP32-S3 UNO: dados da placa ===")
    print("Frequência ....", machine.freq() // 1_000_000, "MHz")
    print("Flash .........", em_mb(esp.flash_size()), "MB")
    # No MicroPython, a PSRAM entra no espaço do gc: não há uma função só
    # para ela. Por isso a RAM livre é o indício de que a PSRAM está ativa.
    print("RAM livre .....", em_mb(gc.mem_free()), "MB")
    print()


def ciclo_do_led(led):
    """Passa o LED RGB por todas as cores de CORES, uma por segundo.

    Args:
        led: objeto neopixel.NeoPixel com 1 LED.
    """
    for nome, cor in CORES:
        print("LED RGB:", nome)
        led[0] = cor
        led.write()
        time.sleep(1)


def main():
    """Ponto de entrada: mostra os dados e repete o ciclo do LED."""
    mostrar_dados_da_placa()
    led = neopixel.NeoPixel(machine.Pin(PINO_LED_RGB), 1)
    print("Teste do LED RGB (GPIO48): a cor muda a cada segundo.")
    try:
        while True:
            ciclo_do_led(led)
    finally:
        # Ao parar pelo Thonny (Ctrl+F2), apaga o LED em vez de deixá-lo
        # aceso na última cor.
        led[0] = (0, 0, 0)
        led.write()


main()
