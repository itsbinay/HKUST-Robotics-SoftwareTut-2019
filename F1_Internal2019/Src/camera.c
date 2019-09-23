#include "camera.h"
#include "cambus.h"
#include "ov7725.h"

// #define STORE_RGB565_BIGENDIAN

#define C_PIN(N) C_##N##_GPIO_Port, C_##N##_Pin

static volatile CameraStatus cam_state = CAM_NOT_INITIALIZED;
static volatile CameraColorMode cam_color = CAM_GRAYSCALE;
static volatile CameraFrameSize cam_size = QQQVGA_80x60;
static volatile CameraFrameRate cam_rate = CAM_75FPS;
#define FrameWidth cam_sizes[cam_size].width
#define FrameHeight cam_sizes[cam_size].height

static const struct {
	uint32_t width, height;
} cam_sizes[CameraFrameSize_Num] = {
	{320, 240},
	{160, 120},
	{ 80,  60},
	{ 40,  30},
};

static const struct {
	uint8_t clkrc, pll;
} cam_rates[CameraFrameRate_Num] = {
	{CLKRC_RESERVED(CLKRC_PRESCALER(0)), COM4_RESERVED(COM4_PLL(3))},
	{CLKRC_RESERVED(CLKRC_PRESCALER(0)), COM4_RESERVED(COM4_PLL(2))},
	{CLKRC_RESERVED(CLKRC_PRESCALER(0)), COM4_RESERVED(COM4_PLL(1))},
	{CLKRC_RESERVED(CLKRC_PRESCALER(2)), COM4_RESERVED(COM4_PLL(3))},
	{CLKRC_RESERVED(CLKRC_PRESCALER(1)), COM4_RESERVED(COM4_PLL(1))},
	{CLKRC_RESERVED(CLKRC_PRESCALER(4)), COM4_RESERVED(COM4_PLL(3))},
	{CLKRC_RESERVED(CLKRC_PRESCALER(2)), COM4_RESERVED(COM4_PLL(1))},
	{CLKRC_RESERVED(CLKRC_PRESCALER(4)), COM4_RESERVED(COM4_PLL(1))},
};

static const uint8_t cam_colors[CameraColorMode_Num] = {
	COM7_RESERVED(COM7_SELECT_QVGA | COM7_OUTPUT_YUV | COM7_BT656_ENABLE),
	COM7_RESERVED(COM7_SELECT_QVGA | COM7_OUTPUT_RGB | COM7_FORMAT_RGB565),
};

CameraStatus CAM_Config(void) {
	uint8_t ov7725ID = 0;
	CAMBUS_Init();

	// if(CAMBUS_Scan() != OV7725ADR) \
		return CAM_NOT_FOUND;
	// HAL_Delay(10);
	if(CAMBUS_Write(OV7725ADR, COM7,COM7_REGISTER_RESET) != CAM_BUS_OKAY) \
		return CAM_INIT_ERROR0;
	if(CAMBUS_Read(OV7725ADR, &ov7725ID,1,PID) != CAM_BUS_OKAY)	\
		return CAM_INIT_ERROR1;
	if(ov7725ID != EXPECTED_PID) \
		return CAM_INIT_ERROR2;
	if(CAMBUS_Read(OV7725ADR, &ov7725ID,1,VER) != CAM_BUS_OKAY)	\
		return CAM_INIT_ERROR3;
	if(ov7725ID != EXPECTED_VER) \
		return CAM_INIT_ERROR4;

	CAMBUS_Write(OV7725ADR, COM2,                        COM2_ODRIVE(3));
	CAMBUS_Write(OV7725ADR, CLKRC,                       0x00);
	CAM_SetColorMode(cam_color);
	CAMBUS_Write(OV7725ADR, HSTART,                      0x3f);
	CAMBUS_Write(OV7725ADR, HSIZE,                       0x50);
	CAMBUS_Write(OV7725ADR, VSTRT,                       0x03);
	CAMBUS_Write(OV7725ADR, VSIZE,                       0x78);
	CAMBUS_Write(OV7725ADR, HREF,                        0x00);
	CAMBUS_Write(OV7725ADR, TGT_B,                       0x7F);
	CAMBUS_Write(OV7725ADR, TGT_R,                       0x7F);
	CAMBUS_Write(OV7725ADR, COM13,                       COM13_DEFAULT);
	CAMBUS_Write(OV7725ADR, FixGain,                     0x00);
	CAMBUS_Write(OV7725ADR, AWB_Ctrl0,                   0xe0);
	CAMBUS_Write(OV7725ADR, DSP_Ctrl1,                   DSP_Ctrl1_FIFO_ENABLE | DSP_Ctrl1_UVADJ_ENABLE
												 | DSP_Ctrl1_SDE_ENABLE | DSP_Ctrl1_COLOR_MATRIX_ENABLE
												 | DSP_Ctrl1_INTERPOLATION_ENABLE | DSP_Ctrl1_GAMMA_ENABLE
												 | DSP_Ctrl1_BLACK_DEFECT_CORRECTION | DSP_Ctrl1_WHITE_DEFECT_CORRECTION);
	CAMBUS_Write(OV7725ADR, DSP_Ctrl2,                   DSP_Ctrl2_VERT_DOWNSAMPLE_EN | DSP_Ctrl2_HORZ_DOWNSAMPLE_EN
												 | DSP_Ctrl2_VERT_ZOOM_EN | DSP_Ctrl2_HORZ_ZOOM_EN);
	CAMBUS_Write(OV7725ADR, DSP_Ctrl3,                   0x00);
	CAMBUS_Write(OV7725ADR, DSP_Ctrl4,                   0x00);
	CAMBUS_Write(OV7725ADR, COM8,                        COM8_ENABLE_FAST_AGCAEC | COM8_AEC_UNLIMITED_STEP_SIZE 
												 | COM8_AEC_STEP_SIZE_LIM_VBLANK | COM8_ENABLE_BANDING_FILTER);
	//TODO: Experiment with AGC, AEC, AWB
	// CAMBUS_Write(OV7725ADR, COM8,		                  COM8_ENABLE_FAST_AGCAEC | COM8_AEC_UNLIMITED_STEP_SIZE
	// 	                                         | COM8_AEC_STEP_SIZE_LIM_VBLANK | COM8_ENABLE_BANDING_FILTER
	// 	                                         | COM8_ENABLE_FINE_AEC | COM8_ENABLE_AGC
	// 	                                         | COM8_ENABLE_AWB | COM8_ENABLE_AEC);
	CAMBUS_Write(OV7725ADR, COM4,                        COM4_RESERVED(COM4_PLL(1)));

	CAMBUS_Write(OV7725ADR, COM6,		          /*0xc5*/COM6_RESERVED(COM6_ENABLE_AUTOWINDOW_FORMAT_CHANGE));
	CAMBUS_Write(OV7725ADR, COM9,		          /*0x21*/COM9_RESERVED(COM9_MAX_AGC(2)));
	CAMBUS_Write(OV7725ADR, BDBase,	                  0xFF);
	CAMBUS_Write(OV7725ADR, BDMStep,	                  0x01);
	CAMBUS_Write(OV7725ADR, AEW,		                  0x34);
	CAMBUS_Write(OV7725ADR, AEB,		                  0x3c);
	CAMBUS_Write(OV7725ADR, VPT,		                  0xa1);
	CAMBUS_Write(OV7725ADR, EXHCL,		                  0x00);
	CAMBUS_Write(OV7725ADR, AWBCtrl3,                    0xaa);
	CAMBUS_Write(OV7725ADR, AWBCtrl1,                    0x5d);

	CAMBUS_Write(OV7725ADR, MTX1,		                  0x5f);
	CAMBUS_Write(OV7725ADR, MTX2,		                  0x53);
	CAMBUS_Write(OV7725ADR, MTX3,		                  0x11);
	CAMBUS_Write(OV7725ADR, MTX4,		                  0x1a);
	CAMBUS_Write(OV7725ADR, MTX5,		                  0x3d);
	CAMBUS_Write(OV7725ADR, MTX6,		                  0x5a);
	CAMBUS_Write(OV7725ADR, MTX_Ctrl,                    0x1e);
	CAMBUS_Write(OV7725ADR, MTX1,                        0xB0);
	CAMBUS_Write(OV7725ADR, MTX2,                        0x9D);
	CAMBUS_Write(OV7725ADR, MTX3,                        0x13);
	CAMBUS_Write(OV7725ADR, MTX4,                        0x16);
	CAMBUS_Write(OV7725ADR, MTX5,                        0x7B);
	CAMBUS_Write(OV7725ADR, MTX6,                        0x91);
	CAMBUS_Write(OV7725ADR, MTX_Ctrl,                    0x1E);
	CAMBUS_Write(OV7725ADR, USAT,		                  0x65);
	CAMBUS_Write(OV7725ADR, VSAT,		                  0x65);
	CAMBUS_Write(OV7725ADR, UVADJ0,	                  0x11);
	CAMBUS_Write(OV7725ADR, UVADJ1,	                  0x02);
	
	CAMBUS_Write(OV7725ADR, GAM1,                        0x0C);
	CAMBUS_Write(OV7725ADR, GAM2,                        0x16);
	CAMBUS_Write(OV7725ADR, GAM3,                        0x2A);
	CAMBUS_Write(OV7725ADR, GAM4,                        0x4E);
	CAMBUS_Write(OV7725ADR, GAM5,                        0x61);
	CAMBUS_Write(OV7725ADR, GAM6,                        0x6F);
	CAMBUS_Write(OV7725ADR, GAM7,                        0x7B);
	CAMBUS_Write(OV7725ADR, GAM8,                        0x86);
	CAMBUS_Write(OV7725ADR, GAM9,                        0x8E);
	CAMBUS_Write(OV7725ADR, GAM10,                       0x97);
	CAMBUS_Write(OV7725ADR, GAM11,                       0xA4);
	CAMBUS_Write(OV7725ADR, GAM12,                       0xAF);
	CAMBUS_Write(OV7725ADR, GAM13,                       0xC5);
	CAMBUS_Write(OV7725ADR, GAM14,                       0xD7);
	CAMBUS_Write(OV7725ADR, GAM15,                       0xE8);
	CAMBUS_Write(OV7725ADR, SLOP,                        0x20);
	CAMBUS_Write(OV7725ADR, HUECOS,                      0x80);
	CAMBUS_Write(OV7725ADR, HUESIN,                      0x80);
	CAMBUS_Write(OV7725ADR, DM_LNL,	                  0x00);
	CAMBUS_Write(OV7725ADR, BDBase,	                  0x99);
	CAMBUS_Write(OV7725ADR, BDMStep,	                  0x03);
	CAMBUS_Write(OV7725ADR, LC_YC,			              0x00);
	CAMBUS_Write(OV7725ADR, LC_RADI,	                  0x00);
	CAMBUS_Write(OV7725ADR, LC_COEF,	                  0x13);
	CAMBUS_Write(OV7725ADR, LC_XC,		                  0x08);
	CAMBUS_Write(OV7725ADR, LC_COEFB,                    0x14);
	CAMBUS_Write(OV7725ADR, LC_COEFR,                    0x17);
	CAMBUS_Write(OV7725ADR, LC_CTR,	                  0x00);

	CAMBUS_Write(OV7725ADR, COM3,                        COM3_VFLIP|COM3_HMIRROR
		                                         |COM3_SWAP_BR|COM3_SWAP_YUV
		                                         /*|COM3_SWAP_ENDIAN*/);
	CAMBUS_Write(OV7725ADR, COM5,                        0x00);
	CAMBUS_Write(OV7725ADR, SIGN,			              0x06);
	CAMBUS_Write(OV7725ADR, REG16,			              0x00);
	CAMBUS_Write(OV7725ADR, COM10,			              0x00/*COM10_ONLY_8MSBS*/);

	CAMBUS_Write(OV7725ADR, SDE,		                  SDE_FXEN_CNSTBRGHT|SDE_FXEN_SATURATION);

	CAMBUS_Write(OV7725ADR, DSPAuto,                     DSPAuto_RESERVED(DSPAuto_AWB_AUTOTHRESH | DSPAuto_DENOISE_AUTOTHRESH
												 | DSPAuto_SHARP_AUTOSTRENGTH | DSPAuto_UV_AUTOSLOPE));
	CAM_SetSharpness(10, 10, 1/*31*/, 1);
	CAM_SetBrightness(0x00);
	CAM_SetContrast(0x25/*0x40*/);
	CAM_SetDenoiseTreshold2(1/*16*/);
	CAM_SetDenoiseTreshold(0);
	CAM_SetFrameSize(cam_size);
	CAM_SetZoom(1.f);

	return CAM_CONFIGURED;
}

CameraStatus CAM_Init(void) {
	if (CAM_Initialized()) return cam_state;

	cam_state = CAM_Config();
	if (cam_state != CAM_CONFIGURED) return cam_state;

	HAL_GPIO_WritePin(C_PIN(OE), 0);
	HAL_GPIO_WritePin(C_PIN(WEN), 1);
	
	cam_state = CAM_CAPTURING;

	return cam_state;
}

void CAM_SetColorMode(CameraColorMode c) {
	if (CAMBUS_Write(OV7725ADR, COM7,cam_colors[c])) cam_color = c;
}

void CAM_SetFrameSize(CameraFrameSize f) {
	if (CAMBUS_Write(OV7725ADR, HOutSize, cam_sizes[f].width>>2)                                != CAM_BUS_OKAY)
		return;
	if (CAMBUS_Write(OV7725ADR, VOutSize, cam_sizes[f].height>>1)                               != CAM_BUS_OKAY)
		return;
	if (CAMBUS_Write(OV7725ADR, EXHCH, (cam_sizes[f].height&0x01<<2)|(cam_sizes[f].width&0x03)) != CAM_BUS_OKAY)
		return;
	if (CAMBUS_Write(OV7725ADR, SCAL0, SCAL0_DOWNSAMPLE_2_POW_N(f))                             != CAM_BUS_OKAY)
		return;
	cam_size = f;
}

void CAM_SetFrameRate(CameraFrameRate r) {
	if (CAMBUS_Write(OV7725ADR, COM4, cam_rates[r].pll)    != CAM_BUS_OKAY)
		return;
	if (CAMBUS_Write(OV7725ADR, CLKRC, cam_rates[r].clkrc) != CAM_BUS_OKAY)
		return;
	
	cam_rate = r;
}

void CAM_SetZoom(float ratio) {
	CAMBUS_Write(OV7725ADR, SCAL1, ratio*SCAL1_ZOOM_RATIO_1_1);
	CAMBUS_Write(OV7725ADR, SCAL2, ratio*SCAL2_ZOOM_RATIO_1_1);
}

void CAM_SetBrightness(uint8_t brightness) {
	CAMBUS_Write(OV7725ADR, BRIGHT, brightness);
}
void CAM_SetContrast(uint8_t contrast) {
	CAMBUS_Write(OV7725ADR, CNST, contrast);
}
void CAM_AutoDenoise(uint8_t en) {
	uint8_t tmp = 0;
	if (CAMBUS_Read(OV7725ADR, &tmp, 1, DSPAuto) != CAM_BUS_OKAY) return;
	if (en) CAMBUS_Write(OV7725ADR, DSPAuto, tmp |  DSPAuto_DENOISE_AUTOTHRESH);
	else    CAMBUS_Write(OV7725ADR, DSPAuto, tmp &~ DSPAuto_DENOISE_AUTOTHRESH);
}
void CAM_SetDenoiseTreshold2(uint8_t thresh) {
	CAMBUS_Write(OV7725ADR, DNSOff, thresh);
}
void CAM_SetDenoiseTreshold(uint8_t thresh) {
	CAMBUS_Write(OV7725ADR, DNSTh, thresh);
}
void CAM_SetSharpness(uint8_t strength, uint8_t thresh, uint8_t upperlim, uint8_t lowerlim) {
	CAMBUS_Write(OV7725ADR, EDGE0, EDGE0_STRENGTH(strength));
	CAMBUS_Write(OV7725ADR, EDGE1, EDGE1_THRESHOLD(thresh));
	CAMBUS_Write(OV7725ADR, EDGE2, upperlim);
	CAMBUS_Write(OV7725ADR, EDGE3, lowerlim);
}
void CAM_AutoSharpness(uint8_t en) {
	uint8_t tmp = 0;
	if (CAMBUS_Read(OV7725ADR, &tmp, 1, DSPAuto) != CAM_BUS_OKAY) return;
	if (en) CAMBUS_Write(OV7725ADR, DSPAuto, tmp |  DSPAuto_SHARP_AUTOSTRENGTH);
	else    CAMBUS_Write(OV7725ADR, DSPAuto, tmp &~ DSPAuto_SHARP_AUTOSTRENGTH);
}

uint32_t CAM_FrameWidth(void) {
	return FrameWidth;
}
uint32_t CAM_FrameHeight(void) {
	return FrameHeight;
}

__forceinline static uint8_t grey_pixel(volatile uint8_t* idr, volatile uint32_t* pclkr, uint32_t pclkh, uint32_t pclkl) {
	//uint16_t t = *idr;
	uint8_t t = *idr;
	*pclkr = pclkh; \
	*pclkr = pclkl; \
	*pclkr = pclkh; \
	*pclkr = pclkl;
	return t;
}
__forceinline static uint16_t rgb_pixel(volatile uint8_t* idr, volatile uint32_t* pclkr, uint32_t pclkh, uint32_t pclkl) {
#if 0
	uint16_t t = *idr << 8;
	*pclkr = pclkh; \
	*pclkr = pclkl;
	__NOP(); //Why the fuck do I need this!?!?
	__NOP(); //Figured out and explained a few lines below
	t |= *idr;
	*pclkr = pclkh; \
	*pclkr = pclkl;
#else
	uint16_t t = *idr;
	*pclkr = pclkh;
	*pclkr = pclkl;
	t<<=8;
	*pclkr = pclkh; //Order is weird?
	t |= *idr;
	*pclkr = pclkl;
	//The data will be updated on D[7:0] after FIFO_PCLK goes low
	//FIFO_PCLK goes low 2 cycles after the STRH instruction (1 cycle for memory write, 1 cycle for GPIO)
	//If I dont have 2 CPU cycles between the falling edge and the read then I would read the exact same data twice
	//This is why I don't shift immediately after reading, 
	//and why I do the rising edge of FIFO_PCLK "before" reading the data (latency makes it happen after)
	//Its filling space that would otherwise need to be NOPs 
	//If you see any glitches then its not actually as okay as I thought,
	//Move the problematic line down one and add a __NOP() in its place
#endif
	return t;
}
static void fifo_read_start(void) {
	HAL_GPIO_WritePin(C_PIN(RRST), 0); //FIFO_RRST_L();
	HAL_GPIO_WritePin(C_PIN(PCLK), 0); //FIFO_PCLK_L();
	HAL_GPIO_WritePin(C_PIN(PCLK), 1); //FIFO_PCLK_H();
	HAL_GPIO_WritePin(C_PIN(RRST), 1); //FIFO_RRST_H();
	HAL_GPIO_WritePin(C_PIN(PCLK), 0); //FIFO_PCLK_L();
	HAL_GPIO_WritePin(C_PIN(PCLK), 1); //FIFO_PCLK_H();
	HAL_GPIO_WritePin(C_PIN(PCLK), 0); //FIFO_PCLK_L();

}
void CAM_GetGrayscale(uint8_t* ptr) {
	fifo_read_start();
	
	volatile uint8_t* idr = (volatile uint8_t*) &C_D0_GPIO_Port->IDR;
	uint8_t* end = ptr + (FrameHeight*FrameWidth);
	
	volatile uint32_t* pclkr = (volatile uint32_t*) &C_PCLK_GPIO_Port->BSRR;
	uint32_t pclkh = C_PCLK_Pin;
	uint32_t pclkl = C_PCLK_Pin<<16;
	
	do {
		*ptr++ = grey_pixel(idr, pclkr, pclkh, pclkl);\
		*ptr++ = grey_pixel(idr, pclkr, pclkh, pclkl);\
		*ptr++ = grey_pixel(idr, pclkr, pclkh, pclkl);\
		*ptr++ = grey_pixel(idr, pclkr, pclkh, pclkl);\
	} while(ptr != end);
	
	cam_state = CAM_CAPTURING;
}
void CAM_GetRGB565(uint16_t* ptr) {
	fifo_read_start();

	volatile uint8_t* idr = (volatile uint8_t*) &C_D0_GPIO_Port->IDR;
	uint16_t* end = ptr + (FrameHeight*FrameWidth);
	
	volatile uint32_t* pclkr = (volatile uint32_t*) C_PCLK_GPIO_Port->BSRR;
	uint32_t pclkh = C_PCLK_Pin;
	uint32_t pclkl = C_PCLK_Pin<<16;
	
	do {
		*ptr++ = rgb_pixel(idr, pclkr, pclkh, pclkl);\
		*ptr++ = rgb_pixel(idr, pclkr, pclkh, pclkl);\
		*ptr++ = rgb_pixel(idr, pclkr, pclkh, pclkl);\
		*ptr++ = rgb_pixel(idr, pclkr, pclkh, pclkl);\
	} while(ptr != end);
	
	cam_state = CAM_CAPTURING;
}

uint8_t CAM_FrameReady(void) {
	return cam_state >= CAM_NEW_FRAME_READY;
}

uint8_t CAM_Initialized(void) {
	return cam_state >= CAM_CONFIGURED;
}

CameraStatus CAM_GetStatus(void) {
	return cam_state;
}

void VSYNC_Interrupt(void) {
	//TODO: I'm still not sure about WEN here
	if(cam_state == CAM_CAPTURING)
	{
		HAL_GPIO_WritePin(C_PIN(WRST), 0);
		HAL_GPIO_WritePin(C_PIN(WEN), 0);
		cam_state = CAM_NEW_FRAME_READY;
		HAL_GPIO_WritePin(C_PIN(WEN), 1);
		HAL_GPIO_WritePin(C_PIN(WRST), 1);
	}
	else if(cam_state == CAM_NEW_FRAME_READY)
	{
		HAL_GPIO_WritePin(C_PIN(WEN), 0);
		cam_state = CAM_MISSED_A_FRAME;
	}
}

// WARNING: 
// Danger lurks ahead
// What follows is the black magic assembly and bithack incantations of Anshuman Medhi
// Proceed with caution, lest you lose your sanity
static __forceinline uint32_t __SWAP16(uint32_t in) {
#if defined(__GNUC__)
	return __REV16(in);
#elif defined(__CC_ARM)
	__ASM {REV16 in, in};
	return in;
#endif
}
static __forceinline uint32_t __UXTB88(uint32_t in) {
	return (in & 0xFF) | ((in<<8)&0xFF0000);
}
static __forceinline uint64_t LOAD64(const void* ptr, const uint32_t off) {
	return *(((uint64_t*)ptr) + off);
}
__forceinline uint16_t grey_to_rgb565(uint32_t input) {
	return ((input << 8) & 0xF800) | ((input << 3) & 0x07E0) | ((input >> 3) & 0x001F);
}
__forceinline uint32_t grey_to_rgb565_2(uint32_t input) {
	return ((input << 8) & 0xF800F800) | ((input << 3) & 0x07E007E0) | ((input >> 3) & 0x001F001F);
}
__forceinline uint16_t grey_to_rgb565_r(uint8_t input) {
	// uint16_t temp = ((input << 8) & 0xF800) | ((input << 3) & 0x07E0) | ((input >> 3) & 0x001F);
	return __SWAP16(grey_to_rgb565(input));
}
__forceinline uint32_t grey_to_rgb565_r2(uint16_t in) {
	return __SWAP16(grey_to_rgb565_2(__UXTB88(in)));
}
void CAM_GreyToRGB565(uint8_t* img, uint16_t* i2) {
	uint8_t* imend = img + FrameHeight*FrameWidth;
	while (img != imend) {
		uint64_t t = LOAD64(img, 0); img += 8;
		uint32_t a = t;
		uint32_t b = t>>32;
		*((uint32_t*)i2) = grey_to_rgb565_r2(a);
		*((uint32_t*)i2+1) = grey_to_rgb565_r2(a>>16);
		*((uint32_t*)i2+2) = grey_to_rgb565_r2(b);
		*((uint32_t*)i2+3) = grey_to_rgb565_r2(b>>16);
		i2 += 8;
		//(5+40+2)/8pixel
	}
}

