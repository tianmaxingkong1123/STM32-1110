/**********************************************************************
* Copyright (c) 2002-2016 山东博科生物产业有限公司 All rights reserved.
*
* 文件名称： TMC429.c
* 描    述： TMC429相关程序，包含初始化,读写函数
*
*
* 修改记录：
*
* V1.0，2006.12.22，乔彬
**********************************************************************/
#include "usmart.h"
#include "usmart_str.h" 

////////////////////////////用户配置区///////////////////////////////////////////////
//这下面要包含所用到的函数所申明的头文件(用户自己添加) 
#include "delay.h"		
#include "sys.h"
#include "AT25128B.h"
#include "TMC429.h"
#include "driver.h"
#include "control.h"
#include "SPI.h"
#include "gpio.h"
#include "Encoder.h"


//#include "lcd.h"
												 
extern void led_set(u8 sta);
extern void test_fun(void(*ledset)(u8),u8 sta);
 
//函数名列表初始化(用户自己添加)
//用户直接在这里输入要执行的函数名及其查找串
struct _m_usmart_nametab usmart_nametab[]=
{
#if USMART_USE_WRFUNS==1 	//如果使能了读写操作
	(void*)read_addr,"u32 read_addr(u32 addr)",
	(void*)write_addr,"void write_addr(u32 addr,u32 val)",	 
#endif
	(void*)delay_ms,"void delay_ms(u16 nms)",
	(void*)delay_us,"void delay_us(u32 nus)",	
      
   
   (void*)WriteEepromByte,"u8 WriteEepromByte(u32 Address, u8 Value)",
   (void*)ReadEepromByte,"u8 ReadEepromByte(u32 Address)",
   (void*)ReadEepromBlock,"void ReadEepromBlock(u32 Address, u32 *Block, u32 Size)",
   (void*)WriteEepromBlock,"u8 WriteEepromBlock(u32 Address, u8 *Block, u32 Size)",
   (void*)Read429VERSION,"u32 Read429VERSION(void)",  
   (void*)Read429Status,"u8 Read429Status(void)",
   (void*)Write429Short,"void Write429Short(u8 Address, int Value)",  
   (void*)Write429Int,"void Write429Int(u8 Address, int Value)",   
   (void*)Set429RampMode,"void Set429RampMode(u8 Axis, u8 RampMode)",
   (void*)Read429Int,"int Read429Int(u8 Address)",
   (void*)Sys_Soft_Reset,"void Sys_Soft_Reset(void)",
   
   (void*) RotateRight,"void RotateRight(void)",    
   (void*) RotateLeft,"RotateLeft(void) ",
   (void*) MotorStop,"MotorStop(void)",
  // (void*) MoveToPosition,"MoveToPosition(void)",
      
   (void*) SystemControl,"void SystemControl(void)",
   (void*) ReadWriteSPI,"u8 ReadWriteSPI(u8 DeviceNumber, u8 TxData, u8 LastTransfer)",
   (void*) Get_Adc_Average,"u16 Get_Adc_Average(u8 ch,u8 times)",
   (void*) Can_usart,"void Can_usart(u8 id,u8 buf0,u8 buf1,u8 buf2,u32 buf3)",
   (void*) Get_Encoder,"u16 Get_Encoder(void)",  
   (void*) Set_Encoder_num, "void Set_Encoder_num(int num) ",
      /*
	(void*)LCD_Clear,"void LCD_Clear(u16 Color)",
	(void*)LCD_Fill,"void LCD_Fill(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 color)",
	(void*)LCD_DrawLine,"void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2)",
	(void*)LCD_DrawRectangle,"void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2)",
	(void*)LCD_Draw_Circle,"void Draw_Circle(u16 x0,u16 y0,u8 r)",
	(void*)LCD_ShowNum,"void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size)",
	(void*)LCD_ShowString,"void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p)",
	(void*)led_set,"void led_set(u8 sta)",
	(void*)test_fun,"void test_fun(void(*ledset)(u8),u8 sta)",				  	    
	(void*)LCD_ReadPoint,"u16 LCD_ReadPoint(u16 x,u16 y)",
   */
};						  
///////////////////////////////////END///////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
//函数控制管理器初始化
//得到各个受控函数的名字
//得到函数总数量
struct _m_usmart_dev usmart_dev=
{
	usmart_nametab,
	usmart_init,
	usmart_cmd_rec,
	usmart_exe,
	usmart_scan,
	sizeof(usmart_nametab)/sizeof(struct _m_usmart_nametab),//函数数量
	0,	  	//参数数量
	0,	 	//函数ID
	1,		//参数显示类型,0,10进制;1,16进制
	0,		//参数类型.bitx:,0,数字;1,字符串	    
	0,	  	//每个参数的长度暂存表,需要MAX_PARM个0初始化
	0,		//函数的参数,需要PARM_LEN个0初始化
};   



















