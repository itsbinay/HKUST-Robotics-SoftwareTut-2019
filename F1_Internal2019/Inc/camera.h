#ifndef __CAMERA_H
#define __CAMERA_H

#include "main.h"

typedef enum __CameraColorMode {
	CAM_GRAYSCALE=0,
	CAM_RGB565,

	CameraColorMode_Num,
} CameraColorMode;

typedef enum __CameraFrameSize {
	QVGA_320x240=0,
	QQVGA_160x120=1,
	QQQVGA_80x60=2,
	QQQQVGA_40x30=3,

	CameraFrameSize_Num,
} CameraFrameSize;

typedef enum __CameraFrameRate {
	CAM_150FPS=0,
	CAM_112FPS,
	CAM_75FPS,
	CAM_50FPS,
	CAM_37FPS,
	CAM_30FPS,
	CAM_25FPS,
	CAM_15FPS,

	CameraFrameRate_Num,
} CameraFrameRate;

typedef enum __CameraStatus {
	CAM_NOT_INITIALIZED=0,
	CAM_INIT_ERROR0,
	CAM_INIT_ERROR1,
	CAM_INIT_ERROR2,
	CAM_INIT_ERROR3,
	CAM_INIT_ERROR4,
	CAM_NOT_FOUND,
	CAM_CONFIGURED,
	CAM_CAPTURING,
	CAM_NEW_FRAME_READY,
	CAM_MISSED_A_FRAME,
} CameraStatus;

static const char * CameraStatusStrings[] = {
	"CAM_NOT_INITIALIZED",
	"CAM_INIT_ERROR0",
	"CAM_INIT_ERROR1",
	"CAM_INIT_ERROR2",
	"CAM_INIT_ERROR3",
	"CAM_INIT_ERROR4",
	"CAM_NOT_FOUND",
	"CAM_CONFIGURED",
	"CAM_CAPTURING",
	"CAM_NEW_FRAME_READY",
	"CAM_MISSED_A_FRAME",
};

CameraStatus CAM_Init(void);
void CAM_SetColorMode(CameraColorMode c);
void CAM_SetFrameSize(CameraFrameSize f);
void CAM_SetFrameRate(CameraFrameRate r);
void CAM_SetZoom(float ratio);
void CAM_SetBrightness(uint8_t brightness);
void CAM_SetContrast(uint8_t contrast);
void CAM_AutoDenoise(uint8_t en);
void CAM_SetDenoiseTreshold2(uint8_t thresh);
void CAM_SetDenoiseTreshold(uint8_t thresh);
void CAM_SetSharpness(uint8_t strength, uint8_t thresh, uint8_t upperlim, uint8_t lowerlim);
void CAM_AutoSharpness(uint8_t en);
CameraColorMode CAM_GetColorMode(void);
CameraFrameSize CAM_GetFrameSize(void);

uint32_t CAM_FrameWidth(void);
uint32_t CAM_FrameHeight(void);

CameraStatus CAM_GetStatus(void);
uint8_t CAM_FrameReady(void);
uint8_t CAM_Initialized(void);

void CAM_GetGrayscale(uint8_t* ptr);
void CAM_GetRGB565(uint16_t* ptr);
void CAM_GreyToRGB565(uint8_t* img, uint16_t* i2);

#endif
