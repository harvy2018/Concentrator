/*
*********************************************************************************************************
*	                                  
*	ģ������ : ��ʱ��ģ��
*	�ļ����� : bsp_timer.c
*	��    �� : V2.0
*	˵    �� : ����systick�жϣ�ʵ�����ɸ������ʱ��
*	�޸ļ�¼ :
*		�汾��  ����       ����    ˵��
*		v0.1    2009-12-27 armfly  �������ļ���ST�̼���汾ΪV3.1.2
*		v1.0    2011-01-11 armfly  ST�̼���������V3.4.0�汾��
*       v2.0    2011-10-16 armfly  ST�̼���������V3.5.0�汾��
*
*	Copyright (C), 2010-2011, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#include "stm32f10x.h"
#include <stdio.h>

#include "bsp_timer.h"

#define TIMX      TIM2
#define TIMX_CLK  RCC_APB1Periph_TIM2
/* 
	�ڴ˶������ɸ������ʱ��ȫ�ֱ���
	ע�⣬��������__IO �� volatile����Ϊ����������жϺ���������ͬʱ�����ʣ��п�����ɱ����������Ż���
*/
#define TMR_COUNT	4		/* �����ʱ���ĸ�������1��������bsp_DelayMS()ʹ�� */

SOFT_TMR g_Tmr[TMR_COUNT];

/* ȫ������ʱ�䣬��λ10ms������uIP */
__IO int32_t g_iRunTime = 0;

static void bsp_SoftTimerDec(SOFT_TMR *_tmr);

void bsp_InitTimer(void)
{
	uint8_t i;
	
	/* �������е������ʱ�� */
	for (i = 0; i < TMR_COUNT; i++)
	{
		g_Tmr[i].count = 0;
		g_Tmr[i].flag = 0;
	}
	
	/* 
		����systic�ж�����Ϊ1ms��������systick�жϡ�
    	��������� \Libraries\CMSIS\CM3\CoreSupport\core_cm3.h 
    	
    	Systick�ж�������(\Libraries\CMSIS\CM3\DeviceSupport\ST\STM32F10x\startup\arm\
    		startup_stm32f10x_hd.s �ļ��ж���Ϊ SysTick_Handler��
    	SysTick_Handler������ʵ����stm32f10x_it.c �ļ���
    	SysTick_Handler����������SysTick_ISR()�������ڱ��ļ�ĩβ��
    */	
	SysTick_Config(SystemCoreClock / 1000);
}

/*
*********************************************************************************************************
*	�� �� ��: SysTick_ISR
*	����˵��: SysTick�жϷ������ÿ��1ms����1��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void SysTick_ISR(void)
{
	static u32 s_count = 0;
    extern u32 SecTick,Unix_Time;
	uint8_t i;

	for (i = 0; i < TMR_COUNT; i++)
	{
		bsp_SoftTimerDec(&g_Tmr[i]);
	}

	g_iRunTime++;	/* ȫ������ʱ��ÿ1ms��1 */	
	if (g_iRunTime == 0x7FFFFFFF)
	{
		g_iRunTime = 0;
	}
		
	if (++s_count >= 1000)
	{
		s_count = 0;
        SecTick++;
		
		//if(Unix_Time!=0)
			Unix_Time++;
		
	}
    
    AdcPro();
    
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_SoftTimerDec
*	����˵��: ÿ��1ms�����ж�ʱ��������1�����뱻SysTick_ISR�����Ե��á�
*	��    �Σ�_tmr : ��ʱ������ָ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void bsp_SoftTimerDec(SOFT_TMR *_tmr)
{
	if (_tmr->flag == 0)
	{
		if (_tmr->count > 0)
		{
			/* �����ʱ����������1�����ö�ʱ�������־ */
			if (--_tmr->count == 0)
			{
				_tmr->flag = 1;
			}
		}
	}
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_DelayMS
*	����˵��: ms���ӳ٣��ӳپ���Ϊ����1ms
*	��    �Σ�n : �ӳٳ��ȣ���λ1 ms�� n Ӧ����2
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_DelayMS(uint32_t n)
{
	/* ���� n = 1 �������������� */
	if (n <= 1)
	{
		n = 2;
	}
	
	__set_PRIMASK(1);  		/* ���ж� */
	g_Tmr[0].count = n;
	g_Tmr[0].flag = 0;
	__set_PRIMASK(0);  		/* ���ж� */

	while (1)
	{
		CPU_IDLE();	/* �˴��ǿղ������û����Զ��壬��CPU����IDLE״̬���Խ��͹��ģ���ʵ��ι�� */

		/* �ȴ��ӳ�ʱ�䵽 */
		if (g_Tmr[0].flag == 1)
		{
			break;
		}
	}
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_StartTimer
*	����˵��: ����һ����ʱ���������ö�ʱ���ڡ�
*	��    �Σ�	_id     : ��ʱ��ID��ֵ��1,TMR_COUNT-1�����û���������ά����ʱ��ID���Ա��ⶨʱ��ID��ͻ��
*						  ��ʱ��ID = 0 ������bsp_DelayMS()����
*				_period : ��ʱ���ڣ���λ1ms
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_StartTimer(uint8_t _id, uint32_t _period)
{
	if (_id >= TMR_COUNT)
	{
		/* while(1); ���� */
		return;
	}

	__set_PRIMASK(1);  		/* ���ж� */
	g_Tmr[_id].count = _period;
	g_Tmr[_id].flag = 0;
	__set_PRIMASK(0);  		/* ���ж� */
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_CheckTimer
*	����˵��: ��ⶨʱ���Ƿ�ʱ
*	��    �Σ�	_id     : ��ʱ��ID��ֵ��1,TMR_COUNT-1�����û���������ά����ʱ��ID���Ա��ⶨʱ��ID��ͻ��
*						  0 ����
*				_period : ��ʱ���ڣ���λ1ms
*	�� �� ֵ: ���� 0 ��ʾ��ʱδ���� 1��ʾ��ʱ��
*********************************************************************************************************
*/
uint8_t bsp_CheckTimer(uint8_t _id)
{
	if (_id >= TMR_COUNT)
	{
		return 0;
	}

	if (g_Tmr[_id].flag == 1)
	{
		g_Tmr[_id].flag = 0;
		return 1;
	}
	else
	{
		return 0;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_GetRunTime
*	����˵��: ��ȡCPU����ʱ�䣬��λ1ms
*	��    �Σ���
*	�� �� ֵ: CPU����ʱ�䣬��λ1ms
*********************************************************************************************************
*/
int32_t bsp_GetRunTime(void)
{
	int runtime; 

	__set_PRIMASK(1);  		/* ���ж� */
	
	runtime = g_iRunTime;	/* ������Systick�жϱ���д����˹��жϽ��б��� */
		
	__set_PRIMASK(0);  		/* ���ж� */

	return runtime;
}

TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
TIM_OCInitTypeDef  TIM_OCInitStructure;

uint16_t PrescalerValue = 0;

void TIM_Int(void) 
{
  PrescalerValue = (uint16_t) (SystemCoreClock / 12000000) - 1;
  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 65535;
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
  /* Prescaler configuration */
  TIM_PrescalerConfig(TIM2, PrescalerValue, TIM_PSCReloadMode_Immediate);

    /* Output Compare Timing Mode configuration: Channel1 */
//  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;
//  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;
//  TIM_OCInitStructure.TIM_Pulse = 60;
//  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
//
//  TIM_OC1Init(TIM2, &TIM_OCInitStructure);
//
//  TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);//TIM_OCPreload_Disable
//  TIM_ITConfig(TIM2, TIM_IT_CC1, ENABLE);
  TIM_Cmd(TIM2, !ENABLE);
}

void TIMX_Init(CALC_TYPE type)  
{  
    TIM_TimeBaseInitTypeDef TimX;  
    RCC_APB1PeriphClockCmd(TIMX_CLK,ENABLE);  
    TimX.TIM_Period=1; //???   
    if(type==CALC_TYPE_S) //��ʱ��SΪ��λʱ��ʱ��Ƶ��57600Hz���ⲿ��Ҫ1250�μ�ʱ   
    {  
        TimX.TIM_Prescaler=7200-1; //10KHZ 100us 
    }else if(type==CALC_TYPE_MS)  
    {  
        TimX.TIM_Prescaler=720-1; ////100KHz 10us,��ʱ������100��Ϊ1ms   
 
    }else if(type==CALC_TYPE_US)  
    {     
        TimX.TIM_Prescaler=(SystemCoreClock/1000000)-1; //1MHz ,����1��Ϊus   
    }else  
    {  
        TimX.TIM_Prescaler=7200-1;  //10KHZ
    }  
    TimX.TIM_ClockDivision=0;  
    TimX.TIM_CounterMode=TIM_CounterMode_Down; //���¼���   
    TIM_TimeBaseInit(TIMX,&TimX);         
}  
  
void TIMX_S_CALC(uint32_t s)  
{  
    u16 counter=(s*10000)&0xFFFF; //ǰ�ᶨʱ��ʱ��Ϊ10kHz   
    TIM_Cmd(TIMX,ENABLE);  
    TIM_SetCounter(TIMX,counter); //���ü���ֵ   
      
    while(counter>1)  
    {  
        counter=TIM_GetCounter(TIMX);  
    }  
    TIM_Cmd(TIMX,DISABLE);  
}  
  
void TIMX_MS_CALC(uint32_t ms)  
{  
    u16 counter=(ms*100)&0xFFFF;   //&0xFFFF ��ֹ���65535
    TIM_Cmd(TIMX,ENABLE);  
    TIM_SetCounter(TIMX,counter); //���ü���ֵ   
      
    while(counter>1)  
    {  
        counter=TIM_GetCounter(TIMX);  
    }  
    TIM_Cmd(TIMX,DISABLE);  
}  
  
void TIMX_US_CALC(uint32_t us)  
{  
    u16 counter=us&0xffff;  
    TIM_Cmd(TIMX,ENABLE);  
    TIM_SetCounter(TIMX,counter); //���ü���ֵ   
  
    while(counter>1)  
    {  
        counter=TIM_GetCounter(TIMX);  
    }  
    TIM_Cmd(TIMX,DISABLE);  
}  

//void uDelay(unsigned int usec)
//{
// 
//   TIM_Cmd(TIM2, ENABLE);
//
//	while(usec--) 
//	{
//		while(TIM2->SR&0x01==0);
//	//	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
//	}
//   TIM_Cmd(TIM2, !ENABLE);
//}

