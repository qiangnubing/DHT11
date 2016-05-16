#ifndef __BSP_DHT11_H__
#define __BSP_DHT11_H__

/*----添加需要包含的头文件----*/
//////////////////////////////////////////////////////////////////
#include "hal_delay.h"
#include "CoOS.h"
//////////////////////////////////////////////////////////////////

void BSP_DHT11_Init_Port(void);
bool BSP_DHT11_ReadData(u8* temp,u8* humi);


#endif
