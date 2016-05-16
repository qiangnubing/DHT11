#include "bsp_dht11.h"

/*
需要移植的代码：
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//DHT11单总线引脚高低电平控制
#define BSP_DHT11_DATA_H		GPIO_SetBits(GPIOA, GPIO_Pin_11)
#define BSP_DHT11_DATA_L		GPIO_ResetBits(GPIOA, GPIO_Pin_11)

//DHT11单总线引脚读取
#define BSP_DHT11_DATA_R    (GPIOA->IDR & GPIO_Pin_11)

//DHT11延时函数
#define BSP_DHT11_DelayMs_Port(x)	CoTickDelay(x)
#define BSP_DHT11_DelayUs_Port(x)	delay_us(x)	//us级延时函数(尽量保持延时的准确性)

/*
 ************************************************************
 *  名称：	BSP_DHT11_Init_Port
 *  功能：	DHT11 IO初始化
 *	输入：	无
 *	输出：  无
 ************************************************************
*/
void BSP_DHT11_Init_Port(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA ,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;	//开漏输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	BSP_DHT11_DATA_H;	//总线空闲保持高电平
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/*
 ************************************************************
 *  名称：	BSP_DHT11_Start
 *  功能：	DHT11开始传输
 *	输入：	无
 *	输出：  FALSE-失败 TRUE-成功
 ************************************************************
*/
bool BSP_DHT11_Start(void)
{
	BSP_DHT11_DATA_L;		//拉低总线
	BSP_DHT11_DelayMs_Port(20);	//拉低至少18ms
	BSP_DHT11_DATA_H;
	BSP_DHT11_DelayUs_Port(30);		//主机拉高20-40us
	
	//等待DHT11回应
	u8 timeout=0;
	while(BSP_DHT11_DATA_R && (timeout<10))	//DHT11会拉低40-80us
	{
		timeout++;
		BSP_DHT11_DelayUs_Port(10);
	}
	
	if(timeout >= 10)return FALSE;
	else timeout=0;
	
	while((!BSP_DHT11_DATA_R) && (timeout<10))
	{
		timeout++;
		BSP_DHT11_DelayUs_Port(10);
	}
	
	if(timeout >= 10) return FALSE;
	else return TRUE;
}

/*
 ************************************************************
 *  名称：	BSP_DHT11_ReadBit
 *  功能：	DHT11读取一位
 *	输入：	无
 *	输出：  读取到的数据
 ************************************************************
*/
u8 BSP_DHT11_ReadBit(void)
{
	u8 timeout=0;
	
	while(BSP_DHT11_DATA_R && (timeout<10))	//等待变为低电平!
	{
		timeout++;
		BSP_DHT11_DelayUs_Port(10);
	}
	
	timeout = 0;
	while((!BSP_DHT11_DATA_R) && (timeout<10))	//等待变为高电平!
	{
		timeout++;
		BSP_DHT11_DelayUs_Port(10);
	}
	
	BSP_DHT11_DelayUs_Port(40);	//等待40us
	
	if(BSP_DHT11_DATA_R) return 1;	//读取一位数据
	else return 0;
}

/*
 ************************************************************
 *  名称：	BSP_DHT11_ReadByte
 *  功能：	DHT11读取一个字节
 *	输入：	无
 *	输出：  读取到的数据
 ************************************************************
*/
u8 BSP_DHT11_ReadByte(void)
{
	u8 data=0;
	for(u8 i=0;i<8;i++)
	{
		data <<= 1;
		data |= BSP_DHT11_ReadBit();
	}
	return data;
}

/*
 ************************************************************
 *  名称：	BSP_DHT11_ReadByte
 *  功能：	DHT11读取温湿度
 *	输入：	temp-温度  humi-湿度
 *	输出：  FALSE-失败  TRUE-成功
 ************************************************************
*/
bool BSP_DHT11_ReadData(u8* temp,u8* humi)
{
	u8 buf[5]; 
	
	if(TRUE == BSP_DHT11_Start())
	{
		for(u8 i=0;i<5;i++)
		{
			buf[i] = BSP_DHT11_ReadByte();
		}
		
		if((buf[0]+buf[1]+buf[2]+buf[3]) == buf[4])
		{
			(*humi) = buf[0];
			(*temp) = buf[2];
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
}
































