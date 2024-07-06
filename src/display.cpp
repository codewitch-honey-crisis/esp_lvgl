#define LCD_IMPLEMENTATION
#include <lcd_init.h>
#include <display.hpp>
#include <lvgl.h>
// our transfer buffers
// For screens with no DMA we only 
// have one buffer
#ifdef LCD_DMA
uint8_t* lcd_buffer1=nullptr;
uint8_t* lcd_buffer2=nullptr;
#else
uint8_t* lcd_buffer1=nullptr;
#endif
static lv_display_t * display_lvgl=nullptr;
#ifdef LCD_DMA
// only needed if DMA enabled
static bool lcd_flush_ready(esp_lcd_panel_io_handle_t panel_io, 
                            esp_lcd_panel_io_event_data_t* edata, 
                            void* user_ctx) {
    if(display_lvgl!=nullptr) {
        lv_display_flush_ready(display_lvgl);
    }
    return true;
}
#endif
static void display_flush( lv_display_t *disp, const lv_area_t *area, uint8_t * px_map) {
    lcd_panel_draw_bitmap(area->x1,area->y1,area->x2,area->y2,px_map);
#ifndef LCD_DMA
    lv_display_flush_ready(disp);
#endif
}
static uint32_t display_ticks() {
    return pdTICKS_TO_MS(xTaskGetTickCount());
}
void display_init() {
#ifdef LCD_PSRAM_BUFFER
    lcd_buffer1 = (uint8_t*)heap_caps_malloc(lcd_buffer_size,MALLOC_CAP_SPIRAM);
#else
    lcd_buffer1 = (uint8_t*)heap_caps_malloc(lcd_buffer_size,MALLOC_CAP_DMA);
#endif
    if(lcd_buffer1==nullptr) {
        puts("Error allocating LCD buffer 1");
        while(1);
    }
#ifdef LCD_DMA
    lcd_buffer2 = (uint8_t*)heap_caps_malloc(lcd_buffer_size,MALLOC_CAP_DMA);
    if(lcd_buffer2==nullptr) {
        puts("Error allocating LCD buffer 2");
        while(1);
    }
#endif
#ifndef LCD_PIN_NUM_VSYNC
    lcd_panel_init(lcd_buffer_size,lcd_flush_ready);
#else
    lcd_panel_init();
#endif
    lv_init();

    lv_tick_set_cb(display_ticks);
    display_lvgl = lv_display_create(LCD_WIDTH, LCD_HEIGHT);
    lv_display_set_flush_cb(display_lvgl, display_flush);
    lv_display_set_buffers(display_lvgl, lcd_buffer1, lcd_buffer2, lcd_buffer_size, LV_DISPLAY_RENDER_MODE_PARTIAL);
}

void display_update() {
    lv_timer_handler();
}