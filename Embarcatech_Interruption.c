#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"           // Biblioteca para manipulação de periféricos PIO
#include "ws2818b.pio.h"             // Programa para controle de LEDs WS2812B
#include "hardware/timer.h"

         

#define LED_COUNT 25
#define LED_PIN 7

const uint ledPin_Red = 13;
const uint buttonA = 5;
const uint buttonB = 6;

uint last_time = 0;
volatile int number = -1;

#define times_per_second 5


// Estrutura para representar um pixel com componentes RGB
struct pixel_t { 
    uint32_t G, R, B;                // Componentes de cor: Verde, Vermelho e Azul
};


typedef struct pixel_t pixel_t;     // Alias para a estrutura pixel_t
typedef pixel_t npLED_t;            // Alias para facilitar o uso no contexto de LEDs

npLED_t leds[LED_COUNT];            // Array para armazenar o estado de cada LED
PIO np_pio;                         // Variável para referenciar a instância PIO usada
uint sm;                            // Variável para armazenar o número do state machine usado

int frame0[5][5][3] = {
                {{0, 0, 0}, {0, 50, 0}, {0, 50, 0}, {0, 50, 0}, {0, 0, 0}},
                {{0, 0, 0}, {0, 50, 0}, {0, 0, 0}, {0, 50, 0}, {0, 0, 0}},
                {{0, 0, 0}, {0, 50, 0}, {0, 0, 0}, {0, 50, 0}, {0, 0, 0}},
                {{0, 0, 0}, {0, 50, 0}, {0, 0, 0}, {0, 50, 0}, {0, 0, 0}},
                {{0, 0, 0}, {0, 50, 0}, {0, 50, 0}, {0, 50, 0}, {0, 0, 0}}
                };

int frame1[5][5][3] = {
                {{0, 0, 0}, {0, 0, 0}, {0, 50, 0}, {0, 0, 0}, {0, 0, 0}},
                {{0, 0, 0}, {0, 50, 0}, {0, 50, 0}, {0, 0, 0}, {0, 0, 0}},
                {{0, 0, 0}, {0, 0, 0}, {0, 50, 0}, {0, 0, 0}, {0, 0, 0}},
                {{0, 0, 0}, {0, 0, 0}, {0, 50, 0}, {0, 0, 0}, {0, 0, 0}},
                {{0, 0, 0}, {0, 50, 0}, {0, 50, 0}, {0, 50, 0}, {0, 0, 0}}
                };

int frame2[5][5][3] = {
                {{0, 0, 0}, {0, 50, 0}, {0, 50, 0}, {0, 0, 0}, {0, 0, 0}},
                {{0, 50, 0}, {0, 0, 0}, {0, 0, 0}, {0, 50, 0}, {0, 0, 0}},
                {{0, 0, 0}, {0, 0, 0}, {0, 50, 0}, {0, 0, 0}, {0, 0, 0}},
                {{0, 0, 0}, {0, 50, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
                {{0, 50, 0}, {0, 50, 0}, {0, 50, 0}, {0, 50, 0}, {0, 0, 0}}
                };

int frame3[5][5][3] = {
                {{0, 0, 0}, {0, 50, 0}, {0, 50, 0}, {0, 50, 0}, {0, 50, 0}},
                {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 50, 0}},
                {{0, 0, 0}, {0, 0, 0}, {0, 50, 0}, {0, 50, 0}, {0, 50, 0}},
                {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 50, 0}},
                {{0, 0, 0}, {0, 50, 0}, {0, 50, 0}, {0, 50, 0}, {0, 50, 0}}
                };

int frame4[5][5][3] = {
                {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 50, 0}, {0, 50, 0}},
                {{0, 0, 0}, {0, 0, 0}, {0, 50, 0}, {0, 0, 0}, {0, 50, 0}},
                {{0, 0, 0}, {0, 50, 0}, {0, 50, 0}, {0, 50, 0}, {0, 50, 0}},
                {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 50, 0}},
                {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 50, 0}}
                };

int frame5[5][5][3] = {
                {{0, 0, 0}, {0, 50, 0}, {0, 50, 0}, {0, 50, 0}, {0, 50, 0}},
                {{0, 0, 0}, {0, 50, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
                {{0, 0, 0}, {0, 50, 0}, {0, 50, 0}, {0, 50, 0}, {0, 50, 0}},
                {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 50, 0}},
                {{0, 0, 0}, {0, 50, 0}, {0, 50, 0}, {0, 50, 0}, {0, 50, 0}}
                };

int frame6[5][5][3] = {
                {{0, 0, 0}, {0, 50, 0}, {0, 50, 0}, {0, 50, 0}, {0, 50, 0}},
                {{0, 0, 0}, {0, 50, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
                {{0, 0, 0}, {0, 50, 0}, {0, 50, 0}, {0, 50, 0}, {0, 50, 0}},
                {{0, 0, 0}, {0, 50, 0}, {0, 0, 0}, {0, 0, 0}, {0, 50, 0}},
                {{0, 0, 0}, {0, 50, 0}, {0, 50, 0}, {0, 50, 0}, {0, 50, 0}}
                };

int frame7[5][5][3] = {
                {{0, 0, 0}, {0, 50, 0}, {0, 50, 0}, {0, 50, 0}, {0, 50, 0}},
                {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 50, 0}},
                {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 50, 0}, {0, 0, 0}},
                {{0, 0, 0}, {0, 0, 0}, {0, 50, 0}, {0, 0, 0}, {0, 0, 0}},
                {{0, 0, 0}, {0, 50, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}
                };

int frame8[5][5][3] = {
                {{0, 0, 0}, {0, 50, 0}, {0, 50, 0}, {0, 50, 0}, {0, 0, 0}},
                {{0, 0, 0}, {0, 50, 0}, {0, 0, 0}, {0, 50, 0}, {0, 0, 0}},
                {{0, 0, 0}, {0, 50, 0}, {0, 50, 0}, {0, 50, 0}, {0, 0, 0}},
                {{0, 0, 0}, {0, 50, 0}, {0, 0, 0}, {0, 50, 0}, {0, 0, 0}},
                {{0, 0, 0}, {0, 50, 0}, {0, 50, 0}, {0, 50, 0}, {0, 0, 0}}
                };

int frame9[5][5][3] = {
                {{0, 0, 0}, {0, 50, 0}, {0, 50, 0}, {0, 50, 0}, {0, 50, 0}},
                {{0, 0, 0}, {0, 50, 0}, {0, 0, 0}, {0, 0, 0}, {0, 50, 0}},
                {{0, 0, 0}, {0, 50, 0}, {0, 50, 0}, {0, 50, 0}, {0, 50, 0}},
                {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 50, 0}},
                {{0, 0, 0}, {0, 50, 0}, {0, 50, 0}, {0, 50, 0}, {0, 50, 0}}
                };


int getIndex(int x, int y) {
    // Se a linha for par (0, 2, 4), percorremos da esquerda para a direita.
    // Se a linha for ímpar (1, 3), percorremos da direita para a esquerda.
    if (y % 2 == 0) {
        return 24-(y * 5 + x); // Linha par (esquerda para direita).
    } else {
        return 24-(y * 5 + (4 - x)); // Linha ímpar (direita para esquerda).
    }
}

// Função para inicializar o PIO para controle dos LEDs
void npInit(uint pin) 
{
    uint offset = pio_add_program(pio0, &ws2818b_program); // Carregar o programa PIO
    np_pio = pio0;                                         // Usar o primeiro bloco PIO

    sm = pio_claim_unused_sm(np_pio, false);              // Tentar usar uma state machine do pio0
    if (sm < 0)                                           // Se não houver disponível no pio0
    {
        np_pio = pio0;                                    // Mudar para o pio0
        sm = pio_claim_unused_sm(np_pio, true);           // Usar uma state machine do pio0
    }

    ws2818b_program_init(np_pio, sm, offset, pin, 800000.f); // Inicializar state machine para LEDs

    for (uint i = 0; i < LED_COUNT; ++i)                  // Inicializar todos os LEDs como apagados
    {
        leds[i].R = 0;
        leds[i].G = 0;
        leds[i].B = 0;
    }
}

// Função para definir a cor de um LED específico
void npSetLED(const uint index, const uint8_t r, const uint8_t g, const uint8_t b) 
{
    leds[index].R = r;                                    // Definir componente vermelho
    leds[index].G = g;                                    // Definir componente verde
    leds[index].B = b;                                    // Definir componente azul
}

// Função para limpar (apagar) todos os LEDs
void npClear() 
{
    for (uint i = 0; i < LED_COUNT; ++i)                  // Iterar sobre todos os LEDs
        npSetLED(i, 0, 0, 0);                             // Definir cor como preta (apagado)
}

// Função para atualizar os LEDs no hardware
void npWrite() 
{
    for (uint i = 0; i < LED_COUNT; ++i)                  // Iterar sobre todos os LEDs
    {
        pio_sm_put_blocking(np_pio, sm, leds[i].G<<24);       // Enviar componente verde
        pio_sm_put_blocking(np_pio, sm, leds[i].R<<24);       // Enviar componente vermelho
        pio_sm_put_blocking(np_pio, sm, leds[i].B<<24);       // Enviar componente azul
    }
}


void print_frame(int frame[5][5][3], float b)
{
    for(int linha = 0; linha < 5; linha++){
        for(int coluna = 0; coluna < 5; coluna++){
            int posicao = getIndex(linha, coluna);
            npSetLED(posicao, (frame[coluna][linha][0])*b, (frame[coluna][linha][1])*b, (frame[coluna][linha][2])*b);
        }
    }
    npWrite();
    //sleep_ms(sleep_time);
   // npClear(); 
}


void led_blink(uint led_pin, uint times)
{   
        gpio_put(led_pin, true);
        sleep_ms((1000/times)/2);
        gpio_put(led_pin, false);
        sleep_ms((1000/times)/2);
}

void gpio_irq_handler(uint gpio, uint32_t events)
{
    // Obtém o tempo atual em microssegundos
    uint32_t current_time = to_us_since_boot(get_absolute_time());
    printf("\n interrupção");
    // Verifica se passou tempo suficiente desde o último evento
    if (current_time - last_time > 200000) // 200 ms de debouncing
    {

        last_time = current_time; 

        if(gpio == buttonA)
        {
            printf("\n interrupção em A, number = %d", number);
            number++;
            if(number > 9) number = 0;
        }else if (gpio == buttonB)    
        {
            printf("\n interrupção em B, number = %d", number);
            number--;
            if(number < 0) number = 9;
        }    
 
        switch (number)
        {
        case 0:
            print_frame(frame0, 0.5);
            break;
        case 1:
            print_frame(frame1, 0.5);
            break;
        case 2:
            print_frame(frame2, 0.5);
            break;
        case 3:
            print_frame(frame3, 0.5);
            break;
        case 4:
            print_frame(frame4, 0.5);
            break;
        case 5:
            print_frame(frame5, 0.5);
            break;
        case 6:
            print_frame(frame6, 0.5);
            break;
        case 7:
            print_frame(frame7, 0.5);
            break;
        case 8:
            print_frame(frame8, 0.5);
            break;
        case 9:
            print_frame(frame9, 0.5);
            break;
        default:
            npClear();
            npWrite();
            break;
        }      
    }
}

int main()
{   
    stdio_init_all();

    gpio_init(ledPin_Red);
    gpio_set_dir(ledPin_Red, GPIO_OUT);

    gpio_init(buttonA);
    gpio_set_dir(buttonA, GPIO_IN);
    gpio_pull_up(buttonA);

    gpio_init(buttonB);
    gpio_set_dir(buttonB, GPIO_IN);
    gpio_pull_up(buttonB);

    npInit(LED_PIN);
    npClear();
    npWrite();

    gpio_set_irq_enabled_with_callback(buttonA, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(buttonB, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);


    while (true) {
        led_blink(ledPin_Red, times_per_second);
    }
}
