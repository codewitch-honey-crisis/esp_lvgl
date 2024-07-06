#pragma once
#if __has_include(<Arduino.h>)
#include <Arduino.h>
#endif
#ifndef E_PAPER
#ifdef ESP_PLATFORM
#include <lcd_config.h>
#endif
#endif

#define LCD_TRANSFER_KB 64

// here we compute how many bytes are needed in theory to store the total screen.
constexpr static const size_t lcd_screen_total_size = (LCD_WIDTH*LCD_HEIGHT*LCD_BIT_DEPTH+7)/8;
// define our transfer buffer(s) 
// For devices with no DMA we only use one buffer.
// Our total size is either LCD_TRANSFER_KB 
// Or the lcd_screen_total_size - whatever
// is smaller
// Note that in the case of DMA the memory
// is divided between two buffers.

#ifdef LCD_DMA
constexpr static const size_t lcd_buffer_size = (LCD_TRANSFER_KB*512)>lcd_screen_total_size?lcd_screen_total_size:(LCD_TRANSFER_KB*512);
extern uint8_t* lcd_buffer1;
extern uint8_t* lcd_buffer2;
#else
#ifdef LCD_PSRAM_BUFFER
constexpr static const size_t lcd_buffer_size = LCD_PSRAM_BUFFER;
#else
constexpr static const size_t lcd_buffer_size = (LCD_TRANSFER_KB*1024)>lcd_screen_total_size?lcd_screen_total_size:(LCD_TRANSFER_KB*1024);
#endif
extern uint8_t* lcd_buffer1;
static uint8_t* const lcd_buffer2 = nullptr;
#endif

// initializes the display
extern void display_init();
// updates the display, redrawing as necessary
extern void display_update();

