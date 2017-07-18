/**********************************************************************
* Copyright (c) 2002-2016 ɽ�����������ҵ���޹�˾ All rights reserved.
*
* �ļ����ƣ� TMC429.c
* ��    ���� TMC429��س��򣬰�����ʼ��,��д����
*
*
* �޸ļ�¼��
*
* V1.0��2006.12.22���Ǳ�
**********************************************************************/
#include "usmart.h"
#include "usmart_str.h" 

////////////////////////////�û�������///////////////////////////////////////////////
//������Ҫ�������õ��ĺ�����������ͷ�ļ�(�û��Լ����) 
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
 
//�������б��ʼ��(�û��Լ����)
//�û�ֱ������������Ҫִ�еĺ�����������Ҵ�
struct _m_usmart_nametab usmart_nametab[]=
{
#if USMART_USE_WRFUNS==1 	//���ʹ���˶�д����
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
//�������ƹ�������ʼ��
//�õ������ܿغ���������
//�õ�����������
struct _m_usmart_dev usmart_dev=
{
	usmart_nametab,
	usmart_init,
	usmart_cmd_rec,
	usmart_exe,
	usmart_scan,
	sizeof(usmart_nametab)/sizeof(struct _m_usmart_nametab),//��������
	0,	  	//��������
	0,	 	//����ID
	1,		//������ʾ����,0,10����;1,16����
	0,		//��������.bitx:,0,����;1,�ַ���	    
	0,	  	//ÿ�������ĳ����ݴ��,��ҪMAX_PARM��0��ʼ��
	0,		//�����Ĳ���,��ҪPARM_LEN��0��ʼ��
};   



















