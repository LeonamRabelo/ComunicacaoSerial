#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/uart.h"
#include "ws2812.pio.h"
#include "inc/ssd1306.h"
#include "inc/font.h"

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C

#define IS_RGBW false
#define NUM_PIXELS 25
#define NUM_NUMBERS 11
#define BOTAO_A 5
#define BOTAO_B 6
#define WS2812_PIN 7
#define LED_PIN_RED 13
#define LED_PIN_BLUE 12
#define LED_PIN_GREEN 11

ssd1306_t ssd; // Inicializa a estrutura do display
static volatile uint32_t last_time = 0; // Armazena o tempo do último evento (em microssegundos)
volatile uint numero = 0;//variavel para inicializar o numero com 0, vai ser alterada nas interrupções (volatile)

// Variável global para armazenar a cor (Entre 0 e 255 para intensidade)
uint8_t led_r = 0; // Intensidade do vermelho
uint8_t led_g = 0; // Intensidade do verde
uint8_t led_b = 200; // Intensidade do azul

static inline void put_pixel(uint32_t pixel_grb){
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b){
    return ((uint32_t)(r) << 8) | ((uint32_t)(g) << 16) | (uint32_t)(b);
}

bool led_numeros[NUM_NUMBERS][NUM_PIXELS] = {
    //Número 0
    {
    0, 1, 1, 1, 0,      
    0, 1, 0, 1, 0, 
    0, 1, 0, 1, 0,   
    0, 1, 0, 1, 0,  
    0, 1, 1, 1, 0   
    },

    //Número 1
    {0, 1, 1, 1, 0,      
    0, 0, 1, 0, 0, 
    0, 0, 1, 0, 0,    
    0, 1, 1, 0, 0,  
    0, 0, 1, 0, 0   
    },

    //Número 2
    {0, 1, 1, 1, 0,      
    0, 1, 0, 0, 0, 
    0, 1, 1, 1, 0,    
    0, 0, 0, 1, 0,
    0, 1, 1, 1, 0   
    },

    //Número 3
    {0, 1, 1, 1, 0,      
    0, 0, 0, 1, 0, 
    0, 1, 1, 1, 0,    
    0, 0, 0, 1, 0,  
    0, 1, 1, 1, 0   
    },

    //Número 4
    {0, 1, 0, 0, 0,      
    0, 0, 0, 1, 0, 
    0, 1, 1, 1, 0,    
    0, 1, 0, 1, 0,     
    0, 1, 0, 1, 0   
    },

    //Número 5
    {0, 1, 1, 1, 0,      
    0, 0, 0, 1, 0, 
    0, 1, 1, 1, 0,   
    0, 1, 0, 0, 0,  
    0, 1, 1, 1, 0   
    },

    //Número 6
    {0, 1, 1, 1, 0,      
    0, 1, 0, 1, 0, 
    0, 1, 1, 1, 0,    
    0, 1, 0, 0, 0,  
    0, 1, 1, 1, 0   
    },

    //Número 7
    {0, 1, 0, 0, 0,      
    0, 0, 0, 1, 0,   
    0, 1, 0, 0, 0,    
    0, 0, 0, 1, 0,  
    0, 1, 1, 1, 0  
    },

    //Número 8
    {0, 1, 1, 1, 0,      
    0, 1, 0, 1, 0, 
    0, 1, 1, 1, 0,    
    0, 1, 0, 1, 0,  
    0, 1, 1, 1, 0   
    },

    //Número 9
    {0, 1, 1, 1, 0,      
    0, 0, 0, 1, 0, 
    0, 1, 1, 1, 0,    
    0, 1, 0, 1, 0,  
    0, 1, 1, 1, 0   
    },

    //APAGAR OS LEDS, representado pelo número (posição) 10
    {0, 0, 0, 0, 0,      
    0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0,    
    0, 0, 0, 0, 0,  
    0, 0, 0, 0, 0   
    }
};

void set_one_led(uint8_t r, uint8_t g, uint8_t b, int numero){
    //Define a cor com base nos parâmetros fornecidos
    uint32_t color = urgb_u32(r, g, b);

    //Define todos os LEDs com a cor especificada
    for(int i = 0; i < NUM_PIXELS; i++){
        if(led_numeros[numero][i]){ //Chama a matriz de leds com base no numero passado
            put_pixel(color); //Liga o LED com um no buffer
        }else{
            put_pixel(0);  //Desliga os LEDs com zero no buffer
        }
    }
}

void inicializarGPIOs(){
    stdio_init_all();

    //Inicializa pinos dos leds RGB
    gpio_init(LED_PIN_GREEN);
    gpio_set_dir(LED_PIN_GREEN, GPIO_OUT);
    gpio_put(LED_PIN_GREEN, 0);
    gpio_init(LED_PIN_BLUE);
    gpio_set_dir(LED_PIN_BLUE, GPIO_OUT);
    gpio_put(LED_PIN_BLUE, 0);
    gpio_init(LED_PIN_RED);
    gpio_set_dir(LED_PIN_RED, GPIO_OUT);
    gpio_put(LED_PIN_RED, 0);

    //Inicializa pinos dos botoes
    gpio_init(BOTAO_A);
    gpio_set_dir(BOTAO_A, GPIO_IN);
    gpio_pull_up(BOTAO_A);
    gpio_init(BOTAO_B); 
    gpio_set_dir(BOTAO_B, GPIO_IN);
    gpio_pull_up(BOTAO_B);

    //Inicializa o pio
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
    gpio_pull_up(I2C_SDA); // Pull up the data line
    gpio_pull_up(I2C_SCL); // Pull up the clock line
}

//Função de interrupção com Debouncing
void gpio_irq_handler(uint gpio, uint32_t events){
uint32_t current_time = to_us_since_boot(get_absolute_time());

    if(current_time - last_time > 200000){ //200 ms de debouncing
    last_time = current_time; //Atualiza o tempo do último evento
    
        //Caso o botão A seja pressionado
        if(gpio == BOTAO_A){
            //Alterna o estado do LED
            gpio_put(LED_PIN_GREEN, !gpio_get(LED_PIN_GREEN));
            //Verifica se o led está ativo ou nao e envia uma mensagem ao Serial Monitor
            gpio_get(LED_PIN_GREEN) ? printf("LED Verde Aceso\n") : printf("LED Verde Desligado\n");
        }
        //Caso o botão B seja pressionado
        if(gpio == BOTAO_B){
            //Alterna o estado do LED
            gpio_put(LED_PIN_BLUE, !gpio_get(LED_PIN_BLUE));
            //Verifica se o led está ativo ou nao e envia uma mensagem ao Serial Monitor
            gpio_get(LED_PIN_BLUE) ? printf("LED Azul Aceso\n") : printf("LED Azul Desligado\n");

        }
    }

    ssd1306_fill(&ssd, false); //Limpa o display                    
    //Imprime estado atual dos leds no display
    gpio_get(LED_PIN_GREEN)?ssd1306_draw_string(&ssd, "LED Verde ON", 10, 10) :
                            ssd1306_draw_string(&ssd, "LED Verde OFF", 10, 10);//Desenha um caracter
    gpio_get(LED_PIN_BLUE)?ssd1306_draw_string(&ssd, "LED Azul ON", 10, 30) :
                            ssd1306_draw_string(&ssd, "LED Azul OFF", 10, 30);//Desenha um caracter
    ssd1306_send_data(&ssd); //Atualiza o display
}


int main(){
    inicializarGPIOs();

    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); // Inicializa o display
    ssd1306_config(&ssd); // Configura o display
    ssd1306_send_data(&ssd); // Envia os dados para o display
    // Limpa o display. O display inicia com todos os pixels apagados.
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);
    //Configuração da interrupção com callback para os botoes A e B
    gpio_set_irq_enabled_with_callback(BOTAO_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(BOTAO_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    ssd1306_fill(&ssd, false); //Limpa o display
    ssd1306_draw_string(&ssd, "BAHIA is World", 10, 30); //Desenha um caracter
    ssd1306_send_data(&ssd); //Atualiza o display

    set_one_led(led_r, led_g, led_b, 10);   //Apagar matriz de led, não guardar valores da ultima vez acionado

    while(true){
    if(stdio_usb_connected()){    //Verifica se há conexão no usb, se for simular no wokwi, retire essa linha
    char c; //Timeout de 1ms para evitar travamento
    //Se um caractere foi recebido
    if (scanf("%c", &c)==1){
        set_one_led(led_r, led_g, led_b, 10);   //Apagar matriz de led caso nao seja passado um numero
        printf("Caractere recebido: %c\n", c);
        //Exibe o caractere no display SSD1306
        //Atualiza o conteúdo do display com animações
        ssd1306_fill(&ssd, false); //Limpa o display
        ssd1306_draw_char(&ssd, c, 20, 30); //Desenha um caracter
        ssd1306_send_data(&ssd); //Atualiza o display

        //Se for um número de 0 a 9, exibir na matriz de LEDs
        if (c >= '0' && c <= '9') {
            int numero = c - '0'; // Converte char para int (ex: '5' → 5)
            set_one_led(led_r, led_g, led_b, numero);
        }
    }
    }
    }
}