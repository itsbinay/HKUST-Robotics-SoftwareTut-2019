#ifndef __LCD_MAIN_H
#define __LCD_MAIN_H

// #define MODEL_ILI9486
// 2.8/3.2inch
// #define MODEL_ILI9341
// 1.8inch
#define MODEL_ST7735

#include "stm32f1xx.h"
#include "gpio.h"
#include "spi.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>

typedef enum {
	PIN_ON_TOP,
	PIN_ON_LEFT,
	PIN_ON_BOTTOM,
	PIN_ON_RIGHT
}TFT_ORIENTATION;

/* GPIO settings */

/* Colors */
#define	RGB888TO565(RGB888)  (((RGB888 >> 8) & 0xF800) |((RGB888 >> 5) & 0x07E0) | ((RGB888 >> 3) & 0x001F))

#define WHITE           (RGB888TO565(0xFFFFFF))
#define BLACK           (RGB888TO565(0x000000))
#define DARK_GREY       (RGB888TO565(0x555555))
#define GREY            (RGB888TO565(0xAAAAAA))
#define RED             (RGB888TO565(0xFF0000))
#define DARK_RED        (RGB888TO565(0x800000))
#define ORANGE          (RGB888TO565(0xFF9900))
#define YELLOW          (RGB888TO565(0xFFFF00))
#define GREEN           (RGB888TO565(0x00FF00))
#define DARK_GREEN      (RGB888TO565(0x00CC00))
#define BLUE            (RGB888TO565(0x0000FF))
#define BLUE2           (RGB888TO565(0x202060))
#define SKY_BLUE        (RGB888TO565(0x11CFFF))
#define CYAN            (RGB888TO565(0x8888FF))
#define PURPLE          (RGB888TO565(0x00AAAA))
#define PINK            (RGB888TO565(0xC71585))
#define GRAYSCALE(S)         (2113*S)

/* Pixels */
#define CHAR_WIDTH              8
#define CHAR_HEIGHT             16

#ifdef MODEL_ILI9486
	#define SCREEN_CHOSEN
	#define MAX_WIDTH               320
	#define MAX_HEIGHT              480

	#define PIXEL_BYTES             3

	// #define CHAR_BUF_SIZE           600

	#define CHAR_MAX_X_VERTICAL     40
	#define CHAR_MAX_Y_VERTICAL     30

	#define CHAR_MAX_X_HORIZONTAL   60
	#define CHAR_MAX_Y_HORIZONTAL   20

	#define CHAR_MAX_X              60      // max between CHAR_MAX_X_VERTICAL and CHAR_MAX_X_HORIZONTAL
	#define CHAR_MAX_Y              30      // max between CHAR_MAX_Y_VERTICAL and CHAR_MAX_Y_HORIZONTAL

#endif

#ifdef MODEL_ILI9341
	#define SCREEN_CHOSEN
	#define MAX_WIDTH               240
	#define MAX_HEIGHT              320

	// #define CHAR_BUF_SIZE           600
	#define PIXEL_BYTES             2

	#define CHAR_MAX_X_VERTICAL     30
	#define CHAR_MAX_Y_VERTICAL     20

	#define CHAR_MAX_X_HORIZONTAL   40
	#define CHAR_MAX_Y_HORIZONTAL   15

	#define CHAR_MAX_X              40      // max between CHAR_MAX_X_VERTICAL and CHAR_MAX_X_HORIZONTAL
	#define CHAR_MAX_Y              20      // max between CHAR_MAX_Y_VERTICAL and CHAR_MAX_Y_HORIZONTAL

#endif

#ifdef MODEL_ST7735
	#define SCREEN_CHOSEN
	#define MAX_WIDTH               128
	#define MAX_HEIGHT              160

	#define PIXEL_BYTES             2
	// #define CHAR_BUF_SIZE           600

	#define CHAR_MAX_X_VERTICAL     16
	#define CHAR_MAX_Y_VERTICAL     10

	#define CHAR_MAX_X_HORIZONTAL   20
	#define CHAR_MAX_Y_HORIZONTAL   8

	#define CHAR_MAX_X              20      // max between CHAR_MAX_X_VERTICAL and CHAR_MAX_X_HORIZONTAL
	#define CHAR_MAX_Y              10      // max between CHAR_MAX_Y_VERTICAL and CHAR_MAX_Y_HORIZONTAL

#endif

#ifndef SCREEN_CHOSEN
#error "You must choose which screen you are using at the top"
#endif

extern uint8_t tft_orientation;
extern uint16_t curr_bg_color;
extern uint16_t curr_text_color;
extern uint16_t curr_text_color_sp;
extern uint16_t curr_highlight_color;

extern uint8_t char_max_x, char_max_y;

typedef struct {
	uint8_t curr_screen;
#if 0
	char text_buf[2][CHAR_MAX_X][CHAR_MAX_Y];
	uint16_t text_color_buf[2][CHAR_MAX_X][CHAR_MAX_Y];
	uint16_t bg_color_buf[2][CHAR_MAX_X][CHAR_MAX_Y];
#else
	char text_buf[2][CHAR_MAX_X_VERTICAL*CHAR_MAX_Y_VERTICAL];
	uint16_t text_color_buf[2][CHAR_MAX_X_VERTICAL*CHAR_MAX_Y_VERTICAL];
	uint16_t bg_color_buf[2][CHAR_MAX_X_VERTICAL*CHAR_MAX_Y_VERTICAL];
#endif
} TFTBuffer_STRUCT, *TFTBuffer;
#define TFT_XY(x, y) ((y)*char_max_x + (x))

static inline bool tft_char_is_changed(const TFTBuffer b, uint8_t x, uint8_t y){
	// return true;
	return (b->text_buf[b->curr_screen][TFT_XY(x,y)] != b->text_buf[!b->curr_screen][TFT_XY(x,y)] \
		|| (b->text_color_buf[b->curr_screen][TFT_XY(x,y)] != b->text_color_buf[!b->curr_screen][TFT_XY(x,y)]) \
		|| (b->bg_color_buf[b->curr_screen][TFT_XY(x,y)] != b->bg_color_buf[!b->curr_screen][TFT_XY(x,y)]));
}

extern TFTBuffer_STRUCT default_tft_screens;
extern TFTBuffer tft_screens;


/* TFT communication control */
void tft_init(TFT_ORIENTATION orientation, uint16_t in_bg_color, uint16_t in_text_color, uint16_t in_text_color_sp, uint16_t in_highlight_color); //, DISPLAY_MODE mode);

/* Display properties configuration */
void tft_set_bg_color(uint16_t in_bg_color);
void tft_set_text_color(uint16_t in_text_color);
void tft_set_highlight_color(uint16_t sp_bg_color);
void tft_set_special_color(uint16_t text_color_sp);

/* Pixel printing */
void tft_force_clear(void);
void tft_print_rectangle(uint16_t color, uint32_t x, uint32_t y, uint32_t w, uint32_t h);
void tft_print_pixel(uint16_t color, uint32_t x, uint32_t y);
void tft_print_image(void* buf, uint32_t x, uint32_t y, uint32_t w, uint32_t h);

/* Text buffer printing */
void tft_printc(uint8_t x, uint8_t y, char * fp);
void tft_prints(uint8_t x, uint8_t y, const char * pstr, ...);
void tft_printi(uint8_t x, uint8_t y, int32_t num);
void tft_printl(uint8_t x, uint8_t y, int64_t num);
void tft_printu(uint8_t x, uint8_t y, uint32_t num);
void tft_printf(uint8_t x, uint8_t y, float num, uint32_t dp);
uint8_t tft_update(uint32_t period);
uint8_t tft_update2(uint32_t period);

#endif		/* __LCD_MAIN_H */
