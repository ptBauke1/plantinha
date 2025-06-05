#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"

#include "hardware/i2c.h"

// Inclua o header principal da sua biblioteca OLED
#include "include/ssd1306/SSD1306_OLED.hpp" // Ajuste o caminho conforme sua estrutura

#include <stdio.h>

#define PIN_RELE 2
#define PIN_SENSOR 26

#define ADC_MAXREADING 3500 // -> Solo seco (valor máximo do ADC)
#define ADC_MINREADING 600 // -> sensor imerso em água (valor mínimo do ADC)

#define HUMIDITY_THRESHOLD 40.0f // Limite de umidade para ativar o relé (40%)
#define PUMP_INTERVAL_US 2000000 // Intervalo de tempo para o relé (1,5 segundo)
#define RELE_INTERVAL_US 120000000 // Intervalo de tempo para o relé 
// 1,5 segundos em us = 1.5 * 1000000 = 1500000 us
// 2 minutos em us = 2 * 60 * 1000000 = 120000000 us

struct repeating_timer timer;

uint16_t adc_raw_value = 0; // Variável para armazenar o valor lido do ADC
float humidity_percentage = 0.0f; // Variável para armazenar a umidade em porcentagem
uint32_t pump_timer = 0; // Timer para controlar o relé
uint32_t rele_time = 0; // Tempo do último acionamento do relé
bool flag_rele = true; // Flag para controlar o estado do relé
// --- Configurações Globais para o OLED e Wrappers ---
const int16_t OLED_WIDTH = 128;
const int16_t OLED_HEIGHT = 32; // Mude para 32 se for um display 128x32

// Buffer para o display
uint8_t oled_main_buffer[OLED_WIDTH * (OLED_HEIGHT / 8)];

// Objeto OLED Global (usado pelas funções wrapper)
SSD1306 oled(OLED_WIDTH, OLED_HEIGHT);
bool oled_is_initialized = false;

// Configurações I2C Padrão (podem ser sobrescritas na função oled_init)
#define I2C_INSTANCE i2c0
#define I2C_SDA_PIN 16
#define I2C_SCL_PIN 17
#define DEFAULT_I2C_CLOCK_SPEED 100 // kHz
#define DEFAULT_OLED_I2C_ADDRESS SSD1306_ADDR // 0x3C


bool repeating_timer_callback(struct repeating_timer *t) {
    adc_raw_value = adc_read(); // Lê o valor do ADC
    return true;
}

void convert_adc(){
    // Converte o valor lido do ADC para porcentagem de umidade (0 a 100%)
    if (adc_raw_value >= ADC_MAXREADING) {
        humidity_percentage = 0.0f; // Garante que o valor não seja maior que o máximo
    } else if (adc_raw_value <= ADC_MINREADING) {
        humidity_percentage = 100.0f; // Garante que o valor não seja menor que o mínimo
    } else {
        // Converte o valor do ADC para porcentagem de umidade
        humidity_percentage = ((float)(ADC_MAXREADING - adc_raw_value) / (ADC_MAXREADING - ADC_MINREADING)) * 100.0f;
    }
}

void control_relay() {
    // Controla o relé com base na umidade, ligando o relé por um tempo, mas sem travar o programa
    // depois que o relé desativar, trava o relé por um tempo definido, para o sensor conseguir estabilizar
    if (!flag_rele) { 
        if (time_us_32() - rele_time >= RELE_INTERVAL_US) {
            flag_rele = true; // Destrava a possibilidade de ativar o relé
            gpio_put(PIN_RELE, false); // Garante que o relé esteja desligado
        }
        else {
            return;
        }
    }
    if (humidity_percentage < HUMIDITY_THRESHOLD) {
        // Se a umidade estiver abaixo do limite, ativa o relé
        gpio_put(PIN_RELE, true);
        pump_timer = time_us_32();
    }
    if (time_us_32() - pump_timer >= PUMP_INTERVAL_US) {
        gpio_put(PIN_RELE, false);
        flag_rele = false; // Desativa o relé
        rele_time = time_us_32(); // Atualiza o tempo do relé
        }
}

// --- Funções Wrapper para Simplificar o Uso do OLED ---

/**
 * @brief Inicializa o display OLED.
 * Configura o buffer e inicializa a comunicação I2C com o display.
 * Deve ser chamada uma vez antes de usar as outras funções oled_*.
 */
bool oled_init(i2c_inst_t* i2c_instance = I2C_INSTANCE,
               uint8_t sda_pin = I2C_SDA_PIN,
               uint8_t scl_pin = I2C_SCL_PIN,
               uint16_t clk_speed_khz = DEFAULT_I2C_CLOCK_SPEED,
               uint8_t i2c_address = DEFAULT_OLED_I2C_ADDRESS) {
    if (oled_is_initialized) {
        printf("OLED ja inicializado.\n");
        return true;
    }

    uint8_t buffer_status = oled.OLEDSetBufferPtr(OLED_WIDTH, OLED_HEIGHT, oled_main_buffer, sizeof(oled_main_buffer));
    if (buffer_status != 0) {
        printf("oled_init: Erro ao configurar o buffer: %d\n", buffer_status);
        return false;
    }

    if (!oled.OLEDbegin(i2c_address, i2c_instance, clk_speed_khz, sda_pin, scl_pin)) {
        printf("oled_init: Falha ao iniciar o OLED via OLEDbegin.\n");
        return false;
    }

    oled_is_initialized = true;
    printf("OLED inicializado com sucesso.\n");
    return true;
}

/**
 * @brief Limpa o buffer do display.
 * Por padrão, atualiza o display (envia o buffer limpo).
 */
void oled_clear(bool update_now = true) {
    if (!oled_is_initialized) {
        printf("oled_clear: OLED nao inicializado.\n");
        return;
    }
    oled.OLEDclearBuffer();
    if (update_now) {
        oled.OLEDupdate();
    }
}

/**
 * @brief Envia o conteúdo do buffer para o display OLED.
 */
void oled_display() {
    if (!oled_is_initialized) {
        printf("oled_display: OLED nao inicializado.\n");
        return;
    }
    oled.OLEDupdate();
}

/**
 * @brief Escreve uma string de texto (pré-formatada) no display.
 *
 * @param x Posição X inicial do texto.
 * @param y Posição Y inicial do texto.
 * @param formatted_text A string de texto já formatada a ser escrita.
 * @param font Ponteiro para a fonte a ser usada (default: pFontDefault).
 * Use nullptr para manter a fonte atualmente selecionada pela biblioteca.
 * @param update_now Se true, atualiza o display imediatamente.
 */
void oled_write_text(int16_t x, int16_t y, const char* formatted_text, const uint8_t* font = pFontDefault, bool update_now = true) {
    if (!oled_is_initialized) {
        printf("oled_write_text: OLED nao inicializado.\n");
        return;
    }
    if (font != nullptr) {
        oled.setFont(font);
    }
    oled.setCursor(x, y);
    oled.print(formatted_text); // oled.print() da classe Print herdada.
    if (update_now) {
        oled_display();
    }
}

/**
 * @brief Desenha um pixel no buffer do display.
 */
void oled_draw_pixel(int16_t x, int16_t y, uint8_t color, bool update_now = true) {
    if (!oled_is_initialized) {
        printf("oled_draw_pixel: OLED nao inicializado.\n");
        return;
    }
    oled.drawPixel(x, y, color);
    if (update_now) {
        oled_display();
    }
}

char adc_text_buffer[32];
char humidity_text_buffer[32];

// --- Função Principal ---
int main() {
    stdio_init_all(); // Inicializa stdio para debug (opcional)
    gpio_init(PIN_RELE); // Inicializa o pino do relé
    gpio_set_dir(PIN_RELE, GPIO_OUT); // Define o pino do relé como saída
    
    adc_init(); // Inicializa o ADC
    adc_gpio_init(PIN_SENSOR); // Inicializa o pino do sensor como ADC
    adc_select_input(0); // Seleciona o canal ADC 0 (pino 26)

    if (!add_repeating_timer_us(-500000, repeating_timer_callback, NULL, &timer)) {
        printf("Falha ao adicionar o timer!\n");
        return 1;
    }

    // Inicializa o OLED
    if (!oled_init(I2C_INSTANCE, I2C_SDA_PIN, I2C_SCL_PIN, DEFAULT_I2C_CLOCK_SPEED, DEFAULT_OLED_I2C_ADDRESS)) {
        printf("Erro ao inicializar o OLED.\n");
        return 1;
    }

    oled_clear(); // Limpa o buffer do OLED

    while (true) {
        convert_adc(); // Converte o valor do ADC para porcentagem de umidade
        control_relay(); // Controla o relé com base na umidade
        snprintf(adc_text_buffer, sizeof(adc_text_buffer), "ADC: %d", adc_raw_value);
        snprintf(humidity_text_buffer, sizeof(humidity_text_buffer), "Umidade: %.2f%%", humidity_percentage);
        // Atualiza o OLED com os valores lidos
        oled_clear(false); // Limpa o buffer do OLED
        oled_write_text(5, 12, adc_text_buffer, pFontDefault, false);      // Escreve valor do ADC, não atualiza
        oled_write_text(5, 22, humidity_text_buffer, pFontDefault, false); // Escreve valor do contador, não atualiza
        oled_display(); // Atualiza o OLED com os dados do buffer
        sleep_ms(250);
    }   

    return 0;
}
