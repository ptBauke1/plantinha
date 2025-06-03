#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"

#include "hardware/i2c.h"

// Inclua o header principal da sua biblioteca OLED
#include "include/ssd1306/SSD1306_OLED.hpp" // Ajuste o caminho conforme sua estrutura

#include <stdio.h>

#define PIN_RELE 2
#define PIN_SENSOR 26

#define ADC_MAXREADING 1800
#define ADC_MINREADING 1000

#define HUMIDITY_THRESHOLD 0.7f // Limite de umidade para ativar o relé (50%)

struct repeating_timer timer;

uint16_t adc_raw_value = 0; // Variável para armazenar o valor lido do ADC
float humidity_percentage = 0.0f; // Variável para armazenar a umidade em porcentagem


bool repeating_timer_callback(struct repeating_timer *t) {
    adc_raw_value = adc_read(); // Lê o valor do ADC
    return true;
}

void convert_adc(){
    // Converte o valor lido do ADC para porcentagem de umidade (0 a 1.0 - inversalmente proporcional)
    if (adc_raw_value >= ADC_MAXREADING) {
        humidity_percentage = 0.0f; // Se o valor for máximo, a umidade é 0%
    } else if (adc_raw_value <= ADC_MINREADING) {
        humidity_percentage = 1.0f; // Se o valor for mínimo, a umidade é 100%
    } else {
        // Interpolação linear para converter o valor do ADC em porcentagem de umidade
        humidity_percentage = 1.0f - ((float)(adc_raw_value - ADC_MINREADING) / (ADC_MAXREADING - ADC_MINREADING));
    }
}

void control_relay() {
    // Controla o relé com base na umidade, ligando o relé por um tempo fixo e desligando-o
    if (humidity_percentage < HUMIDITY_THRESHOLD) { // Se a umidade for menor que 50%
        gpio_put(PIN_RELE, true); // Liga o relé
        sleep_ms(1500); // Mantém o relé ligado por 1 segundo
        gpio_put(PIN_RELE, false); // Desliga o relé
    }
}


// --- Configurações Globais para o OLED e Wrappers ---
const int16_t OLED_WIDTH = 128;
const int16_t OLED_HEIGHT = 32; // Mude para 32 se for um display 128x32

// Buffer para o display
uint8_t oled_main_buffer[OLED_WIDTH * (OLED_HEIGHT / 8)];

// Objeto OLED Global (usado pelas funções wrapper)
SSD1306 oled(OLED_WIDTH, OLED_HEIGHT);
bool oled_is_initialized = false;

// Configurações I2C Padrão (podem ser sobrescritas na função oled_init)
#define DEFAULT_I2C_INSTANCE i2c0
#define DEFAULT_I2C_SDA_PIN 16
#define DEFAULT_I2C_SCL_PIN 17
#define DEFAULT_I2C_CLOCK_SPEED 100 // kHz
#define DEFAULT_OLED_I2C_ADDRESS SSD1306_ADDR // 0x3C

// --- Funções Wrapper para Simplificar o Uso do OLED ---

/**
 * @brief Inicializa o display OLED.
 * Configura o buffer e inicializa a comunicação I2C com o display.
 * Deve ser chamada uma vez antes de usar as outras funções oled_*.
 */
bool oled_init(i2c_inst_t* i2c_instance = DEFAULT_I2C_INSTANCE,
               uint8_t sda_pin = DEFAULT_I2C_SDA_PIN,
               uint8_t scl_pin = DEFAULT_I2C_SCL_PIN,
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
        convert_adc(); // Converte o valor lido do ADC para porcentagem de umidade
        control_relay(); // Controla o relé com base na umidade
        printf("ADC Raw: %d, Humidity: %.2f%%\n", adc_raw_value, humidity_percentage * 100.0f); // Debug output
        sleep_ms(100); // Aguarda 1 segundo antes da próxima leitura
    }

    return 0;
}
