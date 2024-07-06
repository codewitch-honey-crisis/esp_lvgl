#if __has_include(<Arduino.h>)
#include <Arduino.h>
#else
#include <stdio.h>
#include <stdint.h>
#include "esp_lcd_panel_ops.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
extern "C" void app_main();
#endif
#include <lvgl.h>
#include "display.hpp"
#ifdef M5STACK_CORE2
#include <m5core2_power.hpp>
#endif

#ifdef M5STACK_CORE2
m5core2_power power;
#endif

static uint32_t get_ms() {
#ifdef ARDUINO
    return millis();
#else
    return ((uint32_t)pdTICKS_TO_MS(xTaskGetTickCount()));
#endif
}

void update_all() {
    
    display_update();
}
void initialize_common() {
    display_init();
    puts("Booted");
    
     lv_obj_t *label = lv_label_create( lv_scr_act() );
    lv_label_set_text( label, "Hello, I'm LVGL!" );
    lv_obj_align( label, LV_ALIGN_CENTER, 0, 0 );


}
#ifdef ARDUINO
void setup() {
    // enable the power pins, as necessary
#ifdef T_DISPLAY_S3
    pinMode(15, OUTPUT); 
    digitalWrite(15, HIGH);
#elif defined(S3_T_QT)
    pinMode(4, OUTPUT); 
    digitalWrite(4, HIGH);
#endif
#ifdef M5STACK_CORE2
    power.initialize();
#endif

    initialize_common();
}

void loop() {
    update_all();    
}
#else
void loop_task(void* state) {
    while(true) {
        update_all();
        vTaskDelay(5);
    }
}
void app_main() {
    // open stdin as binary
    freopen(NULL, "rb", stdin);
#ifdef T_DISPLAY_S3
    gpio_set_direction((gpio_num_t)15,GPIO_MODE_OUTPUT);
    gpio_set_level((gpio_num_t)15,1);
#elif defined(S3_T_QT)
    gpio_set_direction((gpio_num_t)4,GPIO_MODE_OUTPUT);
    gpio_set_level((gpio_num_t)4,1);
#endif
#ifdef M5STACK_CORE2
    power.initialize();
#endif
    initialize_common();
    TaskHandle_t htask = nullptr;
    xTaskCreate(loop_task,"loop_task",5000,nullptr,uxTaskPriorityGet(nullptr),&htask);
    if(htask==nullptr) {
        printf("Unable to create loop task\n");
    }
    vTaskSuspend(nullptr);
}
#endif