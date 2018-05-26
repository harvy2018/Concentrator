/*
*********************************************************************************************************
*	                                  
*	ģ������ : ��ʱ��ģ��
*	�ļ����� : bsp_timer.h
*	��    �� : V2.0
*	˵    �� : ͷ�ļ�
*
*	Copyright (C), 2010-2011, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __BSP_TIMER_H
#define __BSP_TIMER_H

/* Ŀǰ�ǿղ������û����Զ�����CPU����IDLE״̬�ĺ�����ι������ */
#define CPU_IDLE()
#include <stdint.h>

/* ��ʱ���ṹ�壬��Ա���������� volatile, ����C�������Ż�ʱ���������� */
typedef struct
{
	volatile uint32_t count;	/* ������ */
	volatile uint8_t flag;		/* ��ʱ�����־  */
}SOFT_TMR;



typedef enum {
  CALC_TYPE_S = 0,
  CALC_TYPE_MS,
  CALC_TYPE_US
 } CALC_TYPE;
/* ���ⲿ���õĺ������� */
void bsp_InitTimer(void);
void bsp_DelayMS(uint32_t n);
void bsp_StartTimer(uint8_t _id, uint32_t _period);
uint8_t bsp_CheckTimer(uint8_t _id);
int32_t bsp_GetRunTime(void);

void TIM_Int(void);
void uDelay(unsigned int usec);

void TIMX_Init(CALC_TYPE type) ;
void TIMX_S_CALC(uint32_t s) ;
void TIMX_MS_CALC(uint32_t ms);
void TIMX_US_CALC(uint32_t us);
 
void SysTick_ISR(void);

#endif