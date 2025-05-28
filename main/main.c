#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"

#include "tft_lcd_ili9341/ili9341/ili9341.h"
#include "tft_lcd_ili9341/gfx/gfx.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define P_BL   14
#define P_ADC  26
#define CH_ADC  1
#define R_FIXO 10000u
#define TAMQ     6

static QueueHandle_t filaDados;

void coleta(void *arg) {
    uint16_t valor;
    for (;;) {
        adc_select_input(CH_ADC);
        valor = adc_read();
        xQueueSend(filaDados, &valor, portMAX_DELAY);
        vTaskDelay(pdMS_TO_TICKS(400));
    }
}

void tela(void *arg) {
    uint16_t entrada;

    for (;;) {
        if (xQueueReceive(filaDados, &entrada, portMAX_DELAY)) {
            uint32_t mv = (uint32_t)entrada * 3300 / 4095;
            uint32_t res = mv > 0 ? (R_FIXO * 3300 / mv - R_FIXO) : 0;
            uint32_t microA = (3300 - mv) * 1000 / R_FIXO;

            GFX_clearScreen();
            GFX_setCursor(8, 12);
            GFX_setTextColor(ILI9341_WHITE);
            GFX_setTextBack(ILI9341_BLACK);
            GFX_printf("U: %4u mV\n", mv);
            GFX_setCursor(8, 38);
            GFX_printf("R: %5u Ohm\n", res);
            GFX_setCursor(8, 64);
            GFX_printf("I: %4u uA\n", microA);
            GFX_setCursor(8, 90);
            GFX_printf("RAW: %u\n", entrada);
            GFX_flush();
        }
    }
}

int main(void) {
    stdio_init_all();
    adc_init();
    adc_gpio_init(P_ADC);

    gpio_init(P_BL);
    gpio_set_dir(P_BL, GPIO_OUT);
    gpio_put(P_BL, 1);

    LCD_initDisplay();
    LCD_setRotation(1);
    GFX_createFramebuf();

    filaDados = xQueueCreate(TAMQ, sizeof(uint16_t));
    if (!filaDados) while (1);

    xTaskCreate(coleta, "T1", 256, NULL, 2, NULL);
    xTaskCreate(tela, "T2", 512, NULL, 1, NULL);

    vTaskStartScheduler();
    while (true);
}
