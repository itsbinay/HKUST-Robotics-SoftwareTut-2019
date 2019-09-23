#ifndef __CAMBUS_H
#define __CAMBUS_H

#include "main.h"

typedef enum {
	CAM_BUS_FAIL=0,
	CAM_BUS_OKAY,
} CAMBUSStatus;

CAMBUSStatus CAMBUS_Init(void);
uint32_t CAMBUS_Scan(void);
CAMBUSStatus CAMBUS_Write(uint8_t slave, uint16_t address, uint8_t data);
CAMBUSStatus CAMBUS_Read(uint8_t slave, uint8_t* buffer, uint16_t length, uint8_t address);

#endif
