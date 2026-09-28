# Autor: Prof. Joao Miguel Roehe (@professorjoaomiguel)
# SPDX-License-Identifier: MIT
"""Teste básico da placa ESP32-C3 SuperMini em MicroPython.

O QUE FAZ
    1. Mostra no Shell do Thonny a versão do MicroPython, a frequência da
       CPU, o tamanho da flash, a RAM livre e o endereço MAC do Wi-Fi.
    2. Pisca o LED azul da placa (GPIO8) e mostra o nível lógico do pino a
       cada troca, para o aluno descobrir se o LED acende com 0 ou com 1.
    3. Avisa no Shell cada vez que o botão BOOT (GPIO9) é apertado.

COMO USAR (Thonny)
    1. Grave o firmware MicroPython ESP32_GENERIC_C3 na placa (ver o README
       da placa, seção "Como programar").
    2. Abra este arquivo no Thonny e clique em "Executar" (F5).
       Para rodar sozinho sempre que a placa ligar, salve-o na placa com
       o nome main.py (Arquivo > Salvar como... > Dispositivo MicroPython).
    3. Para parar, clique em "Parar" (Ctrl+F2).

O QUE OBSERVAR
    - "Flash: 4.0 MB" e a versão do MicroPython.
    - Em qual nível (0 ou 1) o LED azul fica ACESO. Anote no README.
    - A mensagem "BOOT apertado" ao apertar o botão BOOT.

ATENÇÃO
    A placa trabalha em 3,3V. Não ligue sinais de 5V nos GPIOs.
"""
import gc
import os
import time

import esp
import machine
import network
import ubinascii

# GPIO do LED azul da placa (serigrafia "IO8"). É o mesmo pino do SDA
# padrão do I2C nesta placa.
PINO_LED = 8

# GPIO do botão BOOT. O botão liga o pino ao GND: apertado = 0.
PINO_BOOT = 9

# Tempo entre as trocas do LED, em milissegundos.
INTERVALO_LED_MS = 1000


def mostrar_dados_da_placa():
    """Mostra no Shell a versão, a CPU, as memórias e o MAC da placa."""
    gc.collect()
    print()
    print("=== ESP32-C3 SuperMini: dados da placa ===")
    print("MicroPython ...", os.uname().release, "-", os.uname().machine)
    print("Frequência ....", machine.freq() // 1_000_000, "MHz")
    print("Flash .........", round(esp.flash_size() / (1024 * 1024), 1), "MB")
    print("RAM livre .....", gc.mem_free() // 1024, "KB")
    # Ler o MAC não liga o Wi-Fi para conectar: só ativa a interface.
    wlan = network.WLAN(network.STA_IF)
    wlan.active(True)
    mac = ubinascii.hexlify(wlan.config("mac"), ":").decode().upper()
    wlan.active(False)
    print("MAC (Wi-Fi) ...", mac)
    print()


def main():
    """Ponto de entrada: mostra os dados, pisca o LED e lê o botão BOOT."""
    mostrar_dados_da_placa()
    led = machine.Pin(PINO_LED, machine.Pin.OUT)
    boot = machine.Pin(PINO_BOOT, machine.Pin.IN, machine.Pin.PULL_UP)
    print("LED azul (GPIO8) troca de nível a cada segundo.")
    print("Anote em qual nível ele fica ACESO. Aperte BOOT para testar o botão.")

    nivel = 0
    ultima_troca = time.ticks_ms()
    boot_anterior = 1
    try:
        while True:
            # Troca o LED sem sleep longo, para o botão continuar sendo lido.
            if time.ticks_diff(time.ticks_ms(), ultima_troca) >= INTERVALO_LED_MS:
                ultima_troca = time.ticks_ms()
                nivel = 1 - nivel
                led.value(nivel)
                print("LED: GPIO8 =", nivel)

            # Avisa só na borda de descida (solto -> apertado).
            boot_agora = boot.value()
            if boot_anterior == 1 and boot_agora == 0:
                print("BOOT apertado")
            boot_anterior = boot_agora
            time.sleep_ms(10)  # filtro simples contra o repique do botão
    finally:
        # Ao parar pelo Thonny, deixa o pino em 1. Se o LED for ativo em
        # LOW (o mais comum nesta placa, a confirmar), isso o apaga.
        led.value(1)


main()
