#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include <stdio.h>

#define PIN_RELE 2
#define PIN_SENSOR 26

#define ADC_MAXREADING 1800
#define ADC_MINREADING 100

#define HUMIDITY_THRESHOLD 0.5f // Limite de umidade para ativar o relé (50%)

struct repeating_timer timer;

// --- Definições dos Pinos do LCD (AJUSTADO CONFORME SUA IMAGEM) ---
const uint RS_PIN = 16; // Pino Register Select (RS) - Conectado ao GP18
const uint E_PIN  = 17; // Pino Enable (E) - Conectado ao GP17
const uint D4_PIN = 18; // Pino de Dado 4 - Conectado ao GP27
const uint D5_PIN = 19; // Pino de Dado 5 - Conectado ao GP21
const uint D6_PIN = 20; // Pino de Dado 6 - Conectado ao GP20
const uint D7_PIN = 21; // Pino de Dado 7 - Conectado ao GP16



uint16_t adc_raw_value = 0; // Variável para armazenar o valor lido do ADC
float humidity_percentage = 0.0f; // Variável para armazenar a umidade em porcentagem


bool repeating_timer_callback(struct repeating_timer *t) {
    adc_raw_value = adc_read(); // Lê o valor do ADC
    return true;
}

void convert_adc(){
    // Converte o valor lido do ADC para porcentagem de umidade (0 a 1.0)
    if (adc_raw_value > ADC_MAXREADING) {
        adc_raw_value = ADC_MAXREADING; // Limita o valor máximo
    } else if (adc_raw_value < ADC_MINREADING) {
        adc_raw_value = ADC_MINREADING; // Limita o valor mínimo
    }
    humidity_percentage = ((float)(adc_raw_value - ADC_MINREADING) / (ADC_MAXREADING - ADC_MINREADING));
}

void control_relay() {
    // Controla o relé com base na umidade, ligando o relé por um tempo fixo e desligando-o
    if (humidity_percentage < HUMIDITY_THRESHOLD) { // Se a umidade for menor que 50%
        gpio_put(PIN_RELE, true); // Liga o relé
        sleep_ms(1500); // Mantém o relé ligado por 1 segundo
        gpio_put(PIN_RELE, false); // Desliga o relé
    }
}
// --- Função Principal ---
int main() {
    stdio_init_all(); // Inicializa stdio para debug (opcional)
    gpio_init(PIN_RELE); // Inicializa o pino do relé
    gpio_set_dir(PIN_RELE, GPIO_OUT); // Define o pino do relé como saída
    
    adc_init(); // Inicializa o ADC
    adc_gpio_init(PIN_SENSOR); // Inicializa o pino do sensor como ADC
    adc_select_input(0); // Seleciona o canal ADC 0 (pino 26)

    if (!add_repeating_timer_us(-10000, repeating_timer_callback, NULL, &timer)) {
        printf("Falha ao adicionar o timer!\n");
        return 1;
    }

    while (true) {
        // Seu código principal pode ir aqui
        convert_adc(); // Converte o valor do ADC para porcentagem de umidade
        control_relay(); // Controla o relé com base na umidade
    }

    return 0;
}