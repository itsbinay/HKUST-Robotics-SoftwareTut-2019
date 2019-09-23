#include "lcd.h"
#include "lcd_font.h"
#include "lcd_init.h"
#include "dma.h"
#include "strhelper.h"

#if (!defined(TFT_RST_GPIO_Port) && !defined(TFT_RST_Pin)) && (defined(TFT_RES_GPIO_Port) && defined(TFT_RES_Pin))
	#define TFT_RST_GPIO_Port TFT_RES_GPIO_Port
	#define TFT_RST_Pin       TFT_RES_Pin
#endif

extern TFTBuffer_STRUCT default_tft_screens;
extern TFTBuffer tft_screens;

//private data
uint16_t curr_bg_color = BLACK;
uint16_t curr_highlight_color = BLACK;
uint16_t curr_text_color = BLACK;
uint16_t curr_text_color_sp = BLACK;

#ifdef MODEL_ST7735
static uint8_t col_shift;
static uint8_t row_shift;
#endif

uint8_t tft_orientation = 0, tft_enabled = 1;
uint8_t char_max_x, char_max_y;

TFTBuffer_STRUCT default_tft_screens = {0};
TFTBuffer tft_screens = &default_tft_screens;

#define curr_screen    tft_screens->curr_screen
#define curr_text_buf  tft_screens->text_buf[curr_screen]
#define next_text_buf  tft_screens->text_buf[!curr_screen]
#define curr_text_color_buf tft_screens->text_color_buf[curr_screen]
#define next_text_color_buf tft_screens->text_color_buf[!curr_screen]
#define curr_bg_color_buf   tft_screens->bg_color_buf[curr_screen]
#define next_bg_color_buf   tft_screens->bg_color_buf[!curr_screen]

#define CHAR_PIXELS            CHAR_WIDTH*CHAR_HEIGHT
#define CHAR_BYTES             CHAR_PIXELS*PIXEL_BYTES
#if defined(MODEL_ILI9486)
	#define THREADED_TFT_BUF_LEN  (7)
#elif defined(MODEL_ILI9341)
	#define THREADED_TFT_BUF_LEN  (21)
#elif defined(MODEL_ST7735)
	#define THREADED_TFT_BUF_LEN  (28)
#endif
#define TFT_DMA_BUF_PIXELS (CHAR_PIXELS * THREADED_TFT_BUF_LEN)
#define TFT_DMA_BUF_SIZE (CHAR_BYTES * THREADED_TFT_BUF_LEN)
uint8_t buf_ptr = 0;
static uint8_t tft_dma_bufs[2][TFT_DMA_BUF_SIZE];
#define curr_tft_dma_buf tft_dma_bufs[buf_ptr]

/**
  * @brief  Clear the whole text buffer
  * @param  None
  * @retval None
  */
static inline void tft_clear(void){
	for(uint8_t y = 0; y < char_max_y; y++){
		for (uint8_t x = 0; x < char_max_x; x++) {
			curr_text_buf[TFT_XY(x,y)] = ' ';
			// curr_text_color_buf[TFT_XY(x,y)] = curr_text_color;
			// curr_bg_color_buf[TFT_XY(x,y)] = curr_bg_color;
		}
	}
}
static inline void tft_clear2(void){
	for(uint8_t y = 0; y < char_max_y; y++){
		for (uint8_t x = 0; x < char_max_x; x++) {
			curr_text_buf[TFT_XY(x,y)] = ' ';
			curr_text_color_buf[TFT_XY(x,y)] = curr_text_color;
			curr_bg_color_buf[TFT_XY(x,y)] = curr_bg_color;
		}
	}
}

static inline void write_command(uint8_t command) {
	while(TFT_SPI.State != HAL_SPI_STATE_READY);

	HAL_GPIO_WritePin(TFT_DC_GPIO_Port, TFT_DC_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&TFT_SPI, (uint8_t*)&command, 1, 1);
	HAL_GPIO_WritePin(TFT_DC_GPIO_Port, TFT_DC_Pin, GPIO_PIN_SET);
}
static inline void write_data(uint8_t* data, uint16_t size) {
	HAL_SPI_Transmit(&TFT_SPI, (uint8_t*)data, size, 1);
}
static inline void write_many(uint8_t command, uint8_t* data, uint16_t size) {
	write_command(command);
	write_data(data, size);
}
static inline void write_many_dma(uint8_t command, uint8_t* data, uint16_t size) {
	write_command(command);
	HAL_SPI_Transmit_DMA(&TFT_SPI, (uint8_t*)data, size);
}
static inline void write_byte(uint8_t command, uint8_t data) {
	write_command(command);
	write_data(&data, 1);
}
static inline void write_2hword(uint8_t command, uint16_t a, uint16_t b) {
	write_command(command);
	uint8_t buf[4] = {a>>8, a&0xFF, b>>8, b&0xFF};
	write_data(buf, 4);
}

/**
  * @brief  Initialization of TFT
  * @param  orientation: default orientation
  * @param  in_bg_color: default background color
  * @param  in_text_color: default text color
  * @param  in_text_color_sp: default special text color
  * @retval None
  */
void tft_init(TFT_ORIENTATION orientation, uint16_t in_bg_color, uint16_t in_text_color, uint16_t in_text_color_sp, uint16_t in_highlight_color) { //, DISPLAY_MODE mode) {
	curr_screen = 0;
	tft_orientation = orientation;

	char_max_x = (orientation % 2) ? CHAR_MAX_X_HORIZONTAL : CHAR_MAX_X_VERTICAL;
	char_max_y = (orientation % 2) ? CHAR_MAX_Y_HORIZONTAL : CHAR_MAX_Y_VERTICAL;

	TFT_SPI_Init();
	
	HAL_GPIO_WritePin(TFT_RST_GPIO_Port, TFT_RST_Pin, GPIO_PIN_SET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(TFT_RST_GPIO_Port, TFT_RST_Pin, GPIO_PIN_RESET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(TFT_RST_GPIO_Port, TFT_RST_Pin, GPIO_PIN_SET);
	HAL_Delay(100);

#if defined(TFT_CS_Pin) && !defined(MODEL_ST7735)
	HAL_GPIO_WritePin(TFT_CS_GPIO_Port, TFT_CS_Pin, GPIO_PIN_SET);
#endif

	uint32_t i=0;
	while (tft_init_code[++i] != TFT_EOF_MARKER) {
		if (tft_init_code[i] == TFT_ORIENTATION_CMD) {
			write_byte(tft_init_code[i], tft_orientation_init_code[tft_orientation]);
			i++;
		} else if (tft_init_code[i] == TFT_DELAY_MARKER) {
			i += 3;
			HAL_Delay(tft_init_code[i]);
		} else if (tft_init_code[i+1] == TFT_EOL_MARKER) {
			write_command(tft_init_code[i]);
			i++;
		} else {
			int len;
			for (len=0; tft_init_code[i+len+1] != TFT_EOL_MARKER; len++) {}
			write_many(tft_init_code[i], (uint8_t*)&tft_init_code[i+1], len);
			i += len+1;
		}
	}

#ifdef MODEL_ST7735
	switch (tft_orientation) {
		case 0:
			col_shift = 2;
			row_shift = 1;
		break;
		case 1:
			col_shift = 1;
			row_shift = 2;
		break;
		case 2:
			col_shift = 2;
			row_shift = 2;
		break;
		case 3:
			col_shift = 1;
			row_shift = 2;
		break;
	}
#endif
	
	tft_set_bg_color(in_bg_color);
	tft_set_text_color(in_text_color);
	tft_set_special_color(in_text_color_sp);
	tft_set_highlight_color(in_highlight_color);
	
	tft_force_clear();
}

/**
  * @brief  Set the current background color
  * @param  None
  * @retval None
  */
void tft_set_bg_color(uint16_t in_bg_color) {
	curr_bg_color = in_bg_color;
}

/**
  * @brief  Set the current special background color
  * @param  None
  * @retval None
  */
void tft_set_highlight_color(uint16_t highlight_color) {
	curr_highlight_color = highlight_color;
}

/**
  * @brief  Set the current text color
  * @param  None
  * @retval None
  */
void tft_set_text_color(uint16_t in_text_color) {
	curr_text_color = in_text_color;
}

/**
  * @brief  Set the current special text color
  * @param  None	
  * @retval None
  */
void tft_set_special_color(uint16_t text_color_sp) {
	curr_text_color_sp = text_color_sp;
}

static inline void tft_set_region(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
	uint16_t startx = x;
	uint16_t starty = y;
	uint16_t endx = x+w;
	uint16_t endy = y+h;

	#ifdef MODEL_ST7735
	startx += col_shift;
	starty += row_shift;
	endx += col_shift;
	endy += row_shift;
	#endif

	write_2hword(0x2a, startx, endx);		// Column addr set
	write_2hword(0x2b, starty, endy);		// Column addr set
	// write_command(0x2c);	
}

/**
  * @brief  Clear every pixel on screen
  * @param  None
  * @retval None
  */
void tft_force_clear(void) {
	tft_clear2();
	tft_update(0);
	tft_clear2();
	if (tft_orientation%2)
		tft_print_rectangle(curr_bg_color, 0, 0, MAX_HEIGHT, MAX_WIDTH);
	else
		tft_print_rectangle(curr_bg_color, 0, 0, MAX_WIDTH, MAX_HEIGHT);
}

void tft_printc(uint8_t x, uint8_t y, char * fp) {
	uint8_t i = x;
	uint8_t j = y;
	uint8_t is_special = 0;
	uint8_t is_highlighted = 0;
	uint8_t is_underlined = 0;
	// uint8_t escaped = 0;
	
	while (*fp && i < char_max_x && j < char_max_y) {
		switch(*fp) {
			case '\r':
			case '\n':
			j++;
			break;
			case '[':
			case ']':
			is_special = (*fp == '[');
			break;
			case '{':
			case '}':
			is_highlighted = (*fp == '{');
			break;
			case '|':
			is_underlined = !is_underlined;
			break;
			default:
			curr_text_buf[TFT_XY(i,j)] = ((*fp) | (is_underlined ? 0x80 : 0x00));
			curr_text_color_buf[TFT_XY(i,j)] = is_special     ? curr_text_color_sp   : curr_text_color;
			curr_bg_color_buf[TFT_XY(i,j)] = is_highlighted ? curr_highlight_color : curr_bg_color;
			i++;
			break;
		}
		fp++;
	}
}

/**
  * @brief  Print a string at certain position, use [] to indicate special words
  * @param  x: starting x-coordinate
  * @param  y: starting y-coordinate
  * @param  pstr: string to be printed
  */
void tft_prints(uint8_t x, uint8_t y, const char * fmt, ...) {
	char buf[CHAR_MAX_X]={0}, *fp=buf;
	
	va_list args;
	va_start( args, fmt );
	print( &fp, fmt, args );

	tft_printc(x, y, buf);
}

void tft_printi(uint8_t x, uint8_t y, int32_t num) {
	char buf[12]={0};

	itos(buf, num);

	tft_printc(x, y, buf);
}

void tft_printl(uint8_t x, uint8_t y, int64_t num) {
	char buf[12]={0};

	ltos(buf, num);

	tft_printc(x, y, buf);
}

void tft_printu(uint8_t x, uint8_t y, uint32_t num) {
	char buf[12]={0};

	utos(buf, num);

	tft_printc(x, y, buf);
}

void tft_printf(uint8_t x, uint8_t y, float num, uint32_t dp) {
	char buf[40];

	ftos(buf, num, dp);

	tft_printc(x, y, buf);
}

static __forceinline void tft_buffer_pixel(uint8_t* out, uint16_t rgb565) {
	#ifdef MODEL_ILI9486 //RGB666
		out[0] = (rgb565 & 0xF800) >> (11-3);
		out[1] = (rgb565 & 0x7E0) >> (5-2);
		out[2] = (rgb565 & 0x1F) << 3;
	#else
		out[0] = rgb565>>8;
		out[1] = rgb565;
	#endif
}

static __forceinline void tft_buffer_pixels(uint8_t* out, uint16_t rgb565, uint32_t len) {
	uint8_t buf[PIXEL_BYTES];
	tft_buffer_pixel(buf, rgb565);
	while(len--) {
		*out++ = buf[0];
		*out++ = buf[1];
	#if PIXEL_BYTES==3
		*out++ = buf[2];
	#endif
	}
}

void tft_print_rectangle(uint16_t color, uint32_t x, uint32_t y, uint32_t w, uint32_t h) {
	uint32_t i = w*h;
	if (i <= (64)) {
		
		uint8_t buf[PIXEL_BYTES];
		tft_buffer_pixel(buf, color);

		tft_set_region(x, y, w-1, h-1);
		write_command(0x2c);
		for (; i != 0; i--) {
			write_data(buf, PIXEL_BYTES);
		}
	} else {
		tft_buffer_pixels(curr_tft_dma_buf, color, (i > TFT_DMA_BUF_PIXELS) ? TFT_DMA_BUF_PIXELS : i);

		// tft_dma_wait();
		tft_set_region(x, y, w-1, h-1);
		write_command(0x2c);
		do {
			int32_t l = (i > TFT_DMA_BUF_PIXELS) ? TFT_DMA_BUF_PIXELS : i;
			while (HAL_SPI_GetState(&TFT_SPI) != HAL_SPI_STATE_READY);
			write_many_dma(0x2c, curr_tft_dma_buf, l*PIXEL_BYTES);
			i -= l;
		} while (i != 0);
		buf_ptr ^= 0x1;
	}
}
void tft_print_pixel(uint16_t color, uint32_t x, uint32_t y) {
	tft_set_region(x, y, 1-1, 1-1);
	
	uint8_t buf[PIXEL_BYTES];
	
	tft_buffer_pixel(buf, color);
	
	write_many(0x2c, buf, PIXEL_BYTES);
}

uint8_t tft_print_image2(void* buf, uint32_t x, uint32_t y, uint32_t w, uint32_t h) {
	if (HAL_SPI_GetState(&TFT_SPI) != HAL_SPI_STATE_READY) return 0;
	
	tft_set_region(x, y, w-1, h-1);
	
	write_many_dma(0x2c, buf, w*h*2);

	return 1;
}
void tft_print_image(void* buf, uint32_t x, uint32_t y, uint32_t w, uint32_t h) {
	while (tft_print_image2(buf, x, y, w, h) == 0);
}

uint8_t tft_update(uint32_t period) {
	static uint32_t last_update = 0;
	if ((HAL_GetTick() - last_update) < period) return 2;
	last_update = HAL_GetTick();

	for (uint8_t j = 0; j < char_max_y; j++) {
		for (uint8_t i = 0; i < char_max_x; i++) {
			if (tft_char_is_changed(tft_screens, i, j)) {
				uint8_t adj_cnt = 1;
				while ((i + adj_cnt < char_max_x) && tft_char_is_changed(tft_screens, i + (adj_cnt), j) && (adj_cnt < THREADED_TFT_BUF_LEN))
					adj_cnt++; //Search for diff char

				uint16_t px = 0;
				uint16_t y = 0;
				while (y < CHAR_HEIGHT) {
					uint8_t char_cnt = 0;
					while (char_cnt != adj_cnt) {
						const uint8_t char_ptr = (CHAR_PTR(curr_text_buf[TFT_XY(i + char_cnt, j)]))[y];
						uint16_t fg = curr_text_color_buf[TFT_XY(i + char_cnt, j)];
						uint16_t bg = curr_bg_color_buf[TFT_XY(i + char_cnt, j)];
						for (uint8_t x = CHAR_WIDTH; x > 0; x--) {
							uint16_t color = ((char_ptr >> (x - 1)) & 0x01) ? fg : bg;
							tft_buffer_pixel(&tft_dma_bufs[buf_ptr][px], color);
							px += PIXEL_BYTES;
						}
						char_cnt++;
					}
					y++;
				}

				tft_print_image(&tft_dma_bufs[buf_ptr][0], 
					i * CHAR_WIDTH, j * CHAR_HEIGHT, adj_cnt * CHAR_WIDTH, CHAR_HEIGHT);
				buf_ptr ^= 0x1;
				i += adj_cnt - 1;
			}
		}
	}

	// Swap pointers
	curr_screen = (curr_screen == 0);

	tft_clear();

	return 0;
}

uint8_t tft_update2(uint32_t period) {
	static enum TFT_UPDATE_STATE {
		TFT_IDLE, TFT_BUFFERING_PIXELS, TFT_WAITING_DMA, 
	} state = 0;
	static uint8_t chari=0, charj=0;
	static uint8_t adj_cnt;
	static uint32_t last_update = 0;
	switch (state) {
		case TFT_IDLE:
			if ((HAL_GetTick() - last_update) < period) {
				return 2;
			} else {
				state = TFT_BUFFERING_PIXELS;
				last_update += period;
			}
		//Fall-through
		case TFT_BUFFERING_PIXELS:
			for (; charj < char_max_y; charj++) {
				for (; chari < char_max_x; chari++) {
					if (tft_char_is_changed(tft_screens, chari, charj)) {
						adj_cnt = 1;
						while ((chari + adj_cnt < char_max_x) && tft_char_is_changed(tft_screens, chari + (adj_cnt), charj) && (adj_cnt < THREADED_TFT_BUF_LEN))
							adj_cnt++; //Search for diff char

						uint16_t px = 0;
						uint16_t y = 0;
						while (y < CHAR_HEIGHT) {
							uint8_t char_cnt = 0;
							while (char_cnt != adj_cnt) {
								const uint8_t char_ptr = (CHAR_PTR(curr_text_buf[TFT_XY(chari + char_cnt, charj)]))[y];
								uint16_t fg = curr_text_color_buf[TFT_XY(chari + char_cnt, charj)];
								uint16_t bg = curr_bg_color_buf[TFT_XY(chari + char_cnt, charj)];
								for (uint8_t x = CHAR_WIDTH; x > 0; x--) {
									uint16_t color = ((char_ptr >> (x - 1)) & 0x01) ? fg : bg;
									tft_buffer_pixel(&tft_dma_bufs[buf_ptr][px], color);
									px += PIXEL_BYTES;
								}
								char_cnt++;
							}
							y++;
						}

						state = TFT_WAITING_DMA;
						break;
					}
				}
				if (state == TFT_WAITING_DMA)
					break;
				else
					chari = 0;
			}
			if (state != TFT_WAITING_DMA) {
				chari = 0; charj = 0;
				curr_screen = (curr_screen == 0);
				tft_clear();
				state = TFT_IDLE;
				return 0;
			}
		//Fall-through
		case TFT_WAITING_DMA:
			if (tft_print_image2(&tft_dma_bufs[buf_ptr][0], chari * CHAR_WIDTH, charj * CHAR_HEIGHT, adj_cnt * CHAR_WIDTH, CHAR_HEIGHT)) {
				buf_ptr ^= 0x1;
				chari += adj_cnt;
				state = TFT_BUFFERING_PIXELS;
				return 1;
			} else return 1;
	}
	return 1;
}
