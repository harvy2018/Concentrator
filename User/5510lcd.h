#ifndef __5110LCD_H__
#define __5110LCD_H__

#include "stm32f10x_gpio.h"



#define RES_PIN  GPIO_Pin_8

#define CE_PIN  GPIO_Pin_15
#define DC_PIN  GPIO_Pin_14
#define DIN_PIN  GPIO_Pin_13
#define CLK_PIN  GPIO_Pin_12
#define BACKLIGHT_PIN  GPIO_Pin_1




#define res0   GPIO_ResetBits(GPIOA, RES_PIN)  //��λ,0��λ
#define sce0   GPIO_ResetBits(GPIOB, CE_PIN)  //Ƭѡ

#define dc0    GPIO_ResetBits(GPIOB, DC_PIN)   //1д���ݣ�0дָ��

#define sdin0  GPIO_ResetBits(GPIOB, DIN_PIN)   //����
#define sclk0  GPIO_ResetBits(GPIOB, CLK_PIN)   //ʱ��
#define backled0  GPIO_ResetBits(GPIOB, BACKLIGHT_PIN)   //����


#define res1   GPIO_SetBits(GPIOA, RES_PIN)  //��λ,0��λ
#define sce1   GPIO_SetBits(GPIOB, CE_PIN)  //Ƭѡ

#define dc1    GPIO_SetBits(GPIOB, DC_PIN)   //1д���ݣ�0дָ��
#define sdin1  GPIO_SetBits(GPIOB, DIN_PIN)   //����
#define sclk1  GPIO_SetBits(GPIOB, CLK_PIN)   //ʱ��
#define backled1  GPIO_SetBits(GPIOB, BACKLIGHT_PIN)   //����

  
/*�������� ��������
GPIO_DeInit ���³�ʼ����Χ�豸GPIOx��ؼĴ���������Ĭ�ϸ�λֵ
GPIO_AFIODeInit ��ʼ��������(remap, event control�� EXTI ����) �Ĵ���
GPIO_Init ����GPIO_��ʼ���ṹָ����Ԫ�س�ʼ����Χ�豸GPIOx
GPIO_StructInit ���GPIO_��ʼ���ṹ��GPIO_InitStruct���ڵ�Ԫ��Ϊ��λֵ
GPIO_ReadInputDataBit ��ָ���˿�������������
GPIO_ReadInputData ��ָ���˿���������
GPIO_ReadOtputDataBit ��ָ���˿������������
GPIO_ReadOtputData ��ָ���˿��������
GPIO_SetBits ��1ָ���Ķ˿�����
GPIO_ResetBits ��0ָ���Ķ˿�����
GPIO_WriteBit ���û����ѡ������ݶ˿�����
GPIO_Write дָ�����ݵ�GPIOx�˿ڼĴ���
GPIO_ANAPinConfig ������ֹ GPIO 4 ģ������ģʽ
GPIO_PinLockConfig ����GPIO���żĴ���
GPIO_EventOutputConfig ѡ��GPIO������Ϊ�¼����
GPIO_EventOutputCmd ������ֹ�¼����
GPIO_PinRemapConfig �ı�ָ�����ŵ�Ӱ��
GPIO_EMIConfig ������ֹGPIO 8 �� 9 ��EMI ģʽ
GPIO_InitTypeDef GPIO_InitStructure;
//���̶���
#define   KEY_OK    GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) 
#define   KEY_DOWN   GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1)
#define   KEY_UP    GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2)
#define   KEY_ESC   GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3)	  
*/


void LCD_init(void);
void LCD_clear(void);
void LCD_write_String(unsigned char X,unsigned char Y,char *s);
void LCD_write_shu(unsigned char row, unsigned char page,unsigned char c); //row:�� page:ҳ dd:�ַ�
void LCD_write_hanzi(unsigned char row, unsigned char page,unsigned char c) ;//row:�� page:ҳ dd:�ַ�
#endif


