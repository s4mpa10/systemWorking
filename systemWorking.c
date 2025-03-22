#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "hardware/pio.h"

// Configuração do pino do buzzer
#define BUZZER_PIN 21

// Configuração da frequência do buzzer (em Hz)
#define BUZZER_FREQUENCY 100

// Definição do número de LEDs e o pino que é utilizado.
#define LED_COUNT 25
#define LED_PIN 7

// Definição de pixel GRB
struct pixel_t {
  uint8_t G, R, B; // Três valores de 8-bits compõem um pixel (Verde, Vermelho, Azul).
};
typedef struct pixel_t pixel_t;
typedef pixel_t npLED_t; // Mudança de nome de "struct pixel_t" para "npLED_t" por clareza.

// Declaração do buffer de pixels que formam a matriz.
npLED_t leds[LED_COUNT];

// Variáveis para uso da máquina PIO.
PIO np_pio;
uint sm;

// Biblioteca gerada pelo arquivo .pio durante compilação.
#include "ws2818b.pio.h"


// Inicializa a máquina PIO para controle da matriz de LEDs.

void npInit(uint pin) {
  // Cria programa PIO.
  uint offset = pio_add_program(pio0, &ws2818b_program);
  np_pio = pio0;

  // Toma posse de uma máquina PIO.
  sm = pio_claim_unused_sm(np_pio, false);
  if (sm < 0) {
    np_pio = pio1;
    sm = pio_claim_unused_sm(np_pio, true); // Se nenhuma máquina estiver livre, panic!
  }

  // Inicia programa na máquina PIO obtida.
  ws2818b_program_init(np_pio, sm, offset, pin, 800000.f);

  // Limpa buffer de pixels.
  for (uint i = 0; i < LED_COUNT; ++i) {
    leds[i].R = 0;
    leds[i].G = 0;
    leds[i].B = 0;
  }
}


// Atribui uma cor RGB a um LED.
void npSetLED(const uint index, const uint8_t r, const uint8_t g, const uint8_t b) {
  leds[index].R = r;
  leds[index].G = g;
  leds[index].B = b;
}


// Limpa o buffer de pixels (desliga todos os LEDs).
void npClear() {
  for (uint i = 0; i < LED_COUNT; ++i)
    npSetLED(i, 0, 0, 0);
}


// Escreve os dados do buffer nos LEDs.
// Envia os dados RGB para a máquina PIO.
void npWrite() {
  // Escreve cada dado de 8-bits dos pixels em sequência no buffer da máquina PIO.
  for (uint i = 0; i < LED_COUNT; ++i) {
    pio_sm_put_blocking(np_pio, sm, leds[i].G);
    pio_sm_put_blocking(np_pio, sm, leds[i].R);
    pio_sm_put_blocking(np_pio, sm, leds[i].B);
  }
  sleep_us(100); // Espera 100us, sinal de RESET do datasheet.
}

// Definição de uma função para inicializar o PWM no pino do buzzer
// Configura a frequência e o ciclo de trabalho.
void pwm_init_buzzer(uint pin) {
    // Configurar o pino como saída de PWM
    gpio_set_function(pin, GPIO_FUNC_PWM);

    // Obter o slice do PWM associado ao pino
    uint slice_num = pwm_gpio_to_slice_num(pin);

    // Configurar o PWM com frequência desejada
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, clock_get_hz(clk_sys) / (BUZZER_FREQUENCY * 4096)); // Divisor de clock
    pwm_init(slice_num, &config, true);

    // Iniciar o PWM no nível baixo (inicialmente o buzzer está desligado)
    pwm_set_gpio_level(pin, 0);
}
// Calcula o índice de um LED na matriz de 5x5, levando em consideração a direção em que cada linha é percorrida (esquerda para direita ou direita para esquerda).

int getIndex(int x, int y) {
    // Se a linha for par (0, 2, 4), percorremos da esquerda para a direita.
    // Se a linha for ímpar (1, 3), percorremos da direita para a esquerda.
    if (y % 2 == 0) {
        return 24-(y * 5 + x); // Linha par (esquerda para direita).
    } else {
        return 24-(y * 5 + (4 - x)); // Linha ímpar (direita para esquerda).
    }
}

// Função para exibir a matriz verde (todos os LEDs acesos na cor verde).
void mostrarMatrizVerde() {
    // Definindo a cor verde (RGB = {0}, {255}, {0}) 
    int matriz_verde[5][5][3] = {
        {{0, 255, 0}, {0, 255, 0}, {0, 255, 0}, {0, 255, 0}, {0, 255, 0}},
        {{0, 255, 0}, {0, 255, 0}, {0, 255, 0}, {0, 255, 0}, {0, 255, 0}},
        {{0, 255, 0}, {0, 255, 0}, {0, 255, 0}, {0, 255, 0}, {0, 255, 0}},
        {{0, 255, 0}, {0, 255, 0}, {0, 255, 0}, {0, 255, 0}, {0, 255, 0}},
        {{0, 255, 0}, {0, 255, 0}, {0, 255, 0}, {0, 255, 0}, {0, 255, 0}}
    };
    // Atribui a cor verde a cada LED da matriz.
    for (int linha = 0; linha < 5; linha++) {
        for (int coluna = 0; coluna < 5; coluna++) {
            int posicao = getIndex(linha, coluna);
            npSetLED(posicao, matriz_verde[coluna][linha][0], matriz_verde[coluna][linha][1], matriz_verde[coluna][linha][2]);
        }
    }
    npWrite(); // Atualiza a cor escrevendo na matriz de LEDs.
}

// Função para exibir a matriz vermelha (todos os LEDs acesos na cor vermelha).
void mostrarMatrizVermelha() {
    // Definindo a cor vermelha (RGB = {255}, {0}, {0}) 
    int matriz_vermelha[5][5][3] = {
        {{255, 0, 0}, {255, 0, 0}, {255, 0, 0}, {255, 0, 0}, {255, 0, 0}},
        {{255, 0, 0}, {255, 0, 0}, {255, 0, 0}, {255, 0, 0}, {255, 0, 0}},
        {{255, 0, 0}, {255, 0, 0}, {255, 0, 0}, {255, 0, 0}, {255, 0, 0}},
        {{255, 0, 0}, {255, 0, 0}, {255, 0, 0}, {255, 0, 0}, {255, 0, 0}},
        {{255, 0, 0}, {255, 0, 0}, {255, 0, 0}, {255, 0, 0}, {255, 0, 0}}
    };
    // Atribui a cor vermelha a cada LED da matriz.
    for (int linha = 0; linha < 5; linha++) {
        for (int coluna = 0; coluna < 5; coluna++) {
            int posicao = getIndex(linha, coluna);
            npSetLED(posicao, matriz_vermelha[coluna][linha][0], matriz_vermelha[coluna][linha][1], matriz_vermelha[coluna][linha][2]);
        }
    }
    npWrite(); // Atualiza a cor escrevendo na matriz de LEDs.
}

// Função para emitir um beep (gerar som no buzzer)
void beep(uint pin) {
    // Obter o slice do PWM associado ao pino
    uint slice_num = pwm_gpio_to_slice_num(pin);

    // Configurar o duty cycle para 50% (ativo)
    pwm_set_gpio_level(pin, 2048);
}

// Função para parar o beep (desligar o buzzer).
void stopBeep(uint pin) {
    // Desativar o sinal PWM (duty cycle 0, desligando o buzzer)
    pwm_set_gpio_level(pin, 0);
}

int main() {
    // Inicializa entradas e saídas.
    stdio_init_all();

    // Configuração do GPIO para o botão como entrada com pull-up
    const uint BUTTON_A_PIN = 5;  // Pino do botão A
    gpio_init(BUTTON_A_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN);

    // Configuração do GPIO para o botão B como entrada com pull-up.
    const uint BUTTON_B_PIN = 6;  // Pino do botão B
    gpio_init(BUTTON_B_PIN);
    gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_B_PIN);

    // Configuração do GPIO para o buzzer como saída
    gpio_init(BUZZER_PIN);
    gpio_set_dir(BUZZER_PIN, GPIO_OUT);
    // Inicializar o PWM no pino do buzzer
    pwm_init_buzzer(BUZZER_PIN);

    // Inicializa matriz de LEDs NeoPixel.
    npInit(LED_PIN);
    npClear(); // Limpa os LEDs (desliga todos).
    npWrite(); // Atualiza a matriz (todos desligados inicialmente).
    mostrarMatrizVermelha(); // Exibe a matriz vermelha como padrão.

    while (true) {
        // Verifica o estado do botão A
        if (gpio_get(BUTTON_A_PIN) == 0) {  // Botão A pressionado (nível lógico baixo)
            printf("Button A pressed\n");
            beep(BUZZER_PIN); // Inicia o beep
            mostrarMatrizVerde(); // Mostra a matriz verde
        }

        // Verifica o estado do botão B
        if (gpio_get(BUTTON_B_PIN) == 0) {  // Botão B pressionado (nível lógico baixo)
            printf("Button B pressed\n");
            stopBeep(BUZZER_PIN); // Para o beep
            mostrarMatrizVermelha(); // Mostra a matriz vermelha
        }

        sleep_ms(100); // Pequena pausa para evitar polling excessivo
    }

    return 0;
}