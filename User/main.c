/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   rtc ���ԣ���ʾʱ���ʽΪ: xx:xx:xx
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:���� F103-ָ���� STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
	
#include "stm32f10x.h"
#include "./usart/bsp_usart.h"
#include "./rtc/bsp_rtc.h"
#include "./lcd/bsp_ili9341_lcd.h"
#include "./key/bsp_key.h"  
#include "./beep/bsp_beep.h"   
#include "bsp_SysTick.h"
#include <stdio.h>
int xua;
int day_max;
// N = 2^32/365/24/60/60 = 136 ��

/*ʱ��ṹ�壬Ĭ��ʱ��2000-01-01 00:00:00*/
struct rtc_time systmtime=
{
0,0,0,1,1,2000,0
};

/*ʱ��ṹ�壬����ʱ��2000-01-01 00:00:08*/
struct rtc_time clocktime=
{
8,25,17,16,12,2019,0
};

extern __IO uint32_t TimeDisplay ;
extern __IO uint32_t TimeAlarm ;


//��*��ע�����
//��bsp_rtc.h�ļ��У�

//1.�����ú�USE_LCD_DISPLAY�����Ƿ�ʹ��LCD��ʾ
//2.�����ú�RTC_CLOCK_SOURCE_LSI��RTC_CLOCK_SOURCE_LSE����ʹ��LSE������LSI����

//3.STM32��LSE����Ҫ��ǳ��ϸ�ͬ���ĵ�·������������Ʒʱ����Щ��������⡣
//  ��ʵ����Ĭ��ʹ��LSI����
//  
//4.��������ϣ��RTC������Դ�������Ȼ���У���Ҫ��������ĵ�ز۰�װť�۵�أ�
//  �������Ҹĳ�ʹ���ⲿ����ģʽRTC_CLOCK_SOURCE_LSE
//  ť�۵���ͺţ�CR1220
/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main()
{		
	
	uint32_t clock_timestamp;
	uint32_t current_timestamp;
	
//��ʹ�øú������Ƿ�ʹ��Һ����ʾ
#ifdef  USE_LCD_DISPLAY
	
		ILI9341_Init ();         //LCD ��ʼ��
		LCD_SetFont(&Font8x16);
		LCD_SetColors(RED,BLACK);

		ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	/* ��������ʾȫ�� */

		ILI9341_DispStringLine_EN(LINE(1),"        xushuoshagou");
#endif
	
	  USART_Config();			
	
		Key_GPIO_Config();
	
		BEEP_GPIO_Config();
		
		/* ����RTC���ж����ȼ� */
	  RTC_NVIC_Config();
	  RTC_CheckAndConfig(&systmtime);
	
		/*�������ӼĴ���*/
		clock_timestamp = mktimev(&clocktime)-TIME_ZOOM;
		RTC_SetAlarm(clock_timestamp);
		
	  while (1)
	  {
			  
				
	    /* ÿ��1s ����һ��ʱ��*/
	    if (TimeDisplay == 1)
	    {
				/* ��ǰʱ�� */
			  //TimeDisplay = 1;
				//ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);
				current_timestamp = RTC_GetCounter();
	      Time_Display( current_timestamp,&systmtime); 		  	
	      TimeDisplay = 0;
	   }
			
			//���°�����ͨ�������޸�ʱ��
			if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON  )
			{
				struct rtc_time set_time;

				/*ʹ�ô��ڽ������õ�ʱ�䣬��������ʱע��ĩβҪ�ӻس�*/
				
			do 
	  {
			printf("\r\n  ������:");
			printf("\r\n  1.set clock time");
			printf("\r\n  2.set alarm clock time ");
			scanf("%d",&xua);
			if(xua==1)
			{
				Time_Regulate_Get(&set_time);
								/*�ý��յ���ʱ������RTC*/
				Time_Adjust(&set_time);
				
				//�򱸷ݼĴ���д���־
				BKP_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);

				break;
			}
			else if (xua==2)
			{	
				printf("\r\n=========================set alarm clock time==================");
				while(1){
				printf("\r\n set alarmclock yesr: ");				
        scanf("%d",&clocktime.tm_year);
							if(clocktime.tm_year>2038||clocktime.tm_year<1970)
				printf("\r\n you enter erro please rewrite!");
				else{
					printf("\r\n set success!");
					break;
				}
				}
				while(1){
				printf("\r\n set alarmclock mon: ");	
				scanf("%d",&clocktime.tm_mon);
					if(clocktime.tm_mon>12)
						printf("\r\n you enter erro please rewrite!");
					else
						break;
				}
			switch(clocktime.tm_mon)
			{
				case 1:
				case 3:
				case 5:
				case 7:
				case 8:
				case 10:
				case 12:					
						day_max = 31;
					break;
				
				case 4:
				case 6:
				case 9:
				case 11:
						day_max = 30;
					break;
				
				case 2:					
				     /*��������*/
						if((clocktime.tm_year%4==0) &&
							 ((clocktime.tm_year%100!=0) || (clocktime.tm_year%400==0)) &&
							 (clocktime.tm_mon>2)) 
								{
									day_max = 29;
								} else 
								{
									day_max = 28;
								}
					break;			
			}
			while(1){
				printf("\r\n set alarmclock day: ");	
				scanf("%d",&clocktime.tm_mday);
				if(day_max<clocktime.tm_mday)
				printf("\r\n you enter erro please rewrite!");
				else{
					printf("\r\n set success!");
					break;
				}
			}
			while(1){
				printf("\r\n set alarmclock hour: ");	
				scanf("%d",&clocktime.tm_hour);
					if(clocktime.tm_hour>23)
				printf("\r\n you enter erro please rewrite!");
				else{
					printf("\r\n set success!");
					break;
			}
		}
		while(1){
				printf("\r\n set alarmclock min: ");	
				scanf("%d",&clocktime.tm_min);
						if(clocktime.tm_min>59)
				printf("\r\n you enter erro please rewrite!");
				else{
					printf("\r\n set success!");
					break;
				}
			}
		while(1){
				printf("\r\n set alarmclock sec: ");	
				scanf("%d",&clocktime.tm_sec);
							if(clocktime.tm_sec>59)
				printf("\r\n you enter erro please rewrite!");
				else{
					printf("\r\n set success!");
					break;
				}
			}
				printf("%d",clocktime.tm_year);
				printf("%d",clocktime.tm_mon);
				printf("%d",clocktime.tm_mday);
				printf("%d",clocktime.tm_hour);
				printf("%d",clocktime.tm_min);
				printf("%d",clocktime.tm_sec);
		    clock_timestamp = mktimev(&clocktime)-TIME_ZOOM;
		    RTC_SetAlarm(clock_timestamp);
		
			}
		break;
	  }while(1);

			} 	
			if(clock_timestamp ==current_timestamp)
			{
			TimeAlarm =1;
			}
			//����
			if( TimeAlarm == 1)
			{
				BEEP(ON);
			}

			//���°������رշ�����
			if( Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON  )
			{
				BEEP(OFF);
				TimeAlarm = 0;
			}
			//Delay_ms(1000);	
	  }

}

/***********************************END OF FILE*********************************/

