#include "cambus.h"


//TODO: All of this Shit
// I2C or Bitbanging??

#if 0 // I2C Doesnt Work???
#include "i2c.h"
#define CB_TIMEOUT 1000
CAMBUSStatus CAMBUS_Init(void) {
	static uint32_t inited = 0;
	if (!inited) {
		CAMBUS_I2C_Init();

		inited = 1;
	}

	return CAM_BUS_OKAY;
}

uint32_t CAMBUS_Scan(void) {
	for (uint8_t addr=0x08; addr<=0x77; addr++) {
	    // __disable_irq();
	    if (HAL_I2C_IsDeviceReady(CAMBUS, addr << 1, 10, CB_TIMEOUT) == HAL_OK) {
	        //__enable_irq();
	        return (addr << 1);
	    }
	    //__enable_irq();
	}

	return 0;
}
CAMBUSStatus CAMBUS_Write(uint8_t slave, uint16_t address, uint8_t data) {
	CAMBUSStatus ret=CAM_BUS_OKAY;
    uint8_t buf[] = {address, data};

    // __disable_irq();
    if(HAL_I2C_Master_Transmit(CAMBUS, slave, buf, 2, CB_TIMEOUT) != HAL_OK) {
        ret = CAM_BUS_FAIL;
    }
    // __enable_irq();

    return ret;
}
CAMBUSStatus CAMBUS_Read(uint8_t slave, uint8_t* buffer, uint16_t length, uint8_t address) {
	CAMBUSStatus ret=CAM_BUS_OKAY;
	HAL_StatusTypeDef sts = HAL_OK;
#if 0
	sts = HAL_I2C_Mem_Read(CAMBUS, slave, address, I2C_MEMADD_SIZE_8BIT, buffer, length, CB_TIMEOUT);
	if (sts != HAL_OK) {
		ret = CAM_BUS_FAIL;
		tft_printi(0, 4, sts);
		tft_printi(0, 4, CAMBUS->ErrorCode);
	}
#else
	// __disable_irq();
	sts = HAL_I2C_Master_Transmit2(CAMBUS, slave, &address, 1, CB_TIMEOUT);
	if(sts != HAL_OK) {
		ret = CAM_BUS_FAIL;
		tft_printi(0, 4, CAMBUS->ErrorCode);
		return ret;
	}
	sts = HAL_I2C_Master_Receive(CAMBUS, slave, buffer, length, CB_TIMEOUT);
    if(sts != HAL_OK) {
        ret = CAM_BUS_FAIL;
		tft_printi(0, 5, CAMBUS->ErrorCode);
    }
	// __enable_irq();
#endif

	return ret;
}
#else
#include "tim.h"
#include "gpio.h"

#define SCCB_TIM TIM7
#define SCCB_PSC 6000
#define SCL_H         (SCCB_SCL_GPIO_Port->BSRR = SCCB_SCL_Pin)
#define SCL_L         (SCCB_SCL_GPIO_Port->BSRR = SCCB_SCL_Pin<<16)
#define SCL_read      (SCCB_SCL_GPIO_Port->IDR  & SCCB_SCL_Pin)
#define SDA_H         (SCCB_SDA_GPIO_Port->BSRR = SCCB_SDA_Pin)
#define SDA_L         (SCCB_SDA_GPIO_Port->BSRR = SCCB_SDA_Pin<<16)
#define SDA_read      (SCCB_SDA_GPIO_Port->IDR  & SCCB_SDA_Pin)

CAMBUSStatus CAMBUS_Init(void) {
	static uint32_t inited = 0;
	if (!inited) {
		TIM7_Init();
		SCCB_TIM->PSC = 0;
		SCCB_TIM->ARR = SCCB_PSC;
		SCCB_TIM->CNT = 0; 
		HAL_GPIO_InitPin(SCCB_SCL_GPIO_Port, SCCB_SCL_Pin|SCCB_SDA_Pin, GPIO_MODE_OUTPUT_OD, GPIO_PULLUP);

		inited = 1;
	}

	return CAM_BUS_OKAY;
}

static __forceinline void sccbDelay(void)
{	
	SCCB_TIM->CR1 |= TIM_CR1_CEN;
	SCCB_TIM->SR &= ~TIM_SR_UIF;
	SCCB_TIM->CNT = 0;
	while (!(SCCB_TIM->SR & TIM_SR_UIF));
	SCCB_TIM->CR1 &= ~TIM_CR1_CEN;
}

static __forceinline uint8_t sccbStart(void)
{
	SDA_H;
	SCL_H;
	sccbDelay();
	if(!SDA_read)
	return CAM_BUS_FAIL;
	SDA_L;
	sccbDelay();
	if(SDA_read) 
	return CAM_BUS_FAIL;
	SDA_L;
	sccbDelay();
	return CAM_BUS_OKAY;
}

static __forceinline void sccbStop(void)
{
	SCL_L;
	sccbDelay();
	SDA_L;
	sccbDelay();
	SCL_H;
	sccbDelay();
	SDA_H;
	sccbDelay();
}

static __forceinline void sccbAck(void)
{	
	SCL_L;
	sccbDelay();
	SDA_L;
	sccbDelay();
	SCL_H;
	sccbDelay();
	SCL_L;
	sccbDelay();
}

static __forceinline void sccbNoAck(void)
{	
	SCL_L;
	sccbDelay();
	SDA_H;
	sccbDelay();
	SCL_H;
	sccbDelay();
	SCL_L;
	sccbDelay();
}

static __forceinline uint8_t sccbWaitAck(void) 	
{
	SCL_L;
	sccbDelay();
	SDA_H;			
	sccbDelay();
	SCL_H;
	sccbDelay();
	if(SDA_read)
	{
      SCL_L;
      return CAM_BUS_FAIL;
	}
	SCL_L;
	return CAM_BUS_OKAY;
}

static __forceinline void sccbSendByte(uint8_t SendByte) 
{
	uint8_t i = 8;
	while(i--)
	{
		SCL_L;
		sccbDelay();
		if(SendByte & 0x80)
			SDA_H;  
		else 
			SDA_L;   
		SendByte <<= 1;
		sccbDelay();
		SCL_H;
		sccbDelay();
	}
	SCL_L;
}

static __forceinline uint8_t sccbReceiveByte(void)  
{ 
    uint8_t i = 8;
    uint8_t receiveByte = 0;
    SDA_H;				
    while(i--)
    {
      receiveByte <<= 1;      
      SCL_L;
      sccbDelay();
			SCL_H;
      sccbDelay();	
      if(SDA_read)
        receiveByte |= 0x01;
    }
    SCL_L;
    return receiveByte;
}

CAMBUSStatus CAMBUS_Write(uint8_t slave, uint16_t address, uint8_t data)
{
	if(sccbStart() != CAM_BUS_OKAY)
		return CAM_BUS_FAIL;
	sccbSendByte(slave);
	if(sccbWaitAck() != CAM_BUS_OKAY)
	{
		sccbStop(); 
		return CAM_BUS_FAIL;
	}
	sccbSendByte((uint8_t)(address & 0x00FF));     
	sccbWaitAck();	
	sccbSendByte(data);
	sccbWaitAck();   
	sccbStop(); 
	return CAM_BUS_OKAY;
}

CAMBUSStatus CAMBUS_Read(uint8_t slave, uint8_t* buffer, uint16_t length, uint8_t address)
{
	if(sccbStart() != CAM_BUS_OKAY)
		return CAM_BUS_FAIL;
	sccbSendByte(slave);
	if(sccbWaitAck() != CAM_BUS_OKAY)
	{
		sccbStop(); 
		return CAM_BUS_FAIL;
	}
	sccbSendByte(address);
	sccbWaitAck();
	sccbStop();
	if(sccbStart() != CAM_BUS_OKAY)
		return CAM_BUS_FAIL;
	sccbSendByte( slave + 1 );
	if(sccbWaitAck() != CAM_BUS_OKAY)
	{
		sccbStop(); 
		return CAM_BUS_FAIL;
	}
	while(length)
	{
		*buffer = sccbReceiveByte();
		if(length == 1)
			sccbNoAck();
		else
			sccbAck(); 
		buffer++;
		length--;
	}
	sccbStop();
	return CAM_BUS_OKAY;
}


#endif
